#include "Renderer.h"
#include "Game.h"

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
};

void Renderer::Shutdown()
{
}

void Renderer::UnloadData()
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

Renderer::ResourceID Renderer::GetTexture(const std::string &fileName)
{
}

Renderer::ResourceID Renderer::GetMesh(const std::string &fileName)
{
}

Renderer::ResourceID Renderer::GetSkeleton(const std::string &fileName)
{
}

Renderer::ResourceID Renderer::GetShader(const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName)
{
}

void Renderer::ReleaseTexture(ResourceID textureID)
{
}

void Renderer::ReleaseMesh(ResourceID meshID)
{
}

void Renderer::ReleaseSkeleton(ResourceID skeletonID)
{
}

void Renderer::ReleaseShader(ResourceID shaderID)
{
}

void Renderer::ReleaseTexture(const std::string &fileName)
{
}

void Renderer::ReleaseMesh(const std::string &fileName)
{
}

void Renderer::ReleaseSkeleton(const std::string &fileName)
{
}

void Renderer::ReleaseShader(const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName)
{
}

void Renderer::ReleaseAllResources()
{
}