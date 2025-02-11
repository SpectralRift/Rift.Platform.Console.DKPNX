#define SHOW_PRIVATE_API

#include <switch.h>

#include <Engine/Platform/NX/Input/NX_TouchDevice.hpp>
#include <Engine/Core/Runtime/Input/InputManager.hpp>
#include <Engine/Core/Runtime/Input/InputEvent.hpp>

namespace engine::platform::nx {
    bool NXTouchDevice::Initialize() {
        return true;
    }

    void NXTouchDevice::Destroy() {}

    void NXTouchDevice::Poll() {
        HidTouchScreenState state{};
        unsigned int width = 0, height = 0;

        // get current screen size
        nwindowGetDimensions(nwindowGetDefault(), &width, &height);

        if (hidGetTouchScreenStates(&state, 1)) {
            for (int i = 0; i < state.count; i++) {
                // logic taken from https://github.com/devkitPro/SDL/blob/switch-sdl-2.28/src/video/switch/SDL_switchtouch.c#L50
                // NX's touchscreen coordinates are provided absolutely to the screen size, which is 1280x720 by default.
                // we need to provide relative coordinates from 0 to 1 to the engine in order to handle input accordingly
                core::math::Vector2 pos{((float) state.touches[i].x / 1280.0f) * width, ((float) state.touches[i].y / 720.0f) * height};

                if (!m_FingerPos.contains(state.touches[i].finger_id)) {
                    m_FingerPos.insert({state.touches[i].finger_id, pos});

                    // position is new in the list, so we consider this a touch start event
                    core::runtime::input::InputManager::Instance()->PushEvent(
                            new core::runtime::input::TouchStartEvent(
                                    this,
                                    (int) state.touches[i].finger_id,
                                    pos
                            )
                    );
                } else {
                    if (m_FingerPos.at(state.touches[i].finger_id) != pos) {
                        m_FingerPos.at(state.touches[i].finger_id).x = pos.x;
                        m_FingerPos.at(state.touches[i].finger_id).y = pos.y;

                        // the position changed, so we signal it (touch drag event)
                        core::runtime::input::InputManager::Instance()->PushEvent(
                                new core::runtime::input::TouchDragEvent(
                                        this,
                                        (int) state.touches[i].finger_id,
                                        pos
                                )
                        );
                    }
                }
            }

            // check what fingers were lifted so that we could push the touch end event
            auto it = m_FingerPos.begin();

LBL_FINGER_CHECK:
            while (it != m_FingerPos.end()) {
                // check if we still get state updates for the current finger
                for (int i = 0; i < state.count; i++) {
                    if (it->first == state.touches[i].finger_id) {
                        it++;
                        goto LBL_FINGER_CHECK;
                    }
                }

                // if the finger does not exist anymore, we push the touch end event
                core::runtime::input::InputManager::Instance()->PushEvent(
                        new core::runtime::input::TouchEndEvent(
                                this,
                                (int) it->first,
                                it->second
                        )
                );

                // we remove the finger from the list and we continue iterating
                it = m_FingerPos.erase(it);
            }
        }
    }

    bool NXTouchDevice::HasTouchPoint(int point) {
        return m_FingerPos.contains(point);
    }

    core::math::Vector2 NXTouchDevice::GetTouchPoint(int point) {
        if (point > GetMaxTouchPoints() ||!m_FingerPos.contains(point)) {
            return {0, 0};
        }

        return m_FingerPos.at(point);
    }
}