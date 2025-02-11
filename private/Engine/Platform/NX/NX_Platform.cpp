#include <switch.h>

#include <Engine/Platform/NX/NX_Window.hpp>
#include <Engine/Platform/NX/NX_Thread.hpp>

#include <Engine/Platform/NX/Input/NX_TouchDevice.hpp>
#include <Engine/Platform/NX/Input/NX_InputProvider.hpp>

#include <Engine/Core/Platform.hpp>
#include <Engine/Core/Runtime/Input/InputManager.hpp>

namespace engine::core::Platform {
    static std::unique_ptr<platform::nx::NXTouchDevice> g_NXTouchDevice;
    static std::shared_ptr<platform::nx::NXInputProvider> g_NXInputProvider;

    std::string GetName() {
        return "Nintendo Switch";
    }

    void ShowMessageBox(std::string_view title, std::string_view message) {
        auto finalMessage = std::string{title} + "\n\n" + std::string{message};

        ErrorApplicationConfig c;
        errorApplicationCreate(&c, finalMessage.data(), 0);
        errorApplicationShow(&c);
    }

    std::unique_ptr<runtime::AppHost> GetSuitableHost() {
        return nullptr;
    }

    std::unique_ptr<runtime::IWindow> CreateAppWindow() {
        return std::make_unique<platform::nx::NXWindow>();
    }

    std::unique_ptr<runtime::IThread> CreateThread() {
        return std::make_unique<platform::nx::NXThread>();
    }

    void Init() {
        // mount ROMFS
        romfsInit();

        socketInitializeDefault();
        nxlinkStdio(); // stdout/stderr > NETWORK

        // initialize touch
        hidInitializeTouchScreen();
    }

    void PreEngineInit() {
        // initialize touch
        g_NXTouchDevice = std::make_unique<platform::nx::NXTouchDevice>();
        runtime::input::InputManager::Instance()->RegisterDevice(g_NXTouchDevice.get());

        // initialize input provider for joycons
        g_NXInputProvider = std::make_shared<platform::nx::NXInputProvider>();
        g_NXInputProvider->Init();
    }

    void Shutdown() {
        if(g_NXTouchDevice) {
            runtime::input::InputManager::Instance()->UnregisterDevice(g_NXTouchDevice.get());
            g_NXTouchDevice = nullptr;
        }

        if(g_NXInputProvider) {
            g_NXInputProvider->Shutdown();
            g_NXInputProvider = nullptr;
        }

        romfsExit();
        socketExit();
    }
}