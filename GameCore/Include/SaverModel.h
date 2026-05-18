#pragma once

#include "Mesh.h"
#include "Skeleton.h"
#include <string>

class SaverModel
{
public:
    static bool SaveMeshJson(const std::string &fileName, const Mesh &mesh);
    static bool SaveMeshBinary(const std::string &fileName, const Mesh &mesh);

    static bool SaveSkeletonJson(const std::string &fileName, const Skeleton &skeleton);
    static bool SaveSkeletonBinary(const std::string &fileName, const Skeleton &skeleton);
};