#include <switch.h>

#include <Engine/Platform/NX/NX_Window.hpp>
#include <Engine/Platform/NX/NX_Thread.hpp>
#include <Engine/Platform/NX/NX_Mutex.hpp>
#include <Engine/Platform/NX/Input/NX_TouchDevice.hpp>
#include <Engine/Platform/NX/Input/NX_InlineVirtualKeyboard.hpp>
#include <Engine/Platform/NX/Input/NX_InputProvider.hpp>
#include <Engine/Core/Platform.hpp>
#include <Engine/Core/Runtime/BasicAppHost.hpp>
#include <Engine/Input/InputManager.hpp>

std::shared_ptr<engine::platform::nx::NXTouchDevice> g_NXTouchDevice;
std::shared_ptr<engine::platform::nx::NXInputProvider> g_NXInputProvider;
std::shared_ptr<engine::platform::nx::NXInlineVirtualKeyboard> g_NXVirtualKeyboard;

namespace engine::core::Platform {
    std::string GetName() {
        return "Nintendo Switch";
    }

    void ShowMessageBox(std::string_view title, std::string_view message) {
        auto finalMessage = std::string{title} + "\n\n" + std::string{message};

        ErrorApplicationConfig c;
        errorApplicationCreate(&c, finalMessage.data(), 0);
        errorApplicationShow(&c);
    }

    std::shared_ptr<input::IVirtualKeyboard> GetVirtualKeyboard() {
        return g_NXVirtualKeyboard;
    }

    std::unique_ptr<runtime::AppHost> GetSuitableHost() {
        return std::make_unique<runtime::BasicAppHost>();
    }

    std::unique_ptr<runtime::IWindow> CreateAppWindow() {
        return std::make_unique<platform::nx::NXWindow>();
    }

    std::unique_ptr<runtime::IThread> CreateThread() {
        return std::make_unique<platform::nx::NXThread>();
    }

    std::unique_ptr<runtime::IMutex> CreateMutex() {
        return std::make_unique<platform::nx::NXMutex>();
    }
}