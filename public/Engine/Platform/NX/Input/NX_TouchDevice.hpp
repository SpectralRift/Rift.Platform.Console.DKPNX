#pragma once

#include <map>

#include <Engine/Input/IInputDevice.hpp>
#include <Engine/Core/Math/Vector2.hpp>

namespace engine::platform::nx {
    struct NXTouchDevice : public input::IInputDevice {
        NXTouchDevice() {}

        bool Initialize() override;

        void Destroy() override;

        void Poll() override;

//        bool HasTouchPoint(int point) override;
//
//        core::math::Vector2 GetTouchPoint(int point) override;

        std::string GetName() const override {
            return "Nintendo Switch Touchscreen";
        }

//        core::runtime::input::InputDeviceType GetType() const override {
//            return core::runtime::input::InputDeviceType::INPUT_DEVICE_TYPE_TOUCH;
//        }

//        int GetMaxTouchPoints() override {
//            return 16;
//        }

        int GetPlayerId() override {
            return 0;
        }
    protected:
        std::map<unsigned int, core::math::Vector2> m_FingerPos;
    };
}