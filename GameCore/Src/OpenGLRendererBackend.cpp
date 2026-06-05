#include "OpenGLRendererBackend.h"
#include "LoaderModel.h"
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

    mScreenWidth = screenWidth;
    mScreenHeight = screenHeight;

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
    // NOTE: MeshのロードにRendererが要る
    Mesh mesh;
    if (!LoaderModel::LoadMesh(fileName, mesh, GetRenderer()))
    {
        return false;
    }

    // キャッシュに追加
    outID = mNextResourceID++;
    mMeshes.emplace(outID, std::move(mesh));

    return true;
}

bool OpenGLRendererBackend::GetSkeleton(const std::string &fileName, ResourceID &outID)
{
    // キャッシュに存在するか
    for (const auto &pair : mSkeletons)
    {
        if (pair.second.GetFileName() == fileName)
        {
            outID = pair.first;
            return true;
        }
    }

    // キャッシュに存在しない場合は新規ロード
    Skeleton skeleton;
    if (!LoaderModel::LoadSkeleton(fileName, skeleton))
    {
        return false;
    }

    // キャッシュに追加
    outID = mNextResourceID++;
    mSkeletons.emplace(outID, std::move(skeleton));

    return true;
}

bool OpenGLRendererBackend::GetShader(const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName, ResourceID &outID)
{
    // キャッシュに存在するか
    for (const auto &pair : mShaders)
    {
        if (pair.second.GetVertexShaderFileName() == vertexShaderFileName &&
            pair.second.GetFragmentShaderFileName() == fragmentShaderFileName)
        {
            outID = pair.first;
            return true;
        }
    }

    // キャッシュに存在しない場合は新規ロード
    Shader shader;
    if (!shader.Load(vertexShaderFileName, fragmentShaderFileName))
    {
        return false;
    }

    // キャッシュに追加
    outID = mNextResourceID++;
    mShaders.emplace(outID, std::move(shader));

    return true;
}

void OpenGLRendererBackend::ReleaseTexture(ResourceID textureID)
{
    mTextures.erase(textureID);
}

void OpenGLRendererBackend::ReleaseMesh(ResourceID meshID)
{
    mMeshes.erase(meshID);
}

void OpenGLRendererBackend::ReleaseSkeleton(ResourceID skeletonID)
{
    mSkeletons.erase(skeletonID);
}

void OpenGLRendererBackend::ReleaseShader(ResourceID shaderID)
{
    mShaders.erase(shaderID);
}

void OpenGLRendererBackend::ReleaseAllResources()
{
    mTextures   .clear();
    mMeshes     .clear();
    mSkeletons  .clear();
    mShaders    .clear();
}

void OpenGLRendererBackend::DrawFrame(const FrameDrawInfo &drawInfo)
{
    // バッファのクリア
    glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer.GetBufferID());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 描画の各段階
    DrawMesh(drawInfo);
}

void OpenGLRendererBackend::DrawMesh(const FrameDrawInfo &drawInfo)
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);

    for (const auto &meshDrawInfo : drawInfo.MeshDrawInfos)
    {
        // 描画処理

        // シェーダー
        auto shaderIter = mShaders.find(meshDrawInfo.ShaderID);
        if (shaderIter == mShaders.end())
        {
            continue;
        }
        Shader &shader = shaderIter->second;
        shader.SetActive();
        shader.SetMatrixUniform("uVIewProj", drawInfo.View * drawInfo.Projection);

        // テクスチャ
        auto textureIter = mTextures.find(meshDrawInfo.TextureID);
        if (textureIter == mTextures.end())
        {
            continue;
        }
        Texture &texture = textureIter->second;
        texture.SetActive();

        // 頂点配列
        auto meshIter = mMeshes.find(meshDrawInfo.MeshID);
        if (meshIter == mMeshes.end())
        {
            continue;
        }
        Mesh &mesh = meshIter->second;
        VertexArray *vertexArray = mesh.GetVertexArray();
        vertexArray->SetActive();

        // 描画
        glDrawElements(GL_TRIANGLES, vertexArray->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
}