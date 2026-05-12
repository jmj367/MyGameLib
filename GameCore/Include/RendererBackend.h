#include "Math.h"
#include "MatrixPalette.h"
#include <string>
#include <vector>

// 前方宣言
class Renderer
{
public:
    using ResourceID = size_t;

    struct SpriteDrawInfo;
    struct MeshDrawInfo;
    struct SkinnedMeshDrawInfo;
    struct PointLightDrawInfo;
    struct SpotLightDrawInfo;
    struct DirectionalLightDrawInfo;
    struct AmbientLightDrawInfo;
};

class RendererBackend
{
public:
    virtual ~RendererBackend() = default;

    virtual bool Initialize(void* windowHandle, float screenWidth, float screenHeight) = 0;
    virtual void Shutdown   () = 0;
    virtual void Draw       () = 0;

    // リソースの取得/解放
	bool GetTexture     (const std::string &fileName, Renderer::ResourceID& outID);
	bool GetMesh        (const std::string &fileName, Renderer::ResourceID& outID);
	bool GetSkeleton    (const std::string &fileName, Renderer::ResourceID& outID);
	bool GetShader      (const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName, Renderer::ResourceID& outID);
	void ReleaseTexture (Renderer::ResourceID textureID );
	void ReleaseMesh    (Renderer::ResourceID meshID    );
	void ReleaseSkeleton(Renderer::ResourceID skeletonID);
	void ReleaseShader  (Renderer::ResourceID shaderID  );
	void ReleaseAllResources();

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
    virtual void Draw(const FrameDrawInfo& drawInfo) = 0;
};