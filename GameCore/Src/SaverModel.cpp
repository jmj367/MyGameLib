#include "SaverModel.h"
#include "JsonData.h"

bool SaverModel::SaveMeshJson(const std::string &fileName, const Mesh &mesh)
{
    JsonData data;
    if(!data.IsType<JsonObject>())
    {
        return false;
    }

    JsonObject root = data.GetObject();
    
}

bool SaverModel::SaveMeshBinary(const std::string &fileName, const Mesh &mesh)
{

}

bool SaverModel::SaveSkeletonJson(const std::string &fileName, const Skeleton &skeleton)
{

}

bool SaverModel::SaveSkeletonBinary(const std::string &fileName, const Skeleton &skeleton)
{

}