#include <memory>

#include <switch.h>

#include <Engine/Platform/NX/Input/NX_TouchDevice.hpp>
#include <Engine/Platform/NX/Input/NX_InlineVirtualKeyboard.hpp>
#include <Engine/Platform/NX/Input/NX_InputProvider.hpp>

#include <Engine/Input/InputKeyRepository.hpp>
#include <Engine/Input/InputAxisRepository.hpp>
#include <Engine/Input/InputManager.hpp>

using namespace engine;

// reference to global stuff
extern std::shared_ptr<platform::nx::NXTouchDevice> g_NXTouchDevice;
extern std::shared_ptr<platform::nx::NXInputProvider> g_NXInputProvider;
extern std::shared_ptr<platform::nx::NXInlineVirtualKeyboard> g_NXVirtualKeyboard;

// reference to EngineMain
extern int EngineMain(int argc, char *argv[]);

void EnginePreShutdown() {
    if(g_NXInputProvider) {
        g_NXInputProvider->Shutdown();
        g_NXInputProvider = nullptr;
    }

    if(g_NXTouchDevice) {
        input::InputManager::Instance()->UnregisterDevice(g_NXTouchDevice.get());
        g_NXTouchDevice = nullptr;
    }

    if(g_NXVirtualKeyboard) {
        g_NXVirtualKeyboard->Destroy();
        g_NXVirtualKeyboard = nullptr;
    }
}

int main(int argc, char *argv[]) {
    // mount ROMFS
    romfsInit();

    socketInitializeDefault();
    nxlinkStdio(); // stdout/stderr > NETWORK

    // initialize touch
    hidInitializeTouchScreen();

    // register input keys
    auto &repoItx = input::InputKeyRepository::Instance();
    auto &axisRepoItx = input::InputAxisRepository::Instance();

    repoItx.AddKey("NX_JoyCon_A");
    repoItx.AddKey("NX_JoyCon_B");
    repoItx.AddKey("NX_JoyCon_X");
    repoItx.AddKey("NX_JoyCon_Y");

    repoItx.AddKey("NX_JoyCon_LeftStick");
    repoItx.AddKey("NX_JoyCon_RightStick");

    repoItx.AddKey("NX_JoyCon_LeftBumper");
    repoItx.AddKey("NX_JoyCon_RightBumper");

    repoItx.AddKey("NX_JoyCon_LeftTrigger");
    repoItx.AddKey("NX_JoyCon_RightTrigger");
    repoItx.AddKey("NX_JoyCon_SideLeft");
    repoItx.AddKey("NX_JoyCon_SideRight");

    repoItx.AddKey("NX_JoyCon_Plus");
    repoItx.AddKey("NX_JoyCon_Minus");

    repoItx.AddKey("NX_JoyCon_DPad_Up");
    repoItx.AddKey("NX_JoyCon_DPad_Down");
    repoItx.AddKey("NX_JoyCon_DPad_Left");
    repoItx.AddKey("NX_JoyCon_DPad_Right");

    axisRepoItx.AddAxis("NX_JoyCon_LeftStickX");
    axisRepoItx.AddAxis("NX_JoyCon_LeftStickY");
    axisRepoItx.AddAxis("NX_JoyCon_RightStickX");
    axisRepoItx.AddAxis("NX_JoyCon_RightStickY");

    // initialize touch
    g_NXTouchDevice = std::make_shared<platform::nx::NXTouchDevice>();
    input::InputManager::Instance()->RegisterDevice(g_NXTouchDevice.get());

    // initialize input provider for joycons
    g_NXInputProvider = std::make_shared<platform::nx::NXInputProvider>();
    g_NXInputProvider->Init();

    // initialize virtual keyboard input
    g_NXVirtualKeyboard = std::make_shared<platform::nx::NXInlineVirtualKeyboard>();
    g_NXVirtualKeyboard->Initialize();

    int ret = EngineMain(argc, argv);

    romfsExit();
    socketExit();

    return ret;
}