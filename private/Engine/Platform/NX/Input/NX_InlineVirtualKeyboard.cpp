#define SHOW_PRIVATE_API

#include <Engine/Platform/NX/Input/NX_InlineVirtualKeyboard.hpp>

#include <Engine/Input/IInputTarget.hpp>

#include <Engine/Runtime/Logger.hpp>
#include <Engine/Core/Platform.hpp>

namespace engine::platform::nx {
    static runtime::Logger g_LoggerNXInlineVirtualKeyboard("NXInlineVirtualKeyboard");

    bool NXInlineVirtualKeyboard::Initialize() {
        if (b_IsInit) {
            g_LoggerNXInlineVirtualKeyboard.Log(runtime::LOG_LEVEL_ERROR, "Inline Virtual keyboard is already initialized!");
            return false;
        }

        g_LoggerNXInlineVirtualKeyboard.Log(runtime::LOG_LEVEL_DEBUG, "Initializing inline virtual keyboard...");

        Result rc = swkbdInlineCreate(&m_InlineKeyboard);

        if (!R_SUCCEEDED(rc)) {
            g_LoggerNXInlineVirtualKeyboard.Log(runtime::LOG_LEVEL_ERROR, "Failed to create inline virtual keyboard.");
            return false;
        }

        swkbdInlineLaunchForLibraryApplet(&m_InlineKeyboard, SwkbdInlineMode_AppletDisplay, 0);
        swkbdInlineSetUtf8Mode(&m_InlineKeyboard, true);

        // ah yes! callbacks! unfortunately, I cannot use a lambda function with a ref to this here
        // sooo the only way to make it work is to make a static function which will refer to core::Platform::GetVirtualKeyboard()
        // which is exactly an instance of NXInlineVirtualKeyboard aka this.
        swkbdInlineSetChangedStringCallback(&m_InlineKeyboard, [](const char *str, SwkbdChangedStringArg *arg) {
            ((NXInlineVirtualKeyboard*) core::Platform::GetVirtualKeyboard().get())->OnStrChangeCallback(str, arg);
        });

        swkbdInlineSetDecidedEnterCallback(&m_InlineKeyboard, [](const char *str, SwkbdDecidedEnterArg *arg) {
            ((NXInlineVirtualKeyboard*) core::Platform::GetVirtualKeyboard().get())->OnEnterCallback(str, arg);
        });

        swkbdInlineSetDecidedCancelCallback(&m_InlineKeyboard, []() {
            g_LoggerNXInlineVirtualKeyboard.Log(runtime::LOG_LEVEL_WARNING, "Cancelled inline virtual keyboard input.");
            ((NXInlineVirtualKeyboard*) core::Platform::GetVirtualKeyboard().get())->b_IsVisible = false;
        });

        b_IsInit = true;

        return true;
    }

    void NXInlineVirtualKeyboard::OnStrChangeCallback(const char *str, SwkbdChangedStringArg *arg) {
        if (m_Target != nullptr) {
            m_Target->SetText({str}, false);
        } else {
            g_LoggerNXInlineVirtualKeyboard.Log(runtime::LOG_LEVEL_WARNING, "OnStrChangeCallback: Virtual keyboard target is null!");
        }
    }

    void NXInlineVirtualKeyboard::OnEnterCallback(const char *str, SwkbdDecidedEnterArg *arg) {
        if (m_Target != nullptr) {
            m_Target->SetText({str}, true);
        } else {
            g_LoggerNXInlineVirtualKeyboard.Log(runtime::LOG_LEVEL_WARNING, "OnEnterCallback: Virtual keyboard target is null!");
        }

        Show(false, nullptr);
    }

    void NXInlineVirtualKeyboard::Destroy() {
        if (b_IsInit) {
            swkbdInlineClose(&m_InlineKeyboard);
            b_IsInit = false;
        }
    }

    void NXInlineVirtualKeyboard::Update() {
        if (b_IsInit) {
            swkbdInlineUpdate(&m_InlineKeyboard, nullptr);
        }
    }

    bool NXInlineVirtualKeyboard::IsPointOnKeyboard(const core::math::Vector2& point) {
        if (!b_IsVisible || !b_IsInit) {
            return false;
        }

        // Switch display is 720 pixels high, so we need to take that into account since
        // swkbdInlineGetMaxHeight() returns the keyboard height relative to the bottom of the screen.
        return (s32) point.y > (720 - swkbdInlineGetMaxHeight(&m_InlineKeyboard));
    }

    void NXInlineVirtualKeyboard::Show(bool show, input::IInputTarget *target) {
        if (!b_IsInit) {
            g_LoggerNXInlineVirtualKeyboard.Log(runtime::LOG_LEVEL_ERROR, "Virtual keyboard is not initialized!");
            return;
        }

        if (show) {
            if (target == nullptr) {
                g_LoggerNXInlineVirtualKeyboard.Log(runtime::LOG_LEVEL_ERROR, "Input target is null! Hiding keyboard!");
                goto LBL_HIDE_KB;
            }

            if(m_Target && m_Target == target) {
                return;
            }

            SwkbdAppearArg args;

            swkbdInlineMakeAppearArg(&args, SwkbdType_Normal);
            args.returnButtonFlag = 1;

            swkbdInlineAppear(&m_InlineKeyboard, &args);

            if(target->GetText().size() > 0) {
                // specify input text and cursor position; otherwise text will be empty
                swkbdInlineSetInputText(&m_InlineKeyboard, target->GetText().data());
                swkbdInlineSetCursorPos(&m_InlineKeyboard, target->GetText().size() - 1);
                Update();
            } else {
                // specify input text and cursor position; otherwise text will be empty
                swkbdInlineSetInputText(&m_InlineKeyboard, "");
                swkbdInlineSetCursorPos(&m_InlineKeyboard, 0);
            }

            Update();

            b_IsVisible = true;
            m_Target = target;
        } else {
LBL_HIDE_KB:
            swkbdInlineDisappear(&m_InlineKeyboard);

            b_IsVisible = false;
            m_Target = nullptr;
        }
    }
}