/*
 * Copyright (C) 2026 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#import "config.h"
#import "ProcessStartupSandboxExtensions.h"

#import <pal/spi/cocoa/CoreServicesSPI.h>
#import <wtf/FileSystem.h>

namespace WebKit {

void ProcessStartupSandboxExtensions::createSandboxExtensions(ASCIILiteral processName)
{
    String bundlePath = webKitBundlePath();
    if (auto handle = SandboxExtension::createHandle(bundlePath, SandboxExtension::Type::ReadOnly))
        webKitBundleDirectoryExtension = WTF::move(*handle);
    createCacheDirectorySandboxExtension(processName);
    createTempDirectorySandboxExtension(processName);
}

void ProcessStartupSandboxExtensions::createCacheDirectorySandboxExtension(ASCIILiteral processName)
{
    char cacheDirectory[PATH_MAX] = { 0 };
    if (__user_local_dirname(getuid(), DIRHELPER_USER_LOCAL_CACHE, cacheDirectory, PATH_MAX)) {
        auto cacheDirectorySpan = unsafeSpan(cacheDirectory);
        String cacheDirectoryForProcess = FileSystem::pathByAppendingComponent(cacheDirectorySpan, processName);
        if (auto handle = SandboxExtension::createHandle(cacheDirectoryForProcess, SandboxExtension::Type::ReadOnly))
            cacheDirectoryExtension = WTF::move(*handle);
    }
}

void ProcessStartupSandboxExtensions::createTempDirectorySandboxExtension(ASCIILiteral processName)
{
    char tempDirectory[PATH_MAX] = { 0 };
    if (__user_local_dirname(getuid(), DIRHELPER_USER_LOCAL_TEMP, tempDirectory, PATH_MAX)) {
        auto tempDirectorySpan = unsafeSpan(tempDirectory);
        String tempDirectoryForProcess = FileSystem::pathByAppendingComponent(tempDirectorySpan, processName);
        if (auto handle = SandboxExtension::createHandle(tempDirectoryForProcess, SandboxExtension::Type::ReadOnly))
            tempDirectoryExtension = WTF::move(*handle);
    }
}

void ProcessStartupSandboxExtensions::consumeSandboxExtensions() const
{
    SandboxExtension::consumePermanently(webKitBundleDirectoryExtension);
    SandboxExtension::consumePermanently(cacheDirectoryExtension);
    SandboxExtension::consumePermanently(tempDirectoryExtension);
}

String ProcessStartupSandboxExtensions::webKitBundlePath() const
{
    RetainPtr<NSBundle> bundle = [NSBundle bundleForClass:NSClassFromString(@"WKWebView")];
    String bundlePath = bundle.get().bundlePath;
    if (!bundlePath.startsWith("/System/Library/Frameworks"_s))
        bundlePath = bundle.get().bundlePath.stringByDeletingLastPathComponent;
    return bundlePath;
}

}
