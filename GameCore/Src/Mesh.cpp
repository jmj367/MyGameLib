// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Mesh.h"
#include "JsonData.h"
#include "Math.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include <rapidjson/document.h>
#include <SDL2/SDL_log.h>
#include <fstream>

// バイナリ形式用の構造体等
namespace
{
	union Vertex
	{
		float f;
		uint8_t b[4];
	};

	const int BinaryVersion = 1;

	struct MeshBinHeader
	{
		// シグネチャ
		char mSignature[4] = {'G', 'M', 'S', 'H'};

		// バージョン
		uint32_t mVersion = BinaryVersion;

		// 頂点フォーマット
		VertexArray::Layout mLayout = VertexArray::PosNormTex;

		// テクスチャ数
		uint32_t mNumTextures = 0;
		uint32_t mNumVerts = 0;
		uint32_t mNumIndices = 0;

		// プロパティ
		AABB mBox{Vector3::Zero, Vector3::Zero};
		float mRadius = 0.0f;
		float mSpecPower = 100.0f;
	};
}

Mesh::Mesh()
	: mVertexArray(nullptr)
	, mRadius(0.0f)
	, mSpecPower(100.0f)
{
}

Mesh::~Mesh()
{
}

bool Mesh::Load(const std::string &fileName, Renderer *renderer)
{
	mFileName = fileName;

	// バイナリ形式で読み込みを試みる
	if (LoadBinary(fileName + ".bin", renderer))
	{
		return true;
	}

	// JSON形式で読み込む
	JsonData data;
	if(!JsonData::LoadJSON(fileName, data))
	{
		SDL_Log("Failed to load mesh %s", fileName.c_str());
		return false;
	}

	//mShaderName = doc["shader"].GetString();

	JsonObject root = data.GetObject();
	std::string shaderName;
	if (!root.Get("shader", shaderName))
	{		
		return false;
	}
	mShaderName = shaderName;

	VertexArray::Layout layout = VertexArray::PosNormTex;
	size_t vertSize = 8;

	std::string vertexFormat;
	if(!root.Get("vertexformat", vertexFormat))
	{
		return false;
	}
	if (vertexFormat == "PosNormSkinTex")
	{
		layout = VertexArray::PosNormSkinTex;
		vertSize = 10;
	}

	// テクスチャの読み込み
	JsonArray textures;
	if (!root.Get("textures", textures) || textures.Size() < 1)
	{
		return false;
	}

	if(!root.Get("specularPower", mSpecPower))
	{
		return false;
	}

	std::vector<std::string> textureNames;

	for(int i = 0; i < textures.Size(); i++)
	{
		// テクスチャが存在するか確認
		std::string texName;
		if(!textures.Get(i, texName))
		{
			return false;
		}
		textureNames.emplace_back(texName);
		ResourceID t;
		if(!renderer->GetTexture(texName, t))
		{
			return false;
		}
		mTextures.emplace_back(t);
	}

	// 頂点の読み込み
	JsonArray vertsJson;
	if (!root.Get("vertices", vertsJson) || vertsJson.Size() < 1)
	{
		return false;
	}

	std::vector<Vertex> vertices;
	vertices.reserve(vertsJson.Size() * vertSize);
	mRadius = 0.0f;

	for(int i = 0; i < vertsJson.Size(); i++)
	{
		// 各頂点の読み込み
		JsonArray vert;
		if(!vertsJson.Get(i, vert))
		{
			return false;
		}

		Vector3 pos;
		if(!vert.Get(0, pos.x) || !vert.Get(1, pos.y) || !vert.Get(2, pos.z))
		{
			return false;
		}
		
		mRadius = Math::Max(mRadius, pos.LengthSq());
		
		if(layout == VertexArray::PosNormTex)
		{
			Vertex v;
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
		else
		{
			Vertex v;
			// 位置・法線を追加
			for(int j = 0; j < 6; j++)
			{
				if(!vert.Get(j, v.f))
				{
					return false;
				}
				vertices.emplace_back(v);
			}

			// スキニング情報を追加
			for(int j = 6; j < 14; j += 4)
			{
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
	mRadius = Math::Sqrt(mRadius);

	// インデックスの読み込み
	// const rapidjson::Value &indJson = doc["indices"];
	// if (!indJson.IsArray() || indJson.Size() < 1)
	// {
	// 	SDL_Log("Mesh %s has no indices", fileName.c_str());
	// 	return false;
	// }
	JsonArray indJson;
	if (!root.Get("indices", indJson) || indJson.Size() < 1)
	{
		return false;
	}

	std::vector<unsigned int> indices;
	indices.reserve(indJson.Size() * 3);
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

	// 頂点配列オブジェクトの作成
	// unsigned int numVerts = static_cast<unsigned>(vertices.size()) / vertSize;
	// mVertexArray = new VertexArray(vertices.data(), numVerts,
	// 							   layout, indices.data(), static_cast<unsigned>(indices.size()));

	// バイナリ形式で保存
	// SaveBinary(
	// 	fileName + ".bin",
	// 	vertices.data(),
	// 	numVerts, layout,
	// 	indices.data(),
	// 	static_cast<unsigned>(indices.size()),
	// 	textureNames,
	// 	mBox,
	// 	mRadius,
	// 	mSpecPower);
	return true;
}

void Mesh::Unload()
{
	delete mVertexArray;
	mVertexArray = nullptr;
}

Mesh::ResourceID Mesh::GetTexture(size_t index)
{
	if (index < mTextures.size())
	{
		return mTextures[index];
	}
	
	return ResourceID(-1);
}

void Mesh::SaveBinary(
	const std::string &fileName,
	const void *verts,
	uint32_t numVerts,
	VertexArray::Layout layout,
	const uint32_t *indices,
	uint32_t numIndices,
	const std::vector<std::string> &textureNames,
	const AABB &box,
	float radius,
	float specPower)
{
	// ヘッダー情報の作成
	MeshBinHeader header;
	header.mLayout = layout;
	header.mNumTextures = static_cast<unsigned>(textureNames.size());
	header.mNumVerts = numVerts;
	header.mNumIndices = numIndices;
	header.mBox = box;
	header.mRadius = radius;

	// セーブファイルの作成
	std::ofstream outFile(fileName, std::ios::out | std::ios::binary);
	if (outFile.is_open())
	{
		// ヘッダー情報の書き込み
		outFile.write(reinterpret_cast<char *>(&header), sizeof(header));

		// テクスチャファイル名の書き込み
		for (const auto &tex : textureNames)
		{
			// ファイル名のサイズを書き込み
			uint16_t nameSize = static_cast<uint16_t>(tex.length()) + 1;
			outFile.write(reinterpret_cast<char *>(&nameSize), sizeof(nameSize));
			outFile.write(tex.c_str(), nameSize - 1);
			outFile.write("\0", 1);
		}

		// 頂点サイズの取得
		unsigned vertexSize = VertexArray::GetVertexSize(layout);
		// 頂点の書き込み
		outFile.write(reinterpret_cast<const char *>(verts),
					  numVerts * vertexSize);
		// インデックスの書き込み
		outFile.write(reinterpret_cast<const char *>(indices),
					  numIndices * sizeof(uint32_t));
	}
}

bool Mesh::LoadBinary(const std::string &fileName, Renderer *renderer)
{
	std::ifstream inFile(fileName, std::ios::in |
									   std::ios::binary);
	if (inFile.is_open())
	{
		// ヘッダー情報の読み込み
		MeshBinHeader header;
		inFile.read(reinterpret_cast<char *>(&header), sizeof(header));

		// シグネチャとバージョンの確認
		char *sig = header.mSignature;
		if (sig[0] != 'G' || sig[1] != 'M' || sig[2] != 'S' ||
			sig[3] != 'H' || header.mVersion != BinaryVersion)
		{
			return false;
		}

		// テクスチャ名の読み込み
		for (uint32_t i = 0; i < header.mNumTextures; i++)
		{
			// 名前のサイズを読む
			uint16_t nameSize = 0;
			inFile.read(reinterpret_cast<char *>(&nameSize), sizeof(nameSize));
			// 名前本体のためのメモリを確保
			char *texName = new char[nameSize];
			// 名前を読む
			inFile.read(texName, nameSize);

			// テクスチャを取得
			Texture *t = renderer->GetTexture(texName);
			if (t == nullptr)
			{
				// テクスチャが存在しない場合、デフォルトテクスチャを使用
				t = renderer->GetTexture(MODEL_FILE_NAMES::DEFAULT_TEXTURE);
			}
			mTextures.emplace_back(t);

			// メモリ解放
			delete[] texName;
		}

		// 頂点データの読み込み
		unsigned vertexSize = VertexArray::GetVertexSize(header.mLayout);
		char *verts = new char[header.mNumVerts * vertexSize];
		inFile.read(verts, header.mNumVerts * vertexSize);

		// インデックスデータの読み込み
		uint32_t *indices = new uint32_t[header.mNumIndices];
		inFile.read(reinterpret_cast<char *>(indices),
					header.mNumIndices * sizeof(uint32_t));

		// 頂点配列オブジェクトの作成
		mVertexArray = new VertexArray(verts, header.mNumVerts,
									   header.mLayout, indices, header.mNumIndices);

		// メモリ解放
		delete[] verts;
		delete[] indices;

		// プロパティの設定
		mBox = header.mBox;
		mRadius = header.mRadius;
		mSpecPower = header.mSpecPower;

		return true;
	}
	return false;
}
