#include <Engine/Platform/NX/NX_Thread.hpp>
#include <Engine/Runtime/Logger.hpp>

namespace engine::platform::nx {
    static runtime::Logger g_LoggerNXThread("NXThread");

    NXThread::NXThread() : m_Thread{}, b_IsRunning(false) {}

    NXThread::~NXThread() {
        Stop();
    }

    void NXThread::Start() {
        if (b_IsRunning) return;

        if (!m_TaskFunc) {
            g_LoggerNXThread.Log(runtime::LOG_LEVEL_ERROR, "No task function set for the thread");
            return;
        }

        Result rc = threadCreate(&m_Thread, ThreadFunction, this, nullptr, 0x4000, 0x3B, -2);
        if (R_FAILED(rc)) {
            g_LoggerNXThread.Log(runtime::LOG_LEVEL_ERROR, "Failed to create thread");
            return;
        }

        b_IsRunning = true;

        rc = threadStart(&m_Thread);
        if (R_FAILED(rc)) {
            g_LoggerNXThread.Log(runtime::LOG_LEVEL_ERROR, "Failed to start thread");
            threadClose(&m_Thread);
            return;
        }
    }

    void NXThread::Stop() {
        if (b_IsRunning) {
            b_IsRunning = false;
            threadWaitForExit(&m_Thread);
            threadClose(&m_Thread);
        }
    }

    void NXThread::Join() {
        if (b_IsRunning) {
            threadWaitForExit(&m_Thread);
        }
    }

    bool NXThread::IsRunning() const {
        return b_IsRunning;
    }

    void NXThread::SetName(std::string_view name) {
        m_Name = name;
    }

    std::string NXThread::GetName() const {
        return m_Name;
    }

    void NXThread::SetTaskFuncInternal(std::function<void()> task) {
        m_TaskFunc = std::move(task);
    }

    void NXThread::ThreadFunction(void *arg) {
        auto *thread = static_cast<NXThread *>(arg);

        if (thread) {
            g_LoggerNXThread.Log(runtime::LOG_LEVEL_DEBUG, "Begin TaskFunc for '%s'", thread->m_Name.c_str());

            if (thread->m_TaskFunc) {
                thread->m_TaskFunc();
            }

            g_LoggerNXThread.Log(runtime::LOG_LEVEL_DEBUG, "End TaskFunc for '%s'", thread->m_Name.c_str());

            thread->b_IsRunning = false;
        }
    }
}