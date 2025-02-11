#pragma once

#include <Engine/Core/Runtime/Input/InputKey.hpp>

namespace engine::platform::nx {
    struct NXInputKeys {
        static const core::runtime::input::InputKey NX_JoyCon_A;
        static const core::runtime::input::InputKey NX_JoyCon_B;
        static const core::runtime::input::InputKey NX_JoyCon_X;
        static const core::runtime::input::InputKey NX_JoyCon_Y;

        static const core::runtime::input::InputKey NX_JoyCon_LeftStick;
        static const core::runtime::input::InputKey NX_JoyCon_RightStick;

        static const core::runtime::input::InputKey NX_JoyCon_LeftBumper;
        static const core::runtime::input::InputKey NX_JoyCon_RightBumper;

        static const core::runtime::input::InputKey NX_JoyCon_LeftTrigger;
        static const core::runtime::input::InputKey NX_JoyCon_RightTrigger;

        static const core::runtime::input::InputKey NX_JoyCon_SideLeft;
        static const core::runtime::input::InputKey NX_JoyCon_SideRight;

        static const core::runtime::input::InputKey NX_JoyCon_Plus;
        static const core::runtime::input::InputKey NX_JoyCon_Minus;

        static const core::runtime::input::InputKey NX_JoyCon_DPad_Up;
        static const core::runtime::input::InputKey NX_JoyCon_DPad_Down;
        static const core::runtime::input::InputKey NX_JoyCon_DPad_Left;
        static const core::runtime::input::InputKey NX_JoyCon_DPad_Right;
    };
}