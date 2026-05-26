#pragma once

#include "Define.h"
#include "Math.h"
#include "MatrixPalette.h"
#include "Renderer.h"
#include <string>
#include <vector>

// レンダラーのバックエンドのインターフェース
// Rendererクラスは描画コマンドの管理とAPIに依存しないインターフェースを提供し、
// RendererBackendクラスは実際の描画処理とリソース管理を担当する
// このクラスを継承して、OpenGLやDirectX等の具体的なバックエンドを実装する
class RendererBackend
{
public:
    RendererBackend(Renderer* renderer)
        : mRenderer(renderer)
    {
    };
    virtual ~RendererBackend() = default;

    virtual bool PrepareWindow() = 0; // ウィンドウ作成前の初期化処理(必要なら)
    virtual bool Initialize(void* windowHandle, float screenWidth, float screenHeight) = 0;
    virtual void Shutdown   () = 0;

    // リソースの取得/解放
	virtual bool GetTexture     (const std::string &fileName, ResourceID& outID) = 0;
	virtual bool GetMesh        (const std::string &fileName, ResourceID& outID) = 0;
	virtual bool GetSkeleton    (const std::string &fileName, ResourceID& outID) = 0;
	virtual bool GetShader      (const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName, ResourceID& outID) = 0;
	virtual void ReleaseTexture (ResourceID textureID ) = 0;
	virtual void ReleaseMesh    (ResourceID meshID    ) = 0;
	virtual void ReleaseSkeleton(ResourceID skeletonID) = 0;
	virtual void ReleaseShader  (ResourceID shaderID  ) = 0;
	virtual void ReleaseAllResources() = 0;

    // フレーム描画
    struct FrameDrawInfo
    {
        Matrix4 View;
        Matrix4 Projection;
        const std::vector<Renderer::SpriteDrawInfo          > &SpriteDrawInfos            ;
        const std::vector<Renderer::MeshDrawInfo            > &MeshDrawInfos              ;
        const std::vector<Renderer::SkinnedMeshDrawInfo     > &SkinnedMeshDrawInfos       ;
        const std::vector<Renderer::PointLightDrawInfo      > &PointLightDrawInfos        ;
        const std::vector<Renderer::SpotLightDrawInfo       > &SpotLightDrawInfos         ;
        const std::vector<Renderer::DirectionalLightDrawInfo> &DirectionalLightDrawInfos  ;
        const std::vector<Renderer::AmbientLightDrawInfo    > &AmbientLightDrawInfos      ;
        const std::vector<Renderer::PostProcessDrawInfo     > &PostProcessDrawInfos       ;
    };
    virtual void DrawFrame(const FrameDrawInfo& drawInfo) = 0;

    Renderer* GetRenderer() const { return mRenderer; }

private:
    Renderer* mRenderer;

};