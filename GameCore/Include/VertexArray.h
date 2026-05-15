// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

class VertexArray
{
public:
	// 頂点レイアウトの種類
	enum Layout
	{
		PosNormTex,
		PosNormSkinTex
	};

	VertexArray() = default;
	VertexArray(const void* verts, unsigned int numVerts, Layout layout,
		const unsigned int* indices, unsigned int numIndices);
	~VertexArray();

	void SetActive();
	unsigned int GetNumIndices() const { return mNumIndices; }
	unsigned int GetNumVerts() const { return mNumVerts; }

	static unsigned int GetVertexSize(VertexArray::Layout layout);
private:
	// いくつの頂点があるか
	unsigned int mNumVerts;
	// いくつのインデックスがあるか
	unsigned int mNumIndices;
	// OpenGLの頂点バッファのID
	unsigned int mVertexBuffer;
	// OpenGLのインデックスバッファのID
	unsigned int mIndexBuffer;
	// OpenGLの頂点配列オブジェクトのID
	unsigned int mVertexArray;
};