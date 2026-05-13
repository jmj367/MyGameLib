#include "Skeleton.h"
#include "MatrixPalette.h"
#include "JsonData.h"
#include <rapidjson/document.h>

bool Skeleton::Load(const std::string& fileName)
{
	mFileName = fileName;

	// Json読込
	JsonData data;
	if (!JsonData::LoadJSON(fileName, data))
	{
		//SDL_Log("Failed to load skeleton %s", fileName.c_str());
		return false;
	}

	// ボーン数の取得
	const JsonObject root = data.GetObject();
	int boneCount;
	if (!root.Get("bonecount", boneCount))
	{
		//SDL_Log("Skeleton %s doesn't have bone count.", fileName.c_str());
		return false;
	}

	size_t count = static_cast<size_t>(boneCount);
	if (count > MAX_SKELETON_BONES)
	{
		//SDL_Log("Skeleton %s exceeds maximum bone count.", fileName.c_str());
		return false;
	}

	mBones.reserve(count);

	JsonArray bones;
	if (!root.Get("bones", bones))
	{
		//SDL_Log("Skeleton %s doesn't have a bone array.", fileName.c_str());
		return false;
	}
	if(bones.Size() != count)
	{
		//SDL_Log("Skeleton %s has a mismatch between the bone count and number of bones.", fileName.c_str());
		return false;
	}

	// ボーンをvectorに格納
	Bone temp;

	for (int i = 0; i < count; i++)
	{
		JsonObject bone;
		if (!bones.Get(i, bone))
		{
			//SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
			return false;
		}

		std::string name;
		if (!bone.Get("name", name))
		{
			//SDL_Log("Skeleton %s: Bone %d doesn't have a name.", fileName.c_str(), i);
			return false;
		}
		temp.Name = name;

		int parent;
		if (!bone.Get("parent", parent))
		{
			//SDL_Log("Skeleton %s: Bone %d doesn't have a parent.", fileName.c_str(), i);
			return false;
		}
		temp.Parent = parent;

		JsonObject bindpose;
		if (!bone.Get("bindpose", bindpose))
		{
			//SDL_Log("Skeleton %s: Bone %d doesn't have a bind pose.", fileName.c_str(), i);
			return false;
		}

		// const rapidjson::Value& rot = bindpose["rot"];
		// const rapidjson::Value& trans = bindpose["trans"];
		// if (!rot.IsArray() || !trans.IsArray())
		// {
		// 	//SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
		// 	return false;
		// }
		JsonArray rot;
		if (!bindpose.Get("rot", rot))
		{
			//SDL_Log("Skeleton %s: Bone %d doesn't have a rotation in bind pose.", fileName.c_str(), i);
			return false;
		}

		float rotf[4];
		for (int j = 0; j < 4; j++)
		{
			if (rot.Get(j, rotf[j]))
			{
				//SDL_Log("Skeleton %s: Bone %d has an invalid rotation in bind pose.", fileName.c_str(), i);
				return false;
			}
		}
		temp.LocalBindPose.mRotation.Set(rotf[0], rotf[1], rotf[2], rotf[3]);

		JsonArray trans;
		if (!bindpose.Get("trans", trans))
		{
			//SDL_Log("Skeleton %s: Bone %d doesn't have a translation in bind pose.", fileName.c_str(), i);
			return false;
		}

		float transf[3];
		for (int j = 0; j < 3; j++)
		{
			if (trans.Get(j, transf[j]))
			{
				//SDL_Log("Skeleton %s: Bone %d has an invalid translation in bind pose.", fileName.c_str(), i);
				return false;
			}
		}
		temp.LocalBindPose.mTranslation.Set(transf[0], transf[1], transf[2]);

		mBones.emplace_back(temp);

		// 親と子の対応
		mBoneChildrenIndex[temp.Parent].emplace_back(i);
	}

	// 逆行列を求める
	ComputeGlobalInvBindPose();

	return true;
}

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