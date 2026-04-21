// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <random>
#include "Math.h"

class Random
{
public:
	static void Init();

	// シード値の設定
	static void Seed(unsigned int seed);

	// float値0.0 ～ 1.0を返す
	static float GetFloat();

	// 範囲を指定して取得
	static float	GetFloatRange	(float			min, float			max);
	static int		GetIntRange		(int			min, int			max);
	static Vector2	GetVector		(const Vector2&	min, const Vector2& max);
	static Vector3	GetVector		(const Vector3&	min, const Vector3& max);
private:
	static std::mt19937 sGenerator;
};