/*
 * Copyright (C) 2025 Apple Inc. All rights reserved.
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

#pragma once

enum {
    kXTScopeNone    = 0,
    kXTScopeProcess = 1UL << 0,
    kXTScopeUser    = 1UL << 1,
    kXTScopeSession = 1UL << 2,
    kXTScopeDefault = kXTScopeProcess,
    kXTScopeGlobal  = kXTScopeUser | kXTScopeSession,
    kXTScopeAll     = kXTScopeProcess | kXTScopeUser | kXTScopeSession,
};

typedef uint32_t XTScope;

enum {
    kXTOptionsDefault                   = 0,
    kXTOptionsPreferAppleSystemFonts    = 1 << 0,
    kXTOptionsRemoveDuplicateFonts      = 1 << 1,
    kXTOptionsRemoveInvisibleFonts      = 1 << 2,
    kXTOptionsRestrictToPostScriptName  = 1 << 3,
    kXTOptionsDoNotSortResults          = 1 << 4,
    kXTOptionsIncludeDisabledFonts      = 1 << 5,
    kXTOptionsDisableFontProviders      = 1 << 16,
    kXTOptionsDisableAutoActivation     = 1 << 17
};
typedef uint32_t XTOptions;
