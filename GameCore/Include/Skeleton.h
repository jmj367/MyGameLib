#pragma once
#include "BoneTransform.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace SkeletonBinary
{
	union BoneIndex
	{
		uint32_t i;
		uint8_t b[4];
	};

	const int BinaryVersion = 1;

	struct SkeletonBinHeader
	{
		// シグネチャ
		char mSignature[4] = {'G', 'S', 'K', 'L'};

		// バージョン
		uint32_t mVersion = BinaryVersion;

		// ボーン数
		uint32_t mNumBones = 0;
	};
}

class Skeleton
{
public:
	// 各ボーンの情報を定義
	struct Bone
	{
		BoneTransform	LocalBindPose;
		std::string		Name;
		int				Parent;
	};

	// ルートボーンのインデックスは0
	// 0の親は-1とする
	static const int ROOT_BONE			= 0;
	static const int ROOT_BONE_PARENT	= -1;

	bool Initialize(const std::vector<Bone>& bones, const std::unordered_map<int, std::vector<int>>& boneChildrenIndex)
	{
		mBones = bones;
		mBoneChildrenIndex = boneChildrenIndex;
		ComputeGlobalInvBindPose();
		return true;
	}

	// その他ゲッター
	size_t						GetNumBones()			const { return mBones.size(); }
	const Bone&					GetBone(size_t idx)		const { return mBones[idx]; }
	const std::vector<Bone>&	GetBones()				const { return mBones; }
	const std::unordered_map<int, std::vector<int>>& 
								GetBoneChildrenIndex()	const { return mBoneChildrenIndex; }
	const std::vector<Matrix4>& GetGlobalInvBindPoses() const { return mGlobalInvBindPoses; }
	const std::string&			GetFileName()			const { return mFileName; }
private:
	// スケルトンロード時に実行される
	void ComputeGlobalInvBindPose();
	// ヘルパー
	void ComputeEachBoneBindPose(int boneIdx, bool isRoot);

	// ボーン
	std::vector<Bone> mBones;

	// NOTE: ボーンインデックスの情報がアニメーション等多数ファイルに渡って使われるためソート不可
	// 親ボーンと子ボーンの対応
	std::unordered_map<int, std::vector<int>> mBoneChildrenIndex;

	// 各ボーンのバインドポーズの逆行列
	std::vector<Matrix4> mGlobalInvBindPoses;

	std::string mFileName;
};