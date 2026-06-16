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

    // ポストプロセス用バッファの初期化
    if (!mPostProcessBuffer.Create(width, height))
    {
        return false;
    }

    // 球メッシュのロード
    if (!GetMesh("" /* TODO: メッシュファイル名 */, mSphereMesh))
    {
        return false;
    }

    // ライト描画用シェーダーのロード
    if (!GetShader("", "" /* TODO: シェーダーファイル名 */, mLightShader))
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
    // 描画の各段階
    DrawMesh(drawInfo.MeshDrawInfos, drawInfo.View, drawInfo.Projection);
    DrawLighting(drawInfo);
    DrawPostProcess(drawInfo);
}

void OpenGLRendererBackend::DrawMesh(const std::vector<Renderer::MeshDrawInfo> &drawInfo, const Matrix4 &view, const Matrix4 &proj)
{
    glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer.GetBufferID());

    // バッファのクリア
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);

    for (const auto &meshDrawInfo : drawInfo)
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
        shader.SetMatrixUniform("uVIewProj", view * proj);

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

void OpenGLRendererBackend::DrawLighting(const FrameDrawInfo &drawInfo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, mPostProcessBuffer.GetWriteBufferID());

    mGBuffer.SetTexturesActive();

    // バッファのクリア
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glDisable(GL_DEPTH_TEST);

    // シェーダー
    auto shaderIter = mShaders.find(mLightShader);
    if (shaderIter == mShaders.end())
    {
        return;
    }
    Shader &shader = shaderIter->second;
    shader.SetActive();

    // カメラ位置をシェーダーに渡す
    Matrix4 invView = drawInfo.View;
    invView.Invert();
    shader.SetVector3Uniform("uCameraPos", invView.GetTranslation());

    // 環境光
    // HACK: AmbientLightDrawInfosは複数の環境光を想定しているが、現状は1つしか描画しない
    if (!drawInfo.AmbientLightDrawInfos.empty())
    {
        const auto &ambientLight = drawInfo.AmbientLightDrawInfos[0];
        shader.SetVector3Uniform("uAmbientLight", ambientLight.Color);
    }

    // 平行光源
    // HACK: DirectionalLightDrawInfosは複数の平行光源を想定しているが、現状は1つしか描画しない
    if (!drawInfo.DirectionalLightDrawInfos.empty())
    {
        const auto &dirLight = drawInfo.DirectionalLightDrawInfos[0];
        shader.SetVector3Uniform("uDirLight.Direction", dirLight.Direction);
        shader.SetVector3Uniform("uDirLight.DiffuseColor", dirLight.DiffuseColor);
        shader.SetVector3Uniform("uDirLight.SpecularColor", dirLight.SpecularColor);
    }

    // 描画
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // 深度バッファをコピー
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBuffer.GetBufferID());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mPostProcessBuffer.GetWriteBufferID());
    int width = static_cast<int>(mScreenWidth);
    int height = static_cast<int>(mScreenHeight);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    // 点光源の描画
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_GEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // ライトのメッシュ内にカメラが入ったときに正しく描画されるように表面をカリングする
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    // シェーダー
    shaderIter = mShaders.find(mLightShader);
    if (shaderIter == mShaders.end())
    {
        return;
    }
    shader = shaderIter->second;
    shader.SetActive();
    shader.SetMatrixUniform("uViewProj", drawInfo.View * drawInfo.Projection);

    // 頂点配列
    auto meshIter = mMeshes.find(mSphereMesh);
    if (meshIter == mMeshes.end())
    {
        return;
    }
    Mesh &mesh = meshIter->second;
    VertexArray *vertexArray = mesh.GetVertexArray();
    vertexArray->SetActive();

    mGBuffer.SetTexturesActive();

    for (const auto &pointLightDrawInfo : drawInfo.PointLightDrawInfos)
    {
        // 点光源の位置と半径をシェーダーに渡す
        shader.SetMatrixUniform("uPointLight.WorldTransform", pointLightDrawInfo.WorldTransform);
        shader.SetVector3Uniform("uPointLight.Position", pointLightDrawInfo.Position);
        shader.SetFloatUniform("uPointLight.Radius", pointLightDrawInfo.Radius);
        shader.SetVector3Uniform("uPointLight.Color", pointLightDrawInfo.Color);
        shader.SetFloatUniform("uPointLight.Intensity", pointLightDrawInfo.Intensity);

        // 描画
        glDrawElements(GL_TRIANGLES, vertexArray->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }

    mPostProcessBuffer.Swap();

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void OpenGLRendererBackend::DrawPostProcess(const FrameDrawInfo &drawInfo)
{
    glDisable(GL_DEPTH_TEST);

    for (const auto &postProcessInfo : drawInfo.PostProcessDrawInfos)
    {
        // ポストプロセスの描画処理

        // 出力先を書き込みバッファに指定
        glBindFramebuffer(GL_FRAMEBUFFER, mPostProcessBuffer.GetWriteBufferID());

        // 前の段階の結果を読み込みテクスチャとしてバインド
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mPostProcessBuffer.GetReadBufferID());

        // シェーダー
        auto shaderIter = mShaders.find(postProcessInfo.ShaderID);
        if (shaderIter == mShaders.end())
        {
            continue;
        }

        Shader &shader = shaderIter->second;
        shader.SetActive();

        // 描画
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // バッファのスワップ
        mPostProcessBuffer.Swap();
    }
}