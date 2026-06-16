#pragma once

#include "RendererBackend.h"
#include "Define.h"
#include "GBuffer.h"
#include "Mesh.h"
#include "PostProcessBuffer.h"
#include "Shader.h"
#include "Skeleton.h"
#include "Texture.h"
#include "VertexArray.h"
#include <SDL2/SDL.h>
#include <vector>

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

    // フレーム描画
    void DrawFrame(const FrameDrawInfo &drawInfo) override;

private:
    // 描画の各段階
    void DrawMesh(
        const Matrix4 &view, 
        const Matrix4 &proj,
        const std::vector<Renderer::MeshDrawInfo> &drawInfo
    );
    void DrawLighting(
        const Matrix4 &view,
        const Matrix4 &proj,
        const std::vector<Renderer::PointLightDrawInfo> &pointLightInfo,
        const std::vector<Renderer::SpotLightDrawInfo> &spotLightInfo,
        const std::vector<Renderer::DirectionalLightDrawInfo> &directionalLightInfo,
        const std::vector<Renderer::AmbientLightDrawInfo> &ambientLightInfo
    );
    //void DrawTransparent(const Renderer::SceneDrawInfo &drawInfo);
    //void DrawEffects(const Renderer::SceneDrawInfo &drawInfo);
    //void Draw3DSprites(const Renderer::SceneDrawInfo &drawInfo);
    //void Draw2DSprites(const Renderer::SceneDrawInfo &drawInfo);
    void DrawPostProcess(const std::vector<Renderer::PostProcessDrawInfo> &postProcessInfo);
    //void DrawUI(const Renderer::SceneDrawInfo &drawInfo);

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
    // ポストプロセス用バッファ
    PostProcessBuffer mPostProcessBuffer;

    // スクリーンサイズ
    float mScreenWidth;
    float mScreenHeight;

    // 球メッシュ(ライトの描画等で使用)
    ResourceID mSphereMesh;
    // シェーダー(ライトの描画等で使用)
    ResourceID mLightShader;
    
};