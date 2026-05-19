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
}

bool SaverModel::SaveSkeletonJson(const std::string &fileName, Skeleton &skeleton, Renderer *renderer)
{
}

bool SaverModel::SaveSkeletonBinary(const std::string &fileName, Skeleton &skeleton, Renderer *renderer)
{
}