#include <Engine/Platform/NX/NX_Mutex.hpp>

namespace engine::platform::nx {
    NXMutex::NXMutex() {
        rmutexInit(&m_Mutex);
    }

    void NXMutex::Lock() {
        rmutexLock(&m_Mutex);
    }

    void NXMutex::Unlock() {
        rmutexUnlock(&m_Mutex);
    }
}