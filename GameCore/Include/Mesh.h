// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "VertexArray.h"
#include <cstdint>
#include <vector>
#include <string>

class Mesh
{
public:
	using ResourceID = size_t;

	Mesh();
	~Mesh();

	void Initialize(
		const std::vector<ResourceID> &textures,
		VertexArray &vertexArray,
		const std::string &shaderName,
		float radius, float specPower)
	{
		mTextures = textures;
		mVertexArray = vertexArray;
		mShaderName = shaderName;
		mRadius = radius;
		mSpecPower = specPower;
	}

	// インデックスのテクスチャを取得
	ResourceID GetTexture(size_t index);

	// バイナリ形式でメッシュを保存
	void SaveBinary(
		const std::string &fileName,
		const void *verts,
		uint32_t numVerts,
		VertexArray::Layout layout,
		const uint32_t *indices,
		uint32_t numIndices,
		const std::vector<std::string> &textureNames,
		float radius,
		float specPower);

	// ゲッター
	VertexArray *GetVertexArray() { return &mVertexArray; }
	const std::string &GetShaderName() const { return mShaderName; }
	const std::string &GetFileName() const { return mFileName; }
	float GetRadius() const { return mRadius; }
	float GetSpecPower() const { return mSpecPower; }

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