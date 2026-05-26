#pragma once

#include "Define.h"
#include <vector>

class GBuffer
{
public:
	// テクスチャの種類
	enum Type
	{
		EDiffuse = 0,
		ENormal,
		EWorldPos,
		NUM_GBUFFER_TEXTURES
	};

	GBuffer();
	~GBuffer();

	// 生成・破棄
	bool Create(int width, int height);
	void Destroy();
	
	// 指定した種類のテクスチャを取得
	ResourceID GetTexture(Type type);

	// ゲッター
	unsigned int GetBufferID() const { return mBufferID; }
private:
	// Gバッファ用テクスチャ群
	std::vector<ResourceID> mTextures;
	// バッファID
	unsigned int mBufferID;
};
