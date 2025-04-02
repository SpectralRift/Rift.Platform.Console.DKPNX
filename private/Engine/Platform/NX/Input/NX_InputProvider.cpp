#include <switch.h>

#include <Engine/Platform/NX/Input/NX_InputProvider.hpp>
#include <Engine/Platform/NX/Input/NX_ControllerDevice.hpp>

#include <Engine/Input/InputManager.hpp>
#include <Engine/Core/Platform.hpp>
#include <Engine/Runtime/Logger.hpp>

namespace engine::platform::nx {
    static runtime::Logger g_LoggerNXInputProvider("NXInputProvider");
    
    void NXInputProvider::Init() {
        g_LoggerNXInputProvider.Log(runtime::LOG_LEVEL_DEBUG, "Initializing Input Provider for NX Platform...\n");

        // single player by default
        SetMaxLocalPlayers(1);

        m_CtrlDetectThread = core::Platform::CreateThread();
        if (!m_CtrlDetectThread) {
            g_LoggerNXInputProvider.Log(runtime::LOG_LEVEL_ERROR, "Could not create controller detection thread!\n");
            return;
        }

        m_CtrlDetectThread->SetName("NX Controller Detection Thread");
        m_CtrlDetectThread->SetTaskFunc(&NXInputProvider::ProcessTask, this);
        m_CtrlDetectThread->Start();

        g_LoggerNXInputProvider.Log(runtime::LOG_LEVEL_INFO, "Input Provider for NX Platform initialized successfully!\n");

        b_IsInit = true;
    }

    void NXInputProvider::ProcessTask() {
        while(m_CtrlDetectThread->IsRunning()) {
            if(!b_IsInit) {
                continue;
            }

            if(m_PlayerDevices.size() != m_MaxLocalPlayers) {
                ReconfigureLocalMP();
                continue;
            }

            for(const auto& d: m_PlayerDevices) {
                if(!d->IsConnected()) {
                    g_LoggerNXInputProvider.Log(runtime::LOG_LEVEL_DEBUG, "Player %i disconnected!\n", d->GetPlayerId());

                    // I think I should call ReconfigureLocalMP from the main thread.
                    ReconfigureLocalMP();
                }
            }
        }
    }

    void NXInputProvider::Shutdown() {
        b_IsInit = false;

        if(m_CtrlDetectThread) {
            m_CtrlDetectThread->Stop();
            m_CtrlDetectThread->Join();
        }

        if(!m_PlayerDevices.empty()) {
            for(auto& d : m_PlayerDevices) {
                input::InputManager::Instance()->UnregisterDevice(d.get());
            }

            m_PlayerDevices.clear();
        }
    }

    void NXInputProvider::SetMaxLocalPlayers(int numPlayers) {
        m_MaxLocalPlayers = numPlayers;

        padConfigureInput(m_MaxLocalPlayers, HidNpadStyleSet_NpadStandard);
        ReconfigureLocalMP();
    }

    void NXInputProvider::ReconfigureLocalMP() {
        if(m_CtrlDetectThread && !m_CtrlDetectThread->IsRunning()) {
            return;
        }

        g_LoggerNXInputProvider.Log(runtime::LOG_LEVEL_DEBUG, "ReconfigureLocalMP called!\n");

        // show config only for more than one player
        if(m_MaxLocalPlayers > 1) {
            HidLaControllerSupportResultInfo info;
            HidLaControllerSupportArg args;
            hidLaCreateControllerSupportArg(&args);

            args.hdr.player_count_max = (s8) m_MaxLocalPlayers;

            hidLaShowControllerSupportForSystem(&info, &args, false);
        }

        // if we have devices allocated, we unregister them and delete them
        if(!m_PlayerDevices.empty()) {
            for(auto& d : m_PlayerDevices) {
                input::InputManager::Instance()->UnregisterDevice(d.get());
            }

            m_PlayerDevices.clear();
        }

        for(int i = 0; i < m_MaxLocalPlayers; i++) {
            auto c = std::make_shared<NXControllerDevice>((HidNpadIdType) i);
            int tries = 0;

            // when you switch controllers, you have to keep trying until you get a valid hit. usually it takes under 16 tries to fetch a controller
            while(tries < 16) {
                g_LoggerNXInputProvider.Log(runtime::LOG_LEVEL_DEBUG, "Trying to initialize controller for player %i... (try %i)\n", i + 1, tries);

                if(c->Initialize()) {
                    g_LoggerNXInputProvider.Log(runtime::LOG_LEVEL_INFO, "Controller for player %i initialized!\n", i + 1);

                    input::InputManager::Instance()->RegisterDevice(c.get());
                    m_PlayerDevices.push_back(std::move(c));
                    break;
                }

                tries++;
            }
        }
    }
}