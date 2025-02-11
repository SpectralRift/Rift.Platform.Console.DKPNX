#pragma once

#include <string>
#include <stdexcept>

#include <Engine/Core/Runtime/IWindow.hpp>

struct NWindow;

namespace engine::platform::nx {
    struct NXWindow : public core::runtime::IWindow {
        NXWindow() : h_Window(nullptr) {}

        bool Create() override;

        void Destroy() override;

        void SetTitle(std::string_view title) override;

        bool IsValid() override;

        void SetPosition(const core::math::Vector2 &position) override;

        void SetSize(const core::math::Vector2 &size) override;

        core::math::Vector2 GetPosition() override;

        core::math::Vector2 GetSize() override;

        void ProcessEvents() override;

        void Show() override;

        void Hide() override;

        void *GetHandle() override;

        void *GetDeviceCtx() override;

        core::runtime::graphics::IGraphicsContext *GetGraphicsContext() override {
            return h_GraphicsContext.get();
        }
    protected:
        NWindow* h_Window;
        std::unique_ptr<core::runtime::graphics::IGraphicsContext> h_GraphicsContext;
    };
}