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
	bool Load(const std::string &fileName);
	void Unload();

	// SDL_Surfaceから作成
	// テキストの描画等で使用
	void CreateFromSurface(struct SDL_Surface *surface);

	// レンダリング用テクスチャの作成
	// サブカメラ等用
	void CreateForRendering(int width, int height, unsigned int format);

	void SetActive(int index = 0);

	// ゲッター
	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }
	unsigned int GetTextureID() const { return mTextureID; }
	const std::string &GetFileName() const { return mFileName; }

private:
	// ファイル名
	std::string mFileName;

	// テクスチャID
	unsigned int mTextureID;

	// テクスチャの幅・高さ
	int mWidth;
	int mHeight;
};
