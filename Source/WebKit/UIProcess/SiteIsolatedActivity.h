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

#pragma once

#include "ProcessThrottler.h"

#include <WebCore/ProcessIdentifier.h>

namespace WebKit {

class RemotePageProxy;
class WebPageProxy;
class WebProcessProxy;

class SiteIsolatedActivity : public RefCountedAndCanMakeWeakPtr<SiteIsolatedActivity> {
    WTF_MAKE_TZONE_ALLOCATED(SiteIsolatedActivity);
    WTF_MAKE_NONCOPYABLE(SiteIsolatedActivity);
public:
    static Ref<SiteIsolatedActivity> create(WebPageProxy&, ASCIILiteral name, ProcessThrottlerActivityType);

    ~SiteIsolatedActivity();

    void addActivityForRemotePage(RemotePageProxy&);
    void removeActivityForRemotePage(RemotePageProxy&);

private:
    SiteIsolatedActivity(WebPageProxy&, ASCIILiteral name, ProcessThrottlerActivityType);

    Ref<ProcessThrottler::Activity> createActivity(WebProcessProxy&);

    WeakPtr<WebPageProxy> m_page;
    ASCIILiteral m_name;
    ProcessThrottlerActivityType m_type;
    HashMap<WebCore::ProcessIdentifier, Ref<ProcessThrottlerActivity>> m_activities;
};

}
