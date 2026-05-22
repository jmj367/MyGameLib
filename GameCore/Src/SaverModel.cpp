#include "SaverModel.h"
#include "JsonData.h"
#include <fstream>

bool SaverModel::SaveMeshJson(const std::string &fileName, Mesh &mesh, Renderer *renderer)
{
    // JSONデータの作成
    JsonData data;
    if (!data.IsType<JsonObject>())
    {
        return false;
    }

    JsonObject root = data.GetObject();

    // メッシュのプロパティをJSONに追加
    root.AddMember("vertexformat", mesh.GetVertexArray()->GetLayoutName());
    root.AddMember("shader", mesh.GetShaderName());
    JsonArray textures;
    for (size_t i = 0; i < mesh.GetNumTextures(); i++)
    {
        // テクスチャIDからテクスチャ名を取得
        std::string texName;
        if (!renderer->GetTexture(mesh.GetTexture(i), texName))
        {
            return false;
        }
        textures.PushBack(texName);
    }
    root.AddMember("textures", textures);
    root.AddMember("specularPower", mesh.GetSpecPower());
    JsonArray vertices;
    VertexArray *vertexArray = mesh.GetVertexArray();
    for (size_t i = 0; i < vertexArray->GetNumVerts(); i++)
    {
        JsonArray vert;
        const void* vertData = static_cast<const char*>(vertexArray->GetVertexBuffer()) + i * VertexArray::GetVertexSize(vertexArray->GetLayout());
        for (size_t j = 0; j < VertexArray::GetVertexSize(vertexArray->GetLayout()) / sizeof(float); j++)
        {
            vert.PushBack(static_cast<const float*>(vertData)[j]);
        }
        vertices.PushBack(vert);
    }
    root.AddMember("vertices", vertices);

    return data.SaveJSON(fileName);
}

bool SaverModel::SaveMeshBinary(const std::string &fileName, Mesh &mesh, Renderer *renderer)
{
    // ヘッダー情報の作成
    MeshBinary::MeshBinHeader header;
    header.mLayout = mesh.GetVertexArray()->GetLayout();
    header.mNumTextures = static_cast<unsigned>(mesh.GetNumTextures());
    header.mNumVerts = mesh.GetVertexArray()->GetNumVerts();
    header.mNumIndices = mesh.GetVertexArray()->GetNumIndices();
    header.mRadius = mesh.GetRadius();
    header.mSpecPower = mesh.GetSpecPower();

    // セーブファイルの作成
    std::ofstream outFile(fileName, std::ios::out | std::ios::binary);

    if(!outFile.is_open())
    {
        return false;
    }

    // ヘッダー情報の書き込み
    outFile.write(reinterpret_cast<char *>(&header), sizeof(header));

    // テクスチャファイル名の書き込み
    for (size_t i = 0; i < mesh.GetNumTextures(); i++)
    {
        // テクスチャIDからテクスチャ名を取得
        std::string texName;
        if (!renderer->GetTexture(mesh.GetTexture(i), texName))
        {
            return false;
        }

        // ファイル名のサイズを書き込み
        uint16_t nameSize = static_cast<uint16_t>(texName.length()) + 1;
        outFile.write(reinterpret_cast<char *>(&nameSize), sizeof(nameSize));
        outFile.write(texName.c_str(), nameSize - 1);
        outFile.write("\0", 1);
    }

    // 頂点サイズの取得
    unsigned vertexSize = VertexArray::GetVertexSize(mesh.GetVertexArray()->GetLayout());

    // 頂点の書き込み
    char* vertData = static_cast<char*>(mesh.GetVertexArray()->GetVertexBuffer());
    outFile.write(vertData, mesh.GetVertexArray()->GetNumVerts() * vertexSize);

    // インデックスの書き込み
    uint32_t* indexData = static_cast<uint32_t*>(mesh.GetVertexArray()->GetIndexBuffer());
    outFile.write(reinterpret_cast<const char*>(indexData), mesh.GetVertexArray()->GetNumIndices() * sizeof(uint32_t));
    
    return true;
}

bool SaverModel::SaveSkeletonJson(const std::string &fileName, Skeleton &skeleton, Renderer *renderer)
{
    // JSONデータの作成
    JsonData data;
    if (!data.IsType<JsonObject>())
    {
        return false;
    }

    JsonObject root = data.GetObject();

    // スケルトンのプロパティをJSONに追加
    root.AddMember("bonecount", static_cast<uint32_t>(skeleton.GetNumBones()));
    JsonArray bones;
    for (size_t i = 0; i < skeleton.GetNumBones(); i++)
    {
        const Skeleton::Bone &bone = skeleton.GetBone(i);

        JsonObject boneObj;
        boneObj.AddMember("name", bone.Name.c_str());
        boneObj.AddMember("parent", bone.Parent);
        
        JsonObject localBindPose;

        JsonArray rotation;
        rotation.PushBack(bone.LocalBindPose.mRotation.x);
        rotation.PushBack(bone.LocalBindPose.mRotation.y);
        rotation.PushBack(bone.LocalBindPose.mRotation.z);
        rotation.PushBack(bone.LocalBindPose.mRotation.w);
        localBindPose.AddMember("rot", rotation);

        JsonArray translation;
        translation.PushBack(bone.LocalBindPose.mTranslation.x);
        translation.PushBack(bone.LocalBindPose.mTranslation.y);
        translation.PushBack(bone.LocalBindPose.mTranslation.z);
        localBindPose.AddMember("trans", translation);

        boneObj.AddMember("bindpose", localBindPose);
        bones.PushBack(boneObj);
    }
    root.AddMember("bones", bones);

    return data.SaveJSON(fileName);
}

bool SaverModel::SaveSkeletonBinary(const std::string &fileName, Skeleton &skeleton, Renderer *renderer)
{
    // ヘッダー情報の作成
    SkeletonBinary::SkeletonBinHeader header;
    header.mNumBones = static_cast<unsigned>(skeleton.GetNumBones());

    // セーブファイルの作成
    std::ofstream outFile(fileName, std::ios::out | std::ios::binary);

    if(!outFile.is_open())
    {
        return false;
    }

    // ヘッダー情報の書き込み
    outFile.write(reinterpret_cast<char *>(&header), sizeof(header));

    // ボーンの書き込み
    for (size_t i = 0; i < skeleton.GetNumBones(); i++)
    {
        const Skeleton::Bone &bone = skeleton.GetBone(i);

        // 名前の書き込み
        uint16_t nameSize = static_cast<uint16_t>(bone.Name.length()) + 1;
        outFile.write(reinterpret_cast<char *>(&nameSize), sizeof(nameSize));
        outFile.write(bone.Name.c_str(), nameSize - 1);
        outFile.write("\0", 1);

        // 親インデックスの書き込み
        outFile.write(reinterpret_cast<const char*>(&bone.Parent), sizeof(bone.Parent));

        // ローカルバインドポーズの書き込み
        outFile.write(reinterpret_cast<const char*>(&bone.LocalBindPose.mRotation), sizeof(bone.LocalBindPose.mRotation));
        outFile.write(reinterpret_cast<const char*>(&bone.LocalBindPose.mTranslation), sizeof(bone.LocalBindPose.mTranslation));
    }

    return true;
}