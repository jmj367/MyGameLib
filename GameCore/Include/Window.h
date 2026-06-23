#pragma once

#include "Define.h"
#include "WindowEvent.h"
#include <string>

class Window
{
public:
    Window()
    {
    }
    virtual ~Window() = default;

    // 初期化
    virtual bool Initialize() = 0;

    // ウィンドウの作成
    virtual bool Create(
        const std::string &title,
        int posX,
        int posY,
        int width,
        int height,
        GraphicsAPI api = GraphicsAPI::None) = 0;
    // ウィンドウの破棄
    virtual void Destroy() = 0;

    // ウィンドウハンドラの取得
    virtual void *GetWindowHandle() const = 0;

    // ウィンドウのイベントを取得
    virtual bool PollWindowEvents(WindowEvent &events) const = 0;

    // 画面位置の取得
    virtual int GetScreenPosX() const = 0;
    virtual int GetScreenPosY() const = 0;

    // 画面サイズの取得
    virtual int GetScreenWidth() const = 0;
    virtual int GetScreenHeight() const = 0;
};