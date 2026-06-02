#pragma once

#include "RendererBackend.h"
#include "Define.h"
#include "GBuffer.h"
#include "Mesh.h"
#include "Shader.h"
#include "Skeleton.h"
#include "Texture.h"
#include "VertexArray.h"
#include <SDL2/SDL.h>

class OpenGLRendererBackend : public RendererBackend
{
public:
    OpenGLRendererBackend();
    ~OpenGLRendererBackend() override;

    bool PrepareWindow() override;
    bool Initialize(void *windowHandle, float screenWidth, float screenHeight) override;
    void Shutdown() override;

    // リソースの取得/解放
    bool GetTexture     (const std::string &fileName, ResourceID &outID) override;
    bool GetMesh        (const std::string &fileName, ResourceID &outID) override;
    bool GetSkeleton    (const std::string &fileName, ResourceID &outID) override;
    bool GetShader      (const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName, ResourceID &outID) override;
    void ReleaseTexture (ResourceID textureID ) override;
    void ReleaseMesh    (ResourceID meshID    ) override;
    void ReleaseSkeleton(ResourceID skeletonID) override;
    void ReleaseShader  (ResourceID shaderID  ) override;
    void ReleaseAllResources() override;

    void DrawFrame(const FrameDrawInfo &drawInfo) override;

private:
    // 描画の各段階
    void DrawMesh(const FrameDrawInfo &drawInfo);
    void DrawLighting(const FrameDrawInfo &drawInfo);
    void DrawForward(const FrameDrawInfo &drawInfo);
    void DrawEffects(const FrameDrawInfo &drawInfo);
    void DrawSprites(const FrameDrawInfo &drawInfo);
    void DrawPostProcess(const FrameDrawInfo &drawInfo);
    void DrawUI(const FrameDrawInfo &drawInfo);

    // リソースのキャッシュ
    std::unordered_map<ResourceID, Texture    > mTextures;
    std::unordered_map<ResourceID, Mesh       > mMeshes;
    std::unordered_map<ResourceID, Skeleton   > mSkeletons;
    std::unordered_map<ResourceID, Shader     > mShaders;

    // リソースIDの管理
    ResourceID mNextResourceID;

    // OpenGLのコンテキスト
    SDL_GLContext mGLContext;

    // Gバッファ
    GBuffer mGBuffer;

    // スクリーンサイズ
    float mScreenWidth;
    float mScreenHeight;
};