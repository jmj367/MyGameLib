#include "OpenGLRendererBackend.h"
#include <GL/glew.h>

OpenGLRendererBackend::OpenGLRendererBackend()
{
}

OpenGLRendererBackend::~OpenGLRendererBackend()
{
}

bool OpenGLRendererBackend::PrepareWindow()
{
    // OpenGLの属性の設定
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,	8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,	8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,	8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,	8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,	24);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    
    return true;
}

bool OpenGLRendererBackend::Initialize(void *windowHandle, float screenWidth, float screenHeight)
{
    SDL_Window *window = static_cast<SDL_Window*>(windowHandle);

    // コンテキスト取得
    mGLContext = SDL_GL_CreateContext(window);

    // GLEWの初期化
    glewExperimental = GL_TRUE;
    if(!glewInit() == GLEW_OK)
    {
        return false;
    }

    // いくつかのプラットフォームではGLEWの初期化後にOpenGLのエラーが発生することがある
    // これをクリアしておく
    glGetError();

    return true;
}

void OpenGLRendererBackend::Shutdown()
{
    // OpenGLのシャットダウン処理をここに記述
    // 例えば、リソースの解放など
}