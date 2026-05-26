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

    // Gバッファの初期化
    int width = static_cast<int>(screenWidth);
    int height = static_cast<int>(screenHeight);
    if(!mGBuffer.Create(width, height))
    {
        return false;
    }

    return true;
}

void OpenGLRendererBackend::Shutdown()
{
    // Gバッファの破棄
    mGBuffer.Destroy();

    // コンテキストの破棄
    SDL_GL_DeleteContext(mGLContext);
}

bool OpenGLRendererBackend::GetTexture(const std::string &fileName, ResourceID &outID)
{
    // キャッシュに存在するか
    for (const auto &pair : mTextures)
    {
        if (pair.second.GetFileName() == fileName)
        {
            outID = pair.first;
            return true;
        }
    }

    // キャッシュに存在しない場合は新規ロード
    Texture texture;
    if (!texture.Load(fileName))
    {
        return false;
    }

    // キャッシュに追加
    outID = mNextResourceID++;
    mTextures.emplace(outID, std::move(texture));

    return true;
}

bool OpenGLRendererBackend::GetMesh(const std::string &fileName, ResourceID &outID)
{
    // キャッシュに存在するか
    for (const auto &pair : mMeshes)
    {
        if (pair.second.GetFileName() == fileName)
        {
            outID = pair.first;
            return true;
        }
    }

    // キャッシュに存在しない場合は新規ロード
    Mesh mesh;
    if (!mesh.Load(fileName, GetRenderer()))
    {
        return false;
    }

    // キャッシュに追加
    outID = mNextResourceID++;
    mMeshes.emplace(outID, std::move(mesh));

    return true;
}