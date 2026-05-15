#include "Mesh.h"
#include "Skeleton.h"
#include "VertexArray.h"
#include <cstdint>
#include <string>

// NOTE: 将来fbx等を読み込む際に、mesh,skeleton等をまとめて読み込むことが予想されるので
// モデルデータ全般のローダーを作成しておく
class LoaderModel
{
public:
    static bool LoadMesh(const std::string &fileName, Mesh &mesh, class Renderer *renderer);
    static bool LoadSkeleton(const std::string &fileName, Skeleton &skeleton);

private:
    static bool LoadMeshJson(class JsonObject *data, Mesh &mesh, class Renderer *renderer);
    static bool LoadMeshJson(const std::string &fileName, Mesh &mesh, class Renderer *renderer);
    static bool LoadMeshBinary(const std::string &fileName, Mesh &mesh, class Renderer *renderer);

    static bool LoadSkeletonJson(class JsonObject *data, Skeleton &skeleton);
    static bool LoadSkeletonJson(const std::string &fileName, Skeleton &skeleton);
    static bool LoadSkeletonBinary(const std::string &fileName, Skeleton &skeleton);
};