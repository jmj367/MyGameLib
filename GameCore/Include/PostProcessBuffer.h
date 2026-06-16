#pragma once

#include "Define.h"
#include "Texture.h"

class PostProcessBuffer
{
public:
    PostProcessBuffer();
    ~PostProcessBuffer();

    // バッファの作成と破棄
    bool Create(float width, float height);
    void Destroy();

    // バッファのスワップ
    void Swap();

    // バッファのIDの取得
    unsigned int GetReadBufferID() const;
    unsigned int GetWriteBufferID() const;

private:
    struct Buffer
    {
        unsigned int FBO;   // フレームバッファオブジェクト
        Texture Texture; // テクスチャ
    };

    Buffer mBuffers[2];      // ダブルバッファリング用のバッファ
    int mCurrentBufferIndex; // 現在のバッファのインデックス
};