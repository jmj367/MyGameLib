#include "JsonData.h"
#include "LoaderModel.h"
#include "MatrixPalette.h"
#include "Renderer.h"
#include <fstream>
#include <vector>
#include <unordered_map>

bool LoaderModel::LoadMesh(const std::string &fileName, Mesh &mesh, Renderer *renderer)
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
        return LoadMeshJson(fileName, mesh, renderer);
    }
    else if (ext == ".bin")
    {
        return LoadMeshBinary(fileName, mesh, renderer);
    }
    else
    {
        // 対応していない拡張子
        return false;
    }

    return true;
}

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

bool LoaderModel::LoadMeshJson(JsonObject* data, Mesh &mesh, Renderer *renderer)
{
    std::string shaderName;
    if (!data->Get("shader", shaderName))
    {
        return false;
    }

    VertexArray::Layout layout = VertexArray::PosNormTex;
    size_t vertSize = 8;

    std::string vertexFormat;
    if (!data->Get("vertexformat", vertexFormat))
    {
        return false;
    }
    if (vertexFormat == "PosNormSkinTex")
    {
        layout = VertexArray::PosNormSkinTex;
        vertSize = 10;
    }

    // テクスチャの読み込み
    std::vector<Mesh::ResourceID> textureIDs;
    JsonArray textures;
    if (!data->Get("textures", textures) || textures.Size() < 1)
    {
        return false;
    }

    float specPower;
    if (!data->Get("specularPower", specPower))
    {
        return false;
    }

    std::vector<std::string> textureNames;
    for (size_t i = 0; i < textures.Size(); i++)
    {
        // テクスチャが存在するか確認
        std::string texName;
        if (!textures.Get(i, texName))
        {
            return false;
        }
        textureNames.push_back(texName);
        Mesh::ResourceID t;
        if (!renderer->GetTexture(texName, t))
        {
            return false;
        }
        textureIDs.push_back(t);
    }

    // 頂点データの読み込み
    JsonArray verts;
    if (!data->Get("verts", verts) || verts.Size() < 1)
    {
        return false;
    }

    std::vector<MeshBinary::Vertex> vertices;
    vertices.reserve(verts.Size());
    float radius = 0.0f;
    for (size_t i = 0; i < verts.Size(); i++)
    {
        // 各頂点の読み込み
        JsonArray vert;
        if (!verts.Get(i, vert))
        {
            return false;
        }

        Vector3 pos;
        if (!vert.Get(0, pos))
        {
            return false;
        }

        radius = Math::Max(radius, pos.LengthSq());

        if(layout == VertexArray::PosNormTex)
        {
            MeshBinary::Vertex v;
            // 位置・法線・テクスチャ座標を追加
            for(int j = 0; j < vert.Size(); j++)
            {
                if(!vert.Get(j, v.f))
                {
                    return false;
                }
                 vertices.emplace_back(v);
            }
        }
        else if (layout == VertexArray::PosNormSkinTex)
        {
            MeshBinary::Vertex v;
            // 位置・法線を追加
            for(int j = 0; j < 6; j++)
            {
                if(!vert.Get(j, v.f))
                {
                    return false;
                }
                 vertices.emplace_back(v);
            }

            // スキンウェイトを追加
            for(int j = 6; j < 14; j += 4)
            {
                MeshBinary::Vertex v;
                uint32_t b;
                if(!vert.Get(j, b))
                {
                    return false;
                }
                v.b[0] = b;

				if(!vert.Get(j + 1, b))
				{
					return false;
				}
				v.b[1] = b;

				if(!vert.Get(j + 2, b))
				{
					return false;
				}
				v.b[2] = b;

				if(!vert.Get(j + 3, b))
				{
					return false;
				}
				v.b[3] = b;

				vertices.emplace_back(v);
            }

            // テクスチャ座標を追加
            for(int j = 14; j < vert.Size(); j++)
            {
                if(!vert.Get(j, v.f))
                {
                    return false;
                }
                 vertices.emplace_back(v);
            }
        }
    }

    // 半径を平方根に変換
    radius = Math::Sqrt(radius);

    JsonArray indJson;
    if (!data->Get("indices", indJson) || indJson.Size() < 1)
    {
        return false;
    }

    std::vector<uint32_t> indices;
    indices.reserve(indJson.Size());
    for (int i = 0; i < indJson.Size(); i++)
    {
        JsonArray ind;
        if (!indJson.Get(i, ind) || ind.Size() != 3)
        {
            return false;
        }

        unsigned int index;
		if(!ind.Get(0, index))
		{
			return false;
		}
		indices.emplace_back(index);

		if(!ind.Get(1, index))
		{
			return false;
		}
		indices.emplace_back(index);

		if(!ind.Get(2, index))
		{
			return false;
		}
		indices.emplace_back(index);
    }

    VertexArray vertexArray(
        vertices.data(), 
        static_cast<unsigned int>(vertices.size()), 
        layout, 
        indices.data(), 
        static_cast<unsigned int>(indices.size()));
    mesh.Initialize(textureIDs, vertexArray, shaderName, radius, specPower);

    return true;
}

bool LoaderModel::LoadMeshJson(const std::string &fileName, Mesh &mesh, Renderer *renderer)
{
    JsonData data;
    if (!data.LoadJSON(fileName))
    {
        return false;
    }
    JsonObject rootObj = data.GetObject();
    return LoadMeshJson(&rootObj, mesh, renderer);
}

bool LoaderModel::LoadMeshBinary(const std::string &fileName, Mesh &mesh, Renderer *renderer)
{
    std::ifstream file(fileName, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    // ヘッダーの読み込み
    MeshBinary::MeshBinHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    // シグネチャの確認
    char* sig = header.mSignature;
    if (sig[0] != 'G' || sig[1] != 'M' || sig[2] != 'S' || sig[3] != 'H')
    {
        return false;
    }

    // テクスチャの読み込み
    std::vector<Mesh::ResourceID> textureIDs;
    for(int i = 0; i < header.mNumTextures; i++)
    {
        uint16_t nameSize = 0;
        file.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        
        // 名前の読み込み
        char* texName = new char[nameSize];
        file.read(texName, nameSize);

        // テクスチャの取得
        Mesh::ResourceID t;
        if (!renderer->GetTexture(texName, t))
        {
            delete[] texName;
            return false;
        }
        textureIDs.push_back(t);
        delete[] texName;
    }

    // 頂点データの読み込み
    unsigned vertexSize = VertexArray::GetVertexSize(header.mLayout);
    char* verts = new char[header.mNumVerts * vertexSize];
    file.read(verts, header.mNumVerts * vertexSize);

    // インデックスの読み込み
    uint32_t* indices = new uint32_t[header.mNumIndices];
    file.read(reinterpret_cast<char*>(indices), header.mNumIndices * sizeof(uint32_t));

    VertexArray vertexArray(verts, header.mNumVerts, header.mLayout, indices, header.mNumIndices);

    // メモリ解放
    delete[] verts;
    delete[] indices;

    // メッシュの初期化
    mesh.Initialize(textureIDs, vertexArray, "DefaultShader", header.mRadius, header.mSpecPower);

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
    if (!data.LoadJSON(fileName))
    {
        return false;
    }
    JsonObject rootObj = data.GetObject();
    return LoadSkeletonJson(&rootObj, skeleton);
}

bool LoaderModel::LoadSkeletonBinary(const std::string &fileName, Skeleton &skeleton)
{
    std::ifstream file(fileName, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    // ヘッダーの読み込み
    SkeletonBinary::SkeletonBinHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    // シグネチャの確認
    char* sig = header.mSignature;
    if (sig[0] != 'G' || sig[1] != 'S' || sig[2] != 'K' || sig[3] != 'L')
    {
        return false;
    }

    // ボーンデータの読み込み
    std::vector<Skeleton::Bone> bones(header.mNumBones);
    for(size_t i = 0; i < header.mNumBones; i++)
    {
        // 名前の読み込み
        uint16_t nameSize = 0;
        file.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        char* name = new char[nameSize];
        file.read(name, nameSize);
        bones[i].Name = name;
        delete[] name;

        // 親の読み込み
        file.read(reinterpret_cast<char*>(&bones[i].Parent), sizeof(bones[i].Parent));

        // ローカルバインドポーズの読み込み
        Quaternion rot;
        file.read(reinterpret_cast<char*>(&rot), sizeof(rot));
        bones[i].LocalBindPose.mRotation = rot;

        Vector3 trans;
        file.read(reinterpret_cast<char*>(&trans), sizeof(trans));
        bones[i].LocalBindPose.mTranslation = trans;
    }

    // 親と子の対応
    std::unordered_map<int, std::vector<int>> boneChildrenIndex;
    for (size_t i = 0; i < bones.size(); i++)
    {
        boneChildrenIndex[bones[i].Parent].push_back(static_cast<int>(i));
    }

    return skeleton.Initialize(bones, boneChildrenIndex);
}