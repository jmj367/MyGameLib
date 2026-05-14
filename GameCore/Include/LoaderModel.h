#include "Mesh.h"
#include "Skeleton.h"
#include <string>

class LoaderModel
{
public:
    static bool LoadMesh(const std::string &fileName, Mesh &mesh);
    static bool LoadSkeleton(const std::string &fileName, Skeleton &skeleton);

private:
    static bool LoadMeshJson(class JsonObject* data, Mesh &mesh);
    static bool LoadMeshJson(const std::string &fileName, Mesh &mesh);

    static bool LoadSkeletonJson(class JsonObject* data, Skeleton &skeleton);
    static bool LoadSkeletonJson(const std::string &fileName, Skeleton &skeleton);
    static bool LoadSkeletonBinary(const std::string &fileName, Skeleton &skeleton);
};