#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/Runtime/IThread.hpp>
#include <Engine/Input/IInputDevice.hpp>

namespace engine::platform::nx {
    struct NXControllerDevice;

    struct NXInputProvider {
        void Init();
        void Shutdown();

        void SetMaxLocalPlayers(int numPlayers);

        // show multiplayer configuration
        void ReconfigureLocalMP();
    protected:
        void ProcessTask();

        bool b_IsInit;
        int m_MaxLocalPlayers;
        std::unique_ptr<core::runtime::IThread> m_CtrlDetectThread;
        std::vector<std::shared_ptr<NXControllerDevice>> m_PlayerDevices;
    };
}