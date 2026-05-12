// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Math.h"

class BoneTransform
{
public:
	Quaternion mRotation;
	Vector3 mTranslation;

	// プロパティ行列を取得
	Matrix4 ToMatrix() const;

	// 補間
	static BoneTransform Interpolate(const BoneTransform& a, const BoneTransform& b, float f);
};
