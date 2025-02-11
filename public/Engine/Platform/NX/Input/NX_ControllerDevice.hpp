#pragma once

#include <map>

#include <switch.h>

#include <Engine/Core/Runtime/Input/IInputDevice.hpp>
#include <Engine/Core/Math/Vector2.hpp>

namespace engine::platform::nx {
    struct NXControllerDevice : public core::runtime::input::IAxisDevice {
        NXControllerDevice(HidNpadIdType padId);

        bool Initialize() override;

        void Destroy() override;

        void Poll() override;

        std::string GetName() const override;

        core::runtime::input::InputDeviceType GetType() const override {
            return core::runtime::input::InputDeviceType::INPUT_DEVICE_TYPE_GAMEPAD;
        }

        int GetPlayerId() override;

        bool HasAxis(core::runtime::input::InputDeviceAxis axis) const override;
        core::math::Vector2 GetAxis(core::runtime::input::InputDeviceAxis axis) const override;

        bool IsConnected() const { return b_IsConnected; }
    protected:
        bool b_IsConnected;
        bool b_IsHandheld;

        PadState m_LastState;
        unsigned int m_LastBtnState;
        unsigned int m_PadDeviceType;

        HidNpadIdType m_PadId{};
        HidNpadStyleTag m_PadStyle{};

        core::math::Vector2 m_LeftStick;
        core::math::Vector2 m_RightStick;

        core::math::Vector2 m_LastLeftStick;
        core::math::Vector2 m_LastRightStick;
    };
}