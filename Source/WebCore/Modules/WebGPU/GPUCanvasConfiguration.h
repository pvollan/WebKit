/*
 * Copyright (C) 2021-2023 Apple Inc. All rights reserved.
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

#include "GPUCanvasAlphaMode.h"
#include "GPUCanvasToneMapping.h"
#include "GPUDevice.h"
#include "GPUPredefinedColorSpace.h"
#include "GPUTextureFormat.h"
#include "GPUTextureUsage.h"
#include "WebGPUCanvasConfiguration.h"
#include <wtf/Vector.h>

namespace WebCore {

struct GPUCanvasConfiguration {
    WebGPU::CanvasConfiguration convertToBacking(bool reportValidationErrors) const
    {
        ASSERT(device);
        return {
            device->backing(),
            WebCore::convertToBacking(format),
            convertTextureUsageFlagsToBacking(usage),
            viewFormats.map([](auto& viewFormat) {
                return WebCore::convertToBacking(viewFormat);
            }),
            WebCore::convertToBacking(colorSpace),
            WebCore::convertToBacking(toneMapping.mode),
            WebCore::convertToBacking(alphaMode),
            reportValidationErrors,
        };
    }

    WeakPtr<GPUDevice, WeakPtrImplWithEventTargetData> device;
    GPUTextureFormat format { GPUTextureFormat::R8unorm };
    GPUTextureUsageFlags usage { GPUTextureUsage::RENDER_ATTACHMENT };
    Vector<GPUTextureFormat> viewFormats;
    GPUPredefinedColorSpace colorSpace { GPUPredefinedColorSpace::SRGB };
    GPUCanvasToneMapping toneMapping;
    GPUCanvasAlphaMode alphaMode { GPUCanvasAlphaMode::Opaque };
};

}
