#include "JsonData.h"
#include "LoaderModel.h"
#include "MatrixPalette.h"
#include <vector>
#include <unordered_map>

bool LoaderModel::LoadSkeleton(const std::string &fileName, Skeleton &skeleton)
{
    // ファイルの拡張子を確認して、適切なローダー関数を呼び出す
    size_t extPos = fileName.find_last_of('.');
    if (extPos == std::string::npos)
    {
        // 拡張子がない場合はエラー
        return false;
    }

    std::string ext = fileName.substr(extPos);
    if (ext == ".json")
    {
        return LoadSkeletonJson(fileName, skeleton);
    }
    else if (ext == ".bin")
    {
        return LoadSkeletonBinary(fileName, skeleton);
    }
    else
    {
        // 対応していない拡張子
        return false;
    }

    return true;
}

bool LoaderModel::LoadSkeletonJson(JsonObject* data, Skeleton &skeleton)
{
    // ボーン数の取得
    int boneCount;
    if (!data->Get("bonecount", boneCount))
    {
        return false;
    }

    size_t count = static_cast<size_t>(boneCount);
    if (count > MAX_SKELETON_BONES)
    {
        return false;
    }

    std::vector<Skeleton::Bone> bones;
    bones.reserve(count);

    JsonArray bonesArray;
    if (!data->Get("bones", bonesArray))
    {
        return false;
    }
    if (bonesArray.Size() != count)
    {
        return false;
    }

    // ボーンをvectorに格納
    std::unordered_map<int, std::vector<int>> boneChildrenIndex;
    for (int i = 0; i < count; i++)
    {
        JsonObject bone;
        if (!bonesArray.Get(i, bone))
        {
            return false;
        }

        Skeleton::Bone temp;

        std::string name;
        if (!bone.Get("name", name))
        {
            return false;
        }
        temp.Name = name;

        int parent;
        if (!bone.Get("parent", parent))
        {
            return false;
        }
        temp.Parent = parent;

        JsonObject bindpose;
        if (!bone.Get("bindpose", bindpose))
        {
            return false;
        }

        JsonArray rot;
        if (!bindpose.Get("rot", rot))
        {
            return false;
        }

        float rotf[4];
        for (int j = 0; j < 4; j++)
        {
            if (!rot.Get(j, rotf[j]))
            {
                return false;
            }
        }
        temp.LocalBindPose.mRotation.Set(rotf[0], rotf[1], rotf[2], rotf[3]);

        JsonArray trans;
        if (!bindpose.Get("trans", trans))
        {
            return false;
        }

        float transf[3];
        for (int j = 0; j < 3; j++)
        {
            if (!trans.Get(j, transf[j]))
            {
                return false;
            }
        }
        temp.LocalBindPose.mTranslation.Set(transf[0], transf[1], transf[2]);

        bones.push_back(temp);

        // 親と子の対応
        boneChildrenIndex[temp.Parent].push_back(i);
    }
    return skeleton.Initialize(bones, boneChildrenIndex);
}

bool LoaderModel::LoadSkeletonJson(const std::string &fileName, Skeleton &skeleton)
{
    JsonData data;
    if (!JsonData::LoadJSON(fileName, data))
    {
        return false;
    }
    JsonObject rootObj = data.GetObject();
    return LoadSkeletonJson(&rootObj, skeleton);
}

bool LoaderModel::LoadSkeletonBinary(const std::string &fileName, Skeleton &skeleton)
{
    // TODO: 実装
    return false;
}