#include <Engine/Platform/NX/NX_Window.hpp>
#include <Engine/Platform/Universal/Graphics/U_EGL_Context.hpp>

#include <Engine/Runtime/Logger.hpp>

#include <switch.h>

namespace engine::platform::nx {
    static runtime::Logger g_LoggerNXWindow("NXWindow");
    
    bool NXWindow::Create() {
        h_Window = nwindowGetDefault();

        h_GraphicsContext = std::make_unique<universal::UEGLContext>(this);

        if (!h_GraphicsContext->Create()) {
            g_LoggerNXWindow.Log(runtime::LOG_LEVEL_ERROR, "Failed to create a valid EGL context!\n");
            return false;
        }

        return true;
    }

    void NXWindow::Destroy() {
        // do not destroy the window!
    }

    void NXWindow::SetTitle(std::string_view title) {
        // no op;
    }

    bool NXWindow::IsValid() {
        return h_Window != nullptr;
    }

    void NXWindow::SetPosition(const core::math::Vector2 &position) {
        // no op; platform can't move window
    }

    void NXWindow::SetSize(const core::math::Vector2 &size) {
        nwindowSetDimensions(h_Window, (int) size.x, (int) size.y);
    }

    core::math::Vector2 NXWindow::GetPosition() {
        return {0, 0};
    }

    core::math::Vector2 NXWindow::GetSize() {
        unsigned int width = 0, height = 0;
        nwindowGetDimensions(h_Window, &width, &height);
        return {(float) width, (float) height};
    }

    void NXWindow::ProcessEvents() {
        appletMainLoop();
    }

    void NXWindow::Show() {

    }

    void NXWindow::Hide() {

    }

    void *NXWindow::GetHandle() {
        return h_Window;
    }

    void *NXWindow::GetDeviceCtx() {
        return nullptr;
    }
}