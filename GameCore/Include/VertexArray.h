// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <string>

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
	VertexArray(const void *verts, unsigned int numVerts, Layout layout,
				const unsigned int *indices, unsigned int numIndices);
	~VertexArray();

	void *GetVertexBuffer	() const;
	void *GetIndexBuffer	() const;

	void SetActive();

	unsigned int GetNumIndices	() const { return mNumIndices	; }
	unsigned int GetNumVerts	() const { return mNumVerts		; }
	Layout GetLayout() const { return mLayout; }
	std::string GetLayoutName() const { return mLayout == PosNormTex ? "PosNormTex" : "PosNormSkinTex"; }
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
	// 頂点レイアウト
	Layout mLayout;
};