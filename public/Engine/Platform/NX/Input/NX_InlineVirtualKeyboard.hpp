#pragma once

#include <switch.h>

#include <functional>
#include <string_view>

#include <Engine/Input/IVirtualKeyboard.hpp>
#include <Engine/Core/Math/Vector2.hpp>

namespace engine::platform::nx {
    struct NXInlineVirtualKeyboard : public input::IVirtualKeyboard {
        NXInlineVirtualKeyboard() : b_IsInit{false}, b_IsVisible{false}, m_InlineKeyboard{} {}

        bool Initialize() override;

        void Destroy() override;

        void Update() override;

        void Show(bool show, input::IInputTarget* target) override;

        bool IsVisible() override {
            return b_IsVisible;
        }

        bool IsPointOnKeyboard(const core::math::Vector2& point) override;

        input::InputIgnoreTarget GetInputIgnoreTarget() const override {
            return input::InputIgnoreTarget::INPUT_IGNORE_TARGET_ALL;
        }
    protected:
        void OnStrChangeCallback(const char* str, SwkbdChangedStringArg* arg);
        void OnEnterCallback(const char* str, SwkbdDecidedEnterArg* arg);

        SwkbdInline m_InlineKeyboard;
        input::IInputTarget* m_Target;

        bool b_IsInit;
        bool b_IsVisible;
    };
}