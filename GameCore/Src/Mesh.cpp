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
		float mRadius = 0.0f;
		float mSpecPower = 100.0f;
	};
}

Mesh::Mesh()
	: mVertexArray()
	, mRadius(0.0f)
	, mSpecPower(100.0f)
{
}

Mesh::~Mesh()
{
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
	float radius,
	float specPower)
{
	// ヘッダー情報の作成
	MeshBinHeader header;
	header.mLayout = layout;
	header.mNumTextures = static_cast<unsigned>(textureNames.size());
	header.mNumVerts = numVerts;
	header.mNumIndices = numIndices;
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