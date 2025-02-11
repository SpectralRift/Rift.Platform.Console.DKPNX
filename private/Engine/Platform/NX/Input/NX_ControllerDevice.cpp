#define SHOW_PRIVATE_API

#include <Engine/Platform/NX/Input/NX_ControllerDevice.hpp>
#include <Engine/Platform/NX/Input/NX_InputKeys.hpp>
#include <Engine/Core/Runtime/Input/InputManager.hpp>
#include <Engine/Core/Runtime/Input/InputEvent.hpp>

#include <cstdio>

namespace engine::platform::nx {
    NXControllerDevice::NXControllerDevice(HidNpadIdType padId) : b_IsConnected(false), m_PadId(padId) {}

    bool NXControllerDevice::Initialize() {
        printf("NXControllerDevice: Initializing controller device for player %i...\n", m_PadId + 1);

        // if this represents Player 1, we configure it so that it allows Handheld mode as well
        if (m_PadId == HidNpadIdType_No1) {
            padInitializeDefault(&m_LastState);
        } else {
            padInitialize(&m_LastState, m_PadId);
        }

        // fetch pad state
        padUpdate(&m_LastState);

        // fetch pad style
        m_PadDeviceType = hidGetNpadDeviceType(m_PadId);
        m_PadStyle = (HidNpadStyleTag) hidGetNpadStyleSet(m_PadId);
        b_IsHandheld = padIsHandheld(&m_LastState);

        m_LeftStick = {};
        m_RightStick = {};

        b_IsConnected = padIsConnected(&m_LastState);
        if(!b_IsConnected) {
            printf("NXControllerDevice: Player %i is not connected!\n", m_PadId + 1);
            return false;
        }

        return b_IsConnected;
    }

    void NXControllerDevice::Destroy() {
        b_IsConnected = false;
    }

    void NXControllerDevice::Poll() {
        if (!b_IsConnected) {
            return;
        }

        // update pad state
        padUpdate(&m_LastState);

        b_IsConnected = padIsConnected(&m_LastState);

        // if the pad isn't connected, we return out.
        if (!b_IsConnected) {
            return;
        }

        auto currentPadType = hidGetNpadDeviceType(m_PadId);
        auto currentPadStyle = (HidNpadStyleTag) hidGetNpadStyleSet(m_PadId);

        // if the pad style becomes mismatched, we "disconnect" to force controller reconfiguration in the provider!
        if (currentPadType != m_PadDeviceType || currentPadStyle != m_PadStyle) {
            printf("NXControllerDevice: Controller type/style mismatch!\n");
            b_IsConnected = false;
            return;
        }

        if(m_PadStyle & HidNpadStyleSet_NpadFullCtrl || b_IsHandheld) {
            auto stickPosL = padGetStickPos(&m_LastState, 0);
            auto stickPosR = padGetStickPos(&m_LastState, 1);

            m_LeftStick.x = (float) stickPosL.x / JOYSTICK_MAX;
            m_LeftStick.y = (float) stickPosL.y / JOYSTICK_MAX;
            m_RightStick.x = (float) stickPosR.x / JOYSTICK_MAX;
            m_RightStick.y = (float) stickPosR.y / JOYSTICK_MAX;
        } else {
            auto stickPos = padGetStickPos(&m_LastState, m_PadDeviceType & HidDeviceTypeBits_JoyRight ? 1 : 0);

            m_LeftStick.x = (float) stickPos.x / JOYSTICK_MAX;
            m_LeftStick.y = (float) stickPos.y / JOYSTICK_MAX;
        }

        // check for axis updates
        if(m_LastLeftStick != m_LeftStick) {
            m_LastLeftStick = m_LeftStick;

            // report new axis value for left stick
            core::runtime::input::InputManager::Instance()->PushEvent(
                    new core::runtime::input::GamepadAxisUpdateEvent(
                            this,
                            core::runtime::input::InputDeviceAxis::INPUT_DEVICE_AXIS_GAMEPAD_STICK_LEFT,
                            m_LeftStick
                    )
            );
        }

        if(m_LastRightStick != m_RightStick) {
            m_LastRightStick = m_RightStick;

            // report new axis value for right stick
            core::runtime::input::InputManager::Instance()->PushEvent(
                    new core::runtime::input::GamepadAxisUpdateEvent(
                            this,
                            core::runtime::input::InputDeviceAxis::INPUT_DEVICE_AXIS_GAMEPAD_STICK_RIGHT,
                            m_RightStick
                    )
            );
        }

        auto currentBtnState = padGetButtons(&m_LastState);

        if(currentBtnState != m_LastBtnState) {
#define BTN_CHECK(nx_btn, nx_btn_type) \
            if (currentBtnState & nx_btn) { \
                if(!(m_LastBtnState & nx_btn)) { \
                    core::runtime::input::InputManager::Instance()->PushEvent( \
                            new core::runtime::input::GamepadButtonDownEvent( \
                                    this, nx_btn_type \
                            ) \
                    ); \
                } \
            } else { \
                if(m_LastBtnState & nx_btn) { \
                    core::runtime::input::InputManager::Instance()->PushEvent( \
                            new core::runtime::input::GamepadButtonUpEvent( \
                                    this, nx_btn_type \
                            ) \
                    ); \
                } \
            }

            BTN_CHECK(HidNpadButton_A, NXInputKeys::NX_JoyCon_A);
            BTN_CHECK(HidNpadButton_B, NXInputKeys::NX_JoyCon_B);
            BTN_CHECK(HidNpadButton_X, NXInputKeys::NX_JoyCon_X);
            BTN_CHECK(HidNpadButton_Y, NXInputKeys::NX_JoyCon_Y);
            BTN_CHECK(HidNpadButton_L, NXInputKeys::NX_JoyCon_LeftBumper);
            BTN_CHECK(HidNpadButton_R, NXInputKeys::NX_JoyCon_RightBumper);
            BTN_CHECK(HidNpadButton_ZL, NXInputKeys::NX_JoyCon_LeftTrigger);
            BTN_CHECK(HidNpadButton_ZR, NXInputKeys::NX_JoyCon_RightTrigger);
            BTN_CHECK(HidNpadButton_AnySL, NXInputKeys::NX_JoyCon_SideLeft);
            BTN_CHECK(HidNpadButton_AnySR, NXInputKeys::NX_JoyCon_SideRight);
            BTN_CHECK(HidNpadButton_StickL, NXInputKeys::NX_JoyCon_LeftStick);
            BTN_CHECK(HidNpadButton_StickR, NXInputKeys::NX_JoyCon_RightStick);
            BTN_CHECK(HidNpadButton_Up, NXInputKeys::NX_JoyCon_DPad_Up);
            BTN_CHECK(HidNpadButton_Down, NXInputKeys::NX_JoyCon_DPad_Down);
            BTN_CHECK(HidNpadButton_Left, NXInputKeys::NX_JoyCon_DPad_Left);
            BTN_CHECK(HidNpadButton_Right, NXInputKeys::NX_JoyCon_DPad_Right);
            BTN_CHECK(HidNpadButton_Plus, NXInputKeys::NX_JoyCon_Plus);
            BTN_CHECK(HidNpadButton_Minus, NXInputKeys::NX_JoyCon_Minus);

#undef BTN_CHECK

            m_LastBtnState = currentBtnState;
        }
    }

    int NXControllerDevice::GetPlayerId() {
        return m_PadId;
    }

    bool NXControllerDevice::HasAxis(core::runtime::input::InputDeviceAxis axis) const {
        return true;
    }

    core::math::Vector2 NXControllerDevice::GetAxis(core::runtime::input::InputDeviceAxis axis) const {
        switch(axis) {
            case core::runtime::input::InputDeviceAxis::INPUT_DEVICE_AXIS_GAMEPAD_STICK_LEFT:
                return m_LeftStick;
            case core::runtime::input::InputDeviceAxis::INPUT_DEVICE_AXIS_GAMEPAD_STICK_RIGHT:
                return m_RightStick;
            default:
                printf("NXControllerDevice: GetAxis - Invalid axis %i!\n", axis);
                return {};
        }
    }

    std::string NXControllerDevice::GetName() const {
        auto playerIdText = "P" + std::to_string(m_PadId + 1);

        if (b_IsHandheld) {
            return "Nintendo Switch Handheld " + playerIdText;
        }

        if (m_PadStyle & HidNpadStyleTag_NpadJoyDual) {
            return "Nintendo Switch JoyCon (Dual) " + playerIdText;
        } else if (m_PadStyle & HidNpadStyleTag_NpadHandheld) {
            if (m_PadDeviceType & HidDeviceTypeBits_HandheldLeft) {
                return "Nintendo Switch Handheld Left JoyCon " + playerIdText;
            } else if (m_PadDeviceType & HidDeviceTypeBits_HandheldRight) {
                return "Nintendo Switch Handheld Right JoyCon " + playerIdText;
            }
        }

        if (m_PadDeviceType & HidDeviceTypeBits_JoyLeft) {
            return "Nintendo Switch Left JoyCon " + playerIdText;
        } else if (m_PadDeviceType & HidDeviceTypeBits_JoyRight) {
            return "Nintendo Switch Right JoyCon " + playerIdText;
        }

        return "Nintendo Switch Controller " + playerIdText;
    }
}