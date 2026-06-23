#pragma once

#include "Window.h"

class SDL2Window : public Window
{
public:
    SDL2Window();
    ~SDL2Window();

    // オーバーライド

    bool Initialize() override;

    bool Create(
        const std::string &title,
        int posX,
        int posY,
        int width,
        int height,
        GraphicsAPI api = GraphicsAPI::None) override;
    void Destroy() override;

    void *GetWindowHandle() const override;

    bool PollWindowEvents(WindowEvent &events) const override;

    int GetScreenPosX() const override { return mScreenPosX; }
    int GetScreenPosY() const override { return mScreenPosY; }
    int GetScreenWidth() const override { return mScreenWidth; }
    int GetScreenHeight() const override { return mScreenHeight; }

private:
    // SDLのウィンドウオブジェクト
    class SDL_Window *mWindow;

    // 画面位置とサイズ
    int mScreenPosX;
    int mScreenPosY;
    int mScreenWidth;
    int mScreenHeight;
};