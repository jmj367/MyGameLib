#pragma once
#include "VertexArray.h"
#include <cstdint>
#include <vector>
#include <string>

namespace MeshBinary
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
		uint32_t mNumTextures 	= 0;
		uint32_t mNumVerts 		= 0;
		uint32_t mNumIndices 	= 0;

		// プロパティ
		float mRadius 		= 0.0f;
		float mSpecPower 	= 100.0f;
	};
}

class Mesh
{
public:
	using ResourceID = size_t;

	Mesh();
	~Mesh();

	void Initialize(
		const 	std::vector<ResourceID> &textures,
				VertexArray 			&vertexArray,
		const 	std::string 			&shaderName,
		float 							radius, 
		float 							specPower)
	{
		mTextures 		= textures		;
		mVertexArray 	= vertexArray	;
		mShaderName 	= shaderName	;
		mRadius 		= radius		;
		mSpecPower 		= specPower		;
	}

	// ゲッター
	ResourceID 	GetTexture		(size_t index) 	const { return mTextures[index]; }
	size_t 		GetNumTextures	() 				const { return mTextures.size(); }

	VertexArray *GetVertexArray() { return &mVertexArray; }

	const std::string &GetShaderName() const { return mShaderName	; }
	const std::string &GetFileName	() const { return mFileName		; }

	float GetRadius		() const { return mRadius	; }
	float GetSpecPower	() const { return mSpecPower; }

private:
	// メッシュに関連付けられたテクスチャ群
	std::vector<ResourceID> mTextures;

	// 頂点配列オブジェクト
	VertexArray mVertexArray;
	// 使用するシェーダー名
	std::string mShaderName;
	// メッシュファイル名
	std::string mFileName;
	// メッシュの球半径
	float mRadius;
	// 鏡面反射の強さ
	float mSpecPower;
};