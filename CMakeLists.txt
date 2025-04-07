cmake_minimum_required(VERSION 3.14)
project(Rift_Platform_Console_DKPNX VERSION 1.1.1)

find_package(PkgConfig REQUIRED)

add_library(
        Rift_Platform_Console_DKPNX
        STATIC
        private/Engine/Platform/NX/NX_Platform.cpp
        private/Engine/Platform/NX/NX_Window.cpp
        private/Engine/Platform/NX/NX_Thread.cpp
        private/Engine/Platform/NX/NX_Mutex.cpp
        private/Engine/Platform/NX/NX_EngineMain.cpp
        private/Engine/Platform/NX/Input/NX_TouchDevice.cpp
        private/Engine/Platform/NX/Input/NX_InlineVirtualKeyboard.cpp
        private/Engine/Platform/NX/Input/NX_InputProvider.cpp
        private/Engine/Platform/NX/Input/NX_ControllerDevice.cpp
)

target_include_directories(
        Rift_Platform_Console_DKPNX
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private"
)

pkg_search_module(DRM_NOUVEAU REQUIRED libdrm_nouveau)
pkg_search_module(GL_API REQUIRED glapi)

# let compiler know about the include directories for OpenGL and nouveau
include_directories(${GL_API_INCLUDE_DIRS} ${DRM_NOUVEAU_INCLUDE_DIRS})
link_directories(${GL_API_LIBRARY_DIRS} ${DRM_NOUVEAU_LIBRARY_DIRS})

rift_resolve_module_libs("Rift.Core.Runtime;Rift.Backend.OpenGL;Rift.Input;Rift.Runtime.Logging" RIFT_NX_DEPS)

target_link_libraries(Rift_Platform_Console_DKPNX ${RIFT_NX_DEPS} ${GL_API_LIBRARIES} ${DRM_NOUVEAU_LIBRARIES} nx)

set(GL_BACKEND_USE_LOADER OFF CACHE BOOL "NX Platform Requirement: Don't use GLAD to load OpenGL, perform static link instead" FORCE)
set(GL_BACKEND_USE_EGL_LOADER OFF CACHE BOOL "NX Platform Requirement: Don't use GLAD to load EGL, perform static link instead" FORCE)
set(GL_BACKEND_USE_EGL ON CACHE BOOL "NX Platform Requirement: Include EGL Support" FORCE)
set(GL_BACKEND_USE_ES ON CACHE BOOL "NX Platform Requirement: Enable OpenGL ES support" FORCE)
set(GL_BACKEND_FORCE_GL ON CACHE BOOL "NX Platform Requirement: Force OpenGL API" FORCE)

# platform-specific config for app package creation

nx_generate_nacp(${CMAKE_PROJECT_NAME}
        NAME "${CMAKE_PROJECT_NAME}"
        VERSION "${CMAKE_PROJECT_VERSION}"
        OUTPUT ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-AppCtlProp.nacp
)

nx_create_nro(${CMAKE_PROJECT_NAME}
        OUTPUT ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.nro
        NACP ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-AppCtlProp.nacp
        ROMFS ${CMAKE_BINARY_DIR}/DataRaw
)