// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Math.h"
#include <string>

class Shader
{
public:
	Shader();
	~Shader();

	// 読込・解放
	bool Load(const std::string &vertName, const std::string &fragName);
	void Unload();

	// アクティブ化
	void SetActive();

	// 行列の引き渡し
	void SetMatrixUniform(const char *name, const Matrix4 &matrix);
	void SetMatrixUniforms(const char *name, Matrix4 *matrices, unsigned count);
	// ベクトルの引き渡し
	void SetVector3Uniform(const char *name, const Vector3 &vector);
	void SetVector2Uniform(const char *name, const Vector2 &vector);
	// 小数の引き渡し
	void SetFloatUniform(const char *name, float value);
	// 整数の引き渡し
	void SetIntUniform(const char *name, int value);

	// ゲッター
	const std::string &GetVertexShaderFileName() const { return mVertexShaderFileName; }
	const std::string &GetFragmentShaderFileName() const { return mFragmentShaderFileName; }

private:
	// シェーダーのコンパイル
	bool CompileShader(const std::string &fileName,
					   unsigned int shaderType,
					   unsigned int &outShader);

	// コンパイル済みか
	bool IsCompiled(unsigned int shader);
	// プログラムが有効か
	bool IsValidProgram();

private:
	// シェーダーのファイル名
	std::string mVertexShaderFileName;
	std::string mFragmentShaderFileName;

	// シェーダーオブジェクト
	unsigned int mVertexShader;
	unsigned int mFragShader;
	unsigned int mShaderProgram;
};
