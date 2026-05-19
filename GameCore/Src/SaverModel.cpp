#include "SaverModel.h"
#include "JsonData.h"
#include <fstream>

bool SaverModel::SaveMeshJson(const std::string &fileName, Mesh &mesh, Renderer *renderer)
{
    JsonData data;
    if (!data.IsType<JsonObject>())
    {
        return false;
    }

    JsonObject root = data.GetObject();

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
    if (outFile.is_open())
    {
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
        outFile.write(reinterpret_cast<const char *>(mesh.GetVertexArray()->GetVertexBuffer()),
                      mesh.GetVertexArray()->GetNumVerts() * vertexSize);
        // インデックスの書き込み
        outFile.write(reinterpret_cast<const char *>(mesh.GetVertexArray()->GetIndexBuffer()),
                      mesh.GetVertexArray()->GetNumIndices() * sizeof(uint32_t));
    }
}

bool SaverModel::SaveSkeletonJson(const std::string &fileName, Skeleton &skeleton, Renderer *renderer)
{
    // TODO: 実装
    return false;
}

bool SaverModel::SaveSkeletonBinary(const std::string &fileName, Skeleton &skeleton, Renderer *renderer)
{
    // TODO: 実装
     return false;
}