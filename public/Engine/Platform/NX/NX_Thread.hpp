#pragma once

#include <string>
#include <switch.h>

#include <Engine/Core/Runtime/IThread.hpp>

namespace engine::platform::nx {
    struct NXThread : public core::runtime::IThread {
        NXThread();

        ~NXThread() override;

        void Start() override;

        void Stop() override;

        void Join() override;

        bool IsRunning() const override;

        void SetName(std::string_view name) override;

        std::string GetName() const override;

    protected:
        void SetTaskFuncInternal(std::function<void()> task) override;

    private:
        static void ThreadFunction(void *arg);

        Thread m_Thread;
        bool b_IsRunning;
        bool b_IsCloseRequested;
        std::string m_Name;
        std::function<void()> m_TaskFunc;
    };
}