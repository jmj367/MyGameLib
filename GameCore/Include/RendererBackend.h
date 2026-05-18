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
    virtual ~RendererBackend() = default;

    virtual bool Initialize(void* windowHandle, float screenWidth, float screenHeight) = 0;
    virtual void Shutdown   () = 0;
    virtual void Draw       () = 0;

    // リソースの取得/解放
	virtual bool GetTexture     (const std::string &fileName, Renderer::ResourceID& outID) = 0;
	virtual bool GetMesh        (const std::string &fileName, Renderer::ResourceID& outID) = 0;
	virtual bool GetSkeleton    (const std::string &fileName, Renderer::ResourceID& outID) = 0;
	virtual bool GetShader      (const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName, Renderer::ResourceID& outID) = 0;
	virtual void ReleaseTexture (Renderer::ResourceID textureID ) = 0;
	virtual void ReleaseMesh    (Renderer::ResourceID meshID    ) = 0;
	virtual void ReleaseSkeleton(Renderer::ResourceID skeletonID) = 0;
	virtual void ReleaseShader  (Renderer::ResourceID shaderID  ) = 0;
	virtual void ReleaseAllResources() = 0;

    // フレーム描画
    struct FrameDrawInfo
    {
        Matrix4 View;
        Matrix4 Projection;
        const std::vector<Renderer::SpriteDrawInfo>&            Sprites;
        const std::vector<Renderer::MeshDrawInfo>&              Meshes;
        const std::vector<Renderer::SkinnedMeshDrawInfo>&       SkinnedMeshes;
        const std::vector<Renderer::PointLightDrawInfo>&        PointLights;
        const std::vector<Renderer::SpotLightDrawInfo>&         SpotLights;
        const std::vector<Renderer::DirectionalLightDrawInfo>&  DirectionalLights;
        const std::vector<Renderer::AmbientLightDrawInfo>&      AmbientLights;
    };
    virtual void DrawFrame(const FrameDrawInfo& drawInfo) = 0;
};