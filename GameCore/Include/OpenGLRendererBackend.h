#include "RendererBackend.h"
#include "Mesh.h"
#include "Shader.h"
#include "Skeleton.h"
#include "Texture.h"

class OpenGLRendererBackend : public RendererBackend
{
public:
    OpenGLRendererBackend(Renderer *renderer);
    ~OpenGLRendererBackend() override;

    bool Initialize(void *windowHandle, float screenWidth, float screenHeight) override;
    void Shutdown() override;
    void Draw() override;

    // リソースの取得/解放
    bool GetTexture     (const std::string &fileName, Renderer::ResourceID &outID) override;
    bool GetMesh        (const std::string &fileName, Renderer::ResourceID &outID) override;
    bool GetSkeleton    (const std::string &fileName, Renderer::ResourceID &outID) override;
    bool GetShader      (const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName, Renderer::ResourceID &outID) override;
    void ReleaseTexture (Renderer::ResourceID textureID ) override;
    void ReleaseMesh    (Renderer::ResourceID meshID    ) override;
    void ReleaseSkeleton(Renderer::ResourceID skeletonID) override;
    void ReleaseShader  (Renderer::ResourceID shaderID  ) override;
    void ReleaseAllResources() override;

    void DrawFrame(const FrameDrawInfo &drawInfo) override;

private:
    // リソースのキャッシュ
    std::unordered_map<Renderer::ResourceID, Texture    > mTextures;
    std::unordered_map<Renderer::ResourceID, Mesh       > mMeshes;
    std::unordered_map<Renderer::ResourceID, Skeleton   > mSkeletons;
    std::unordered_map<Renderer::ResourceID, Shader     > mShaders;
};