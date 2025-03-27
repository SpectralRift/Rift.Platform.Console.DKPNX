#pragma once

#include <Engine/Core/Runtime/IMutex.hpp>

#include <switch.h>

namespace engine::platform::nx {
    struct NXMutex : public core::runtime::IMutex {
        NXMutex();

        void Lock() override;
        void Unlock() override;
    protected:
        RMutex m_Mutex;
    };
}