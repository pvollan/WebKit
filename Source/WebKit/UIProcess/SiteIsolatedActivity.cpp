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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "SiteIsolatedActivity.h"

#include "BrowsingContextGroup.h"
#include "RemotePageProxy.h"
#include "WebPageProxy.h"

namespace WebKit {

WTF_MAKE_TZONE_ALLOCATED_IMPL(SiteIsolatedActivity);

Ref<SiteIsolatedActivity> SiteIsolatedActivity::create(WebPageProxy& page, ASCIILiteral name, ProcessThrottlerActivityType type)
{
    return adoptRef(*new SiteIsolatedActivity(page, name, type));
}

SiteIsolatedActivity::SiteIsolatedActivity(WebPageProxy& page, ASCIILiteral name, ProcessThrottlerActivityType type)
    : m_page(page)
    , m_name(name)
    , m_type(type)
{
    RefPtr protectedPage = m_page;

    Ref process = protectedPage->siteIsolatedProcess();
    m_activities.add(process->coreProcessIdentifier(), createActivity(process));

    Ref group = protectedPage->browsingContextGroup();
    group->forEachRemotePage(page, [this](auto& remotePageProxy) {
        Ref process = remotePageProxy.process();
        m_activities.add(process->coreProcessIdentifier(), createActivity(process));
    });

    protectedPage->addSiteIsolatedActivity(*this);
}

SiteIsolatedActivity::~SiteIsolatedActivity()
{
    if (RefPtr protectedPage = m_page)
        protectedPage->addSiteIsolatedActivity(*this);
}

Ref<ProcessThrottler::Activity> SiteIsolatedActivity::createActivity(WebProcessProxy& process)
{
    Ref protectedProcess = process;
    if (m_type == ProcessThrottlerActivityType::Foreground)
        return protectedProcess->throttler().foregroundActivity(m_name);
    return protectedProcess->throttler().backgroundActivity(m_name);
}

void SiteIsolatedActivity::addActivityForRemotePage(RemotePageProxy& remotePageProxy)
{
    auto& process = remotePageProxy.process();
    Ref<ProcessThrottler::Activity> activity = createActivity(process);
    m_activities.set(process.coreProcessIdentifier(), activity);

}

void SiteIsolatedActivity::removeActivityForRemotePage(RemotePageProxy& remotePageProxy)
{
    auto& process = remotePageProxy.process();
    m_activities.remove(process.coreProcessIdentifier());
}

}
