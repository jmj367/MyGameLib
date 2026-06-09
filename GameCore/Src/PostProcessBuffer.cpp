#include "PostProcessBuffer.h"
#include "Texture.h"
#include <GL/glew.h>

PostProcessBuffer::PostProcessBuffer()
    : mCurrentBufferIndex(0)
{
    mBuffers[0].FBO = 0;
    mBuffers[1].FBO = 0;
}

PostProcessBuffer::~PostProcessBuffer()
{
    Destroy();
}

bool PostProcessBuffer::Create(float width, float height)
{
    // ポストプロセス用の二つのカラーテクスチャとFBOを作成
    for(int i = 0; i < 2; ++i)
    {
        // FBOの作成
        glGenFramebuffers(1, &mBuffers[i].FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, mBuffers[i].FBO);

        // カラーテクスチャの作成
        Texture tex;
        tex.CreateForRendering(static_cast<int>(width), static_cast<int>(height), GL_RGBA);
        mBuffers[i].Texture = tex;

        // FBOにテクスチャをアタッチ
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mBuffers[i].Texture.GetTextureID(), 0);

        // FBOの完成チェック
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            Destroy();
            return false;
        }
    }

    return true;
}

void PostProcessBuffer::Destroy()
{
    for(int i = 0; i < 2; ++i)
    {
        glDeleteFramebuffers(1, &mBuffers[i].FBO);
        mBuffers[i].Texture.Unload();
    }
}

void PostProcessBuffer::Swap()
{
    mCurrentBufferIndex = 1 - mCurrentBufferIndex;
}

unsigned int PostProcessBuffer::GetReadBufferID() const
{
    return mBuffers[mCurrentBufferIndex].FBO;
}

unsigned int PostProcessBuffer::GetWriteBufferID() const
{
    return mBuffers[1 - mCurrentBufferIndex].FBO;
}