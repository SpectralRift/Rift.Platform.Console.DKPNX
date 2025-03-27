#pragma once

#include <map>

#include <switch.h>

#include <Engine/Input/IInputDevice.hpp>
#include <Engine/Core/Math/Vector2.hpp>

namespace engine::platform::nx {
struct NXControllerDevice : public input::IInputDevice {
        NXControllerDevice(HidNpadIdType padId);

        bool Initialize() override;

        void Destroy() override;

        void Poll() override;

        std::string GetName() const override;

//        core::runtime::input::InputDeviceType GetType() const override {
//            return core::runtime::input::InputDeviceType::INPUT_DEVICE_TYPE_GAMEPAD;
//        }

        int GetPlayerId() override;

//        bool HasAxis(core::runtime::input::InputDeviceAxis axis) const;
//        core::math::Vector2 GetAxis(core::runtime::input::InputDeviceAxis axis) const;

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