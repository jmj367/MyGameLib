#include "Skeleton.h"
#include "MatrixPalette.h"
#include "JsonData.h"
#include <rapidjson/document.h>

void Skeleton::ComputeGlobalInvBindPose()
{
	// 各ボーンの行列を求める
	mGlobalInvBindPoses.resize(GetNumBones());

	// ルートボーンは一つであることを前提とする
	auto iter = mBoneChildrenIndex.find(ROOT_BONE_PARENT);

	ComputeEachBoneBindPose((*iter).second[0], true);

	// 逆行列にする
	for (size_t i = 0; i < mGlobalInvBindPoses.size(); i++)
	{
		mGlobalInvBindPoses[i].Invert();
	}
}

void Skeleton::ComputeEachBoneBindPose(int boneIdx, bool isRoot)
{
	// ボーンの行列を求める
	// ルートボーンはそのまま
	// 子ボーンは親ボーンとの相対的な行列を求める
	if (isRoot)
	{
		mGlobalInvBindPoses[boneIdx] = mBones[boneIdx].LocalBindPose.ToMatrix();
	}
	else
	{
		Matrix4 localMat = mBones[boneIdx].LocalBindPose.ToMatrix();
		mGlobalInvBindPoses[boneIdx] = localMat * mGlobalInvBindPoses[mBones[boneIdx].Parent];
	}

	// 各ボーンでこの関数を実行する
	auto iter = mBoneChildrenIndex.find(boneIdx);
	if (iter != mBoneChildrenIndex.end())
	{
		for (auto childIdx : (*iter).second)
		{
			ComputeEachBoneBindPose(childIdx, false);
		}
	}
}