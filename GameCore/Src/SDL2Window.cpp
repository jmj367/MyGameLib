#include "SDL2Window.h"
#include <SDL2/SDL.h>

SDL2Window::SDL2Window()
    : mWindow(nullptr)
{
}

SDL2Window::~SDL2Window()
{
    Destroy();
}

bool SDL2Window::Initialize()
{
    // SDLの初期化
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        return false;
    }

    // TODO: SDL_TTFの初期化

    return true;
}

bool SDL2Window::Create(
    const std::string &title,
    int posX,
    int posY,
    int width,
    int height,
    GraphicsAPI api)
{
    // ウィンドウの作成
    switch (api)
    {
    case GraphicsAPI::OpenGL:
        mWindow = SDL_CreateWindow(title.c_str(), posX, posY, width, height, SDL_WINDOW_OPENGL);
        break;
    }

    // 画面位置とサイズを設定
    mScreenPosX = posX;
    mScreenPosY = posY;
    mScreenWidth = width;
    mScreenHeight = height;

    return true;
}

void SDL2Window::Destroy()
{
    if (mWindow)
    {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }
}

void *SDL2Window::GetWindowHandle() const
{
    return static_cast<void *>(mWindow);
}

bool SDL2Window::PollWindowEvents(WindowEvent &event) const
{
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent))
    {
        switch (sdlEvent.type)
        {
        case SDL_QUIT:
            event.EventType = WindowEventType::Quit;
            break;
        case SDL_MOUSEWHEEL:
            event.EventType = WindowEventType::MouseWheel;
            event.MouseWheel.WheelX = sdlEvent.wheel.x;
            event.MouseWheel.WheelY = sdlEvent.wheel.y;
            break;
        case SDL_CONTROLLERDEVICEADDED:
            event.EventType = WindowEventType::ControllerAdded;
            event.Controller.ControllerID = sdlEvent.cdevice.which;
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
            event.EventType = WindowEventType::ControllerRemoved;
            event.Controller.ControllerID = sdlEvent.cdevice.which;
            break;
        default:
            event.EventType = WindowEventType::None;
            return false;
        }
    }
    
    return true;
}