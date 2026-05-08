// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Renderer.h"
#include <string>

class Texture
{
public:
	Texture();
	~Texture();

	// 読込・解放
	virtual bool Load(const std::string &fileName);
	virtual void Unload();

	// レンダリング用テクスチャの作成
	// サブカメラ等用
	void CreateForRendering(int width, int height, unsigned int format);

	// ゲッター
	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }
	const std::string &GetFileName() const { return mFileName; }

private:
	// ファイル名
	std::string mFileName;

	// テクスチャの幅・高さ
	int mWidth;
	int mHeight;
};
