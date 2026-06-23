#include "Renderer.h"
#include "Game.h"
#include "OpenGLRendererBackend.h"
#include "Shader.h"
#include "Texture.h"

Renderer::Renderer()
    : mScreenWidth(0.0f)
    , mScreenHeight(0.0f)
{
}

Renderer::~Renderer()
{
}

bool Renderer::PrepareWindow()
{
    // バックエンドのウィンドウ準備
    return mBackend->PrepareWindow();
}

bool Renderer::Initialize(void* windowHandle, float screenWidth, float screenHeight, GraphicsAPI apiType)
{
    mScreenWidth = screenWidth;
    mScreenHeight = screenHeight;

    // バックエンドの初期化
    switch (apiType)
    {
    case GraphicsAPI::OpenGL:
        mBackend = std::make_unique<OpenGLRendererBackend>(this);
        break;
    default:
        return false;
    }

    mBackend->Initialize(windowHandle, screenWidth, screenHeight);

    return true;
}

void Renderer::Shutdown()
{
    mBackend->Shutdown();
}

void Renderer::Draw(void *windowHandle)
{
    // 描画コマンドのリストをバックエンドに渡して描画
    RendererBackend::FrameDrawInfo drawInfo
    {
        windowHandle,
        mSceneDrawList,
    };
    mBackend->DrawFrame(drawInfo);

    // 描画コマンドのリストをクリア
    {
        std::lock_guard<std::mutex> lock(mFrameDrawListMutex);
        mSceneDrawList.clear();
    }
}

void Renderer::DrawSprite(const SpriteDrawInfo &spriteInfo)
{
    std::lock_guard<std::mutex> lock(mSpriteDrawListMutex);
    // DrawOrderの小さい順に描画するため、DrawOrderを考慮して挿入する
    auto iter = mSpriteDrawList.begin();
    while (iter != mSpriteDrawList.end() && iter->DrawOrder <= spriteInfo.DrawOrder)
    {
        ++iter;
    }
    mSpriteDrawList.insert(iter, spriteInfo);
}

void Renderer::DrawMesh(const MeshDrawInfo &meshInfo)
{
    std::lock_guard<std::mutex> lock(mMeshDrawListMutex);
    mMeshDrawList.push_back(meshInfo);
}

void Renderer::DrawPointLight(const PointLightDrawInfo &pointLight)
{
    std::lock_guard<std::mutex> lock(mPointLightDrawListMutex);
    mPointLightDrawList.push_back(pointLight);
}

void Renderer::DrawSpotLight(const SpotLightDrawInfo &spotLight)
{
    std::lock_guard<std::mutex> lock(mSpotLightDrawListMutex);
    mSpotLightDrawList.push_back(spotLight);
}

void Renderer::DrawDirectionalLight(const DirectionalLightDrawInfo &directionalLight)
{
    std::lock_guard<std::mutex> lock(mDirectionalLightDrawListMutex);
    mDirectionalLightDrawList.push_back(directionalLight);
}

void Renderer::DrawAmbientLight(const AmbientLightDrawInfo &ambientLight)
{
    std::lock_guard<std::mutex> lock(mAmbientLightDrawListMutex);
    mAmbientLightDrawList.push_back(ambientLight);
}

void Renderer::DrawPostProcess(const PostProcessDrawInfo &postProcessInfo)
{
    std::lock_guard<std::mutex> lock(mPostProcessDrawListMutex);
    mPostProcessDrawList.push_back(postProcessInfo);
}

void Renderer::DrawScene(const Matrix4 &view, const Matrix4 &proj)
{
    std::lock_guard<std::mutex> lock(mFrameDrawListMutex);
    
    // パッケージング
    SceneDrawInfo drawInfo
    {
        view,
        proj,
        mMeshDrawList,
        mPointLightDrawList,
        mSpotLightDrawList,
        mDirectionalLightDrawList,
        mAmbientLightDrawList,
        mSpriteDrawList,
        mPostProcessDrawList,
    };

    mSceneDrawList.push_back(drawInfo);

    // 一時リストのクリア
    {
        std::lock_guard<std::mutex> lock(mMeshDrawListMutex);
        mMeshDrawList.clear();
    }
    {
        std::lock_guard<std::mutex> lock(mPointLightDrawListMutex);
        mPointLightDrawList.clear();
    }
    {
        std::lock_guard<std::mutex> lock(mSpotLightDrawListMutex);
        mSpotLightDrawList.clear();
    }
    {
        std::lock_guard<std::mutex> lock(mDirectionalLightDrawListMutex);
        mDirectionalLightDrawList.clear();
    }
    {
        std::lock_guard<std::mutex> lock(mAmbientLightDrawListMutex);
        mAmbientLightDrawList.clear();
    }
    {
        std::lock_guard<std::mutex> lock(mSpriteDrawListMutex);
        mSpriteDrawList.clear();
    }
    {
        std::lock_guard<std::mutex> lock(mPostProcessDrawListMutex);
        mPostProcessDrawList.clear();
    }
}

bool Renderer::GetTexture(const std::string &fileName, ResourceID &outID)
{
    // すでに取得している場合はIDを返す
    auto iter = mTextureFileNameToID.find(fileName);
    if (iter != mTextureFileNameToID.end())
    {
        outID = iter->second;
        return true;
    }

    // バックエンドにリクエストしてIDを取得
    bool result = mBackend->GetTexture(fileName, outID);
    if (result)
    {
        mTextureFileNameToID[fileName] = outID;
    }
    return result;
}

bool Renderer::GetMesh(const std::string &fileName, ResourceID &outID)
{
    // すでに取得している場合はIDを返す
    auto iter = mMeshFileNameToID.find(fileName);
    if (iter != mMeshFileNameToID.end())
    {
        outID = iter->second;
        return true;
    }

    // バックエンドにリクエストしてIDを取得
    bool result = mBackend->GetMesh(fileName, outID);
    if (result)
    {
        mMeshFileNameToID[fileName] = outID;
    }
    return result;
}

bool Renderer::GetSkeleton(const std::string &fileName, ResourceID &outID)
{
    // すでに取得している場合はIDを返す
    auto iter = mSkeletonFileNameToID.find(fileName);
    if (iter != mSkeletonFileNameToID.end())
    {
        outID = iter->second;
        return true;
    }

    // バックエンドにリクエストしてIDを取得
    bool result = mBackend->GetSkeleton(fileName, outID);
    if (result)
    {
        mSkeletonFileNameToID[fileName] = outID;
    }
    return result;
}

bool Renderer::GetShader(const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName, ResourceID &outID)
{
    // すでに取得している場合はIDを返す
    auto iter = mShaderFileNameToID.find(std::make_pair(vertexShaderFileName, fragmentShaderFileName));
    if (iter != mShaderFileNameToID.end())
    {
        outID = iter->second;
        return true;
    }

    // バックエンドにリクエストしてIDを取得
    bool result = mBackend->GetShader(vertexShaderFileName, fragmentShaderFileName, outID);
    if (result)
    {
        mShaderFileNameToID[std::make_pair(vertexShaderFileName, fragmentShaderFileName)] = outID;
    }
    return result;
}

void Renderer::ReleaseTexture(ResourceID textureID)
{
    mBackend->ReleaseTexture(textureID);
}

void Renderer::ReleaseMesh(ResourceID meshID)
{
    mBackend->ReleaseMesh(meshID);
}

void Renderer::ReleaseSkeleton(ResourceID skeletonID)
{
    mBackend->ReleaseSkeleton(skeletonID);
}

void Renderer::ReleaseShader(ResourceID shaderID)
{
    mBackend->ReleaseShader(shaderID);
}

void Renderer::ReleaseTexture(const std::string &fileName)
{
    // ファイル名からIDを取得して解放
    auto iter = mTextureFileNameToID.find(fileName);
    if (iter != mTextureFileNameToID.end())
    {
        ReleaseTexture(iter->second);
        mTextureFileNameToID.erase(iter);
    }
}

void Renderer::ReleaseMesh(const std::string &fileName)
{
    auto iter = mMeshFileNameToID.find(fileName);
    if (iter != mMeshFileNameToID.end())
    {
        ReleaseMesh(iter->second);
        mMeshFileNameToID.erase(iter);
    }
}

void Renderer::ReleaseSkeleton(const std::string &fileName)
{
    auto iter = mSkeletonFileNameToID.find(fileName);
    if (iter != mSkeletonFileNameToID.end())
    {
        ReleaseSkeleton(iter->second);
        mSkeletonFileNameToID.erase(iter);
    }
}

void Renderer::ReleaseShader(const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName)
{
    auto iter = mShaderFileNameToID.find(std::make_pair(vertexShaderFileName, fragmentShaderFileName));
    if (iter != mShaderFileNameToID.end())
    {
        ReleaseShader(iter->second);
        mShaderFileNameToID.erase(iter);
    }
}

void Renderer::ReleaseAllResources()
{
    mBackend->ReleaseAllResources();
    mTextureFileNameToID    .clear();
    mMeshFileNameToID       .clear();
    mSkeletonFileNameToID   .clear();
    mShaderFileNameToID     .clear();
}

bool Renderer::GetTextureName(ResourceID textureID, std::string &outFileName)
{
    for (const auto &pair : mTextureFileNameToID)
    {
        if (pair.second == textureID)
        {
            outFileName = pair.first;
            return true;
        }
    }
    return false;
}

bool Renderer::GetMeshName(ResourceID meshID, std::string &outFileName)
{
    for (const auto &pair : mMeshFileNameToID)
    {
        if (pair.second == meshID)
        {
            outFileName = pair.first;
            return true;
        }
    }
    return false;
}

bool Renderer::GetSkeletonName(ResourceID skeletonID, std::string &outFileName)
{
    for (const auto &pair : mSkeletonFileNameToID)
    {
        if (pair.second == skeletonID)
        {
            outFileName = pair.first;
            return true;
        }
    }
    return false;
}

bool Renderer::GetShaderName(ResourceID shaderID, std::string &outVertexShaderFileName, std::string &outFragmentShaderFileName)
{
    for (const auto &pair : mShaderFileNameToID)
    {
        if (pair.second == shaderID)
        {
            outVertexShaderFileName = pair.first.first;
            outFragmentShaderFileName = pair.first.second;
            return true;
        }
    }
    return false;
}