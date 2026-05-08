#include "Renderer.h"
#include "Game.h"
#include "Shader.h"
#include "Texture.h"

Renderer::Renderer(Game *game)
    : mGame(game), mScreenWidth(0.0f), mScreenHeight(0.0f)
{
}

Renderer::~Renderer()
{
}

bool Renderer::Initialize(float screenWidth, float screenHeight)
{
    mScreenWidth = screenWidth;
    mScreenHeight = screenHeight;

    return true;
}

void Renderer::Shutdown()
{
}

void Renderer::Draw()
{
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

void Renderer::DrawSkinnedMesh(const SkinnedMeshDrawInfo &skinnedMeshInfo)
{
    std::lock_guard<std::mutex> lock(mSkinnedMeshDrawListMutex);
    mSkinnedMeshDrawList.push_back(skinnedMeshInfo);
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

bool Renderer::GetTexture(const std::string &fileName, ResourceID &outID)
{
    // 既にキャッシュされているか確認
    auto iter = mTextureFileNameToID.find(fileName);
    if (iter != mTextureFileNameToID.end())
    {
        outID = iter->second;
        return true;
    }

    // キャッシュされていない場合は新規に読み込む
    Texture texture;
    if (texture.Load(fileName))
    {
        ResourceID id = mNextResourceID++;
        mTextures.emplace(id, std::move(texture));
        mTextureFileNameToID[fileName] = id;
        outID = id;
        return true;
    }

    return false;
}

bool Renderer::GetMesh(const std::string &fileName, ResourceID &outID)
{
    return false;
}

bool Renderer::GetSkeleton(const std::string &fileName, ResourceID &outID)
{
    return false;
}

bool Renderer::GetShader(const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName, ResourceID &outID)
{
    // シェーダーは頂点シェーダーとフラグメントシェーダーの組み合わせで管理する
    std::pair<std::string, std::string> shaderKey(vertexShaderFileName, fragmentShaderFileName);
    auto iter = mShaderFileNameToID.find(shaderKey);
    if (iter != mShaderFileNameToID.end())
    {
        outID = iter->second;
        return true;
    }

    // キャッシュされていない場合は新規に読み込む
    Shader shader;
    if (shader.Load(vertexShaderFileName, fragmentShaderFileName))
    {
        ResourceID id = mNextResourceID++;
        mShaders.emplace(id, std::move(shader));
        mShaderFileNameToID[shaderKey] = id;
        outID = id;
        return true;
    }

    return false;
}

void Renderer::ReleaseTexture(ResourceID textureID)
{
    // テクスチャIDからTextureオブジェクトを取得
    auto iter = mTextures.find(textureID);
    if (iter != mTextures.end())
    {
        // テクスチャをアンロード
        iter->second.Unload();

        // ファイル名からIDのマップからも削除
        const std::string &fileName = iter->second.GetFileName();
        mTextureFileNameToID.erase(fileName);

        // テクスチャのキャッシュから削除
        mTextures.erase(iter);
    }
}

void Renderer::ReleaseMesh(ResourceID meshID)
{
}

void Renderer::ReleaseSkeleton(ResourceID skeletonID)
{
}

void Renderer::ReleaseShader(ResourceID shaderID)
{
    // シェーダーIDからShaderオブジェクトを取得
    auto iter = mShaders.find(shaderID);
    if (iter != mShaders.end())
    {
        // シェーダーをアンロード
        iter->second.Unload();

        // ファイル名からIDのマップからも削除
        const std::string &vertexShaderFileName = iter->second.GetVertexShaderFileName();
        const std::string &fragmentShaderFileName = iter->second.GetFragmentShaderFileName();
        mShaderFileNameToID.erase(std::make_pair(vertexShaderFileName, fragmentShaderFileName));

        // シェーダーのキャッシュから削除
        mShaders.erase(iter);
    }
}

void Renderer::ReleaseTexture(const std::string &fileName)
{
    // ファイル名からIDを取得
    auto iter = mTextureFileNameToID.find(fileName);
    if (iter != mTextureFileNameToID.end())
    {
        ResourceID id = iter->second;
        ReleaseTexture(id);
    }
}

void Renderer::ReleaseMesh(const std::string &fileName)
{
}

void Renderer::ReleaseSkeleton(const std::string &fileName)
{
}

void Renderer::ReleaseShader(const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName)
{
    // ファイル名からIDを取得
    auto iter = mShaderFileNameToID.find(std::make_pair(vertexShaderFileName, fragmentShaderFileName));
    if (iter != mShaderFileNameToID.end())
    {
        ResourceID id = iter->second;
        ReleaseShader(id);
    }
}

void Renderer::ReleaseAllResources()
{
    // テクスチャを全てアンロードしてキャッシュもクリア
    for (auto &pair : mTextures)
    {
        pair.second.Unload();
    }
    mTextures.clear();
    mTextureFileNameToID.clear();

    // シェーダーを全てアンロードしてキャッシュもクリア
    for (auto &pair : mShaders)
    {
        pair.second.Unload();
    }
    mShaders.clear();
    mShaderFileNameToID.clear();

    // TODO: メッシュ、スケルトンも同様にアンロードしてキャッシュをクリアする
}