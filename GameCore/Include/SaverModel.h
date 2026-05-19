#pragma once

#include "Mesh.h"
#include "Renderer.h"
#include "Skeleton.h"
#include <string>

class SaverModel
{
public:
    static bool SaveMeshJson(const std::string &fileName, Mesh &mesh, Renderer *renderer);
    static bool SaveMeshBinary(const std::string &fileName, Mesh &mesh, Renderer *renderer);

    static bool SaveSkeletonJson(const std::string &fileName, Skeleton &skeleton, Renderer *renderer);
    static bool SaveSkeletonBinary(const std::string &fileName, Skeleton &skeleton, Renderer *renderer);
};