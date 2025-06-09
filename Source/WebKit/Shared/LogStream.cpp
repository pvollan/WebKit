/* Copyright (C) 2024 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "LogStream.h"

#if ENABLE(LOGD_BLOCKING_IN_WEBCONTENT)

#include <wtf/OSObjectPtr.h>

#if HAVE(OS_SIGNPOST)
#include <wtf/SystemTracing.h>
#endif

#include "LogStreamMessages.h"
#include "Logging.h"
#include "StreamConnectionWorkQueue.h"
#include "StreamServerConnection.h"

#define MESSAGE_CHECK(assertion, connection) MESSAGE_CHECK_BASE(assertion, connection)

namespace WebKit {

static std::atomic<unsigned> globalLogCountForTesting { 0 };

LogStream::LogStream(int32_t pid, LogStreamIdentifier logStreamIdentifier)
    : m_logStreamIdentifier(logStreamIdentifier)
    , m_pid(pid)
{
}

LogStream::~LogStream()
{
}

void LogStream::stopListeningForIPC()
{
    assertIsMainRunLoop();
#if ENABLE(STREAMING_IPC_IN_LOG_FORWARDING)
    if (RefPtr logStreamConnection = m_logStreamConnection)
        logStreamConnection->stopReceivingMessages(Messages::LogStream::messageReceiverName(), m_logStreamIdentifier.toUInt64());
#endif
}

os_log_t LogStream::osLogObject(std::span<const uint8_t> logChannel, std::span<const uint8_t> logCategory)
{
    String logChannelString(logChannel);
    String logCategoryString(logCategory);
    if (auto log = m_osLogs.getOptional({ logChannelString, logCategoryString }))
        return log->get();
    auto subsystem = byteCast<char>(logChannel.data());
    auto category = byteCast<char>(logCategory.data());
    OSObjectPtr<os_log_t> log = adoptOSObject(os_log_create(subsystem, category));
    m_osLogs.set({ logChannelString, logCategoryString }, log);
    return log.get();
}

void LogStream::logOnBehalfOfWebContent(std::span<const uint8_t> logSubsystem, std::span<const uint8_t> logCategory, std::span<const uint8_t> nullTerminatedLogString, uint8_t logType)
{
#if ENABLE(STREAMING_IPC_IN_LOG_FORWARDING)
    ASSERT(!isMainRunLoop());
#endif

    auto isNullTerminated = [](std::span<const uint8_t> view) {
        return view.data() && !view.empty() && view.back() == '\0';
    };

    bool isValidLogType = logType == OS_LOG_TYPE_DEFAULT || logType == OS_LOG_TYPE_INFO || logType == OS_LOG_TYPE_DEBUG || logType == OS_LOG_TYPE_ERROR || logType == OS_LOG_TYPE_FAULT;
#if ENABLE(STREAMING_IPC_IN_LOG_FORWARDING)
    MESSAGE_CHECK(isNullTerminated(nullTerminatedLogString) && isValidLogType, m_logStreamConnection->connection());
#else
    RefPtr logConnection = m_logConnection.get();
    MESSAGE_CHECK(isNullTerminated(nullTerminatedLogString) && isValidLogType, logConnection);
#endif

    // os_log_hook on sender side sends a null category and subsystem when logging to OS_LOG_DEFAULT.
    OSObjectPtr<os_log_t> osLog;
    if (isNullTerminated(logSubsystem) && isNullTerminated(logCategory)) {
        if (equalSpans("Testing\0"_span, logCategory))
            globalLogCountForTesting++;
        osLog = osLogObject(logSubsystem, logCategory);
    }

    auto osLogPointer = osLog.get() ? osLog.get() : OS_LOG_DEFAULT;

#if HAVE(OS_SIGNPOST)
    if (WTFSignpostHandleIndirectLog(osLogPointer, m_pid, byteCast<char>(nullTerminatedLogString)))
        return;
#endif

    // Use '%{public}s' in the format string for the preprocessed string from the WebContent process.
    // This should not reveal any redacted information in the string, since it has already been composed in the WebContent process.
    os_log_with_type(osLogPointer, static_cast<os_log_type_t>(logType), "WP[PID=%d] %{public}s", m_pid, byteCast<char>(nullTerminatedLogString).data());
}

#if ENABLE(STREAMING_IPC_IN_LOG_FORWARDING)
void LogStream::setup(IPC::StreamServerConnectionHandle&& serverConnection, CompletionHandler<void(IPC::Semaphore& streamWakeUpSemaphore, IPC::Semaphore& streamClientWaitSemaphore)>&& completionHandler)
{
    m_logStreamConnection = IPC::StreamServerConnection::tryCreate(WTFMove(serverConnection), { });

    static NeverDestroyed<Ref<IPC::StreamConnectionWorkQueue>> logQueue = IPC::StreamConnectionWorkQueue::create("Log work queue"_s);

    if (RefPtr logStreamConnection = m_logStreamConnection) {
        logStreamConnection->open(logQueue.get());
        logStreamConnection->startReceivingMessages(*this, Messages::LogStream::messageReceiverName(), m_logStreamIdentifier.toUInt64());
        completionHandler(logQueue.get()->wakeUpSemaphore(), logStreamConnection->clientWaitSemaphore());
    }
}
#else
void LogStream::setup(IPC::Connection& connection)
{
    m_logConnection = connection;
}
#endif

unsigned LogStream::logCountForTesting()
{
    return globalLogCountForTesting;
}

#if __has_include("LogMessagesImplementations.h")
#include "LogMessagesImplementations.h"
#endif

}

#endif
