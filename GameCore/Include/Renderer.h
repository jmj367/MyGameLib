#pragma once

#include "Math.h"
#include "MatrixPalette.h"
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>

class Renderer
{
public:
	using ResourceID = size_t;

	Renderer(class Game *game);
	virtual ~Renderer();

	virtual bool Initialize(float screenWidth, float screenHeight);
	virtual void Shutdown();
	virtual void Draw();

	// スプライト描画の為の構造体
	struct SpriteDrawInfo
	{
		size_t TextureID;
		Vector2 TextureSize;
		Matrix4 WorldTransform;
		ResourceID ShaderID;
		int DrawOrder;
	};
	// スプライトの描画コマンドを送信
	void DrawSprite(const SpriteDrawInfo &spriteInfo);

	// メッシュ描画の為の構造体
	struct MeshDrawInfo
	{
		ResourceID MeshID;
		ResourceID TextureID;
		Matrix4 WorldTransform;
		ResourceID ShaderID;
	};
	// メッシュの描画コマンドを送信
	void DrawMesh(const MeshDrawInfo &meshInfo);

	// スキンメッシュ描画の為の構造体
	struct SkinnedMeshDrawInfo
	{
		ResourceID MeshID;
		ResourceID TextureID;
		MatrixPalette MatrixPalette;
		Matrix4 WorldTransform;
		ResourceID ShaderID;
	};
	// スキンメッシュの描画コマンドを送信
	void DrawSkinnedMesh(const SkinnedMeshDrawInfo &skinnedMeshInfo);

	// リソースの取得/解放
	ResourceID GetTexture(const std::string &fileName);
	ResourceID GetMesh(const std::string &fileName);
	ResourceID GetSkeleton(const std::string &fileName);
	ResourceID GetShader(const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName);
	void ReleaseTexture(ResourceID textureID);
	void ReleaseMesh(ResourceID meshID);
	void ReleaseSkeleton(ResourceID skeletonID);
	void ReleaseShader(ResourceID shaderID);
	void ReleaseTexture(const std::string &fileName);
	void ReleaseMesh(const std::string &fileName);
	void ReleaseSkeleton(const std::string &fileName);
	void ReleaseShader(const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName);
	void ReleaseAllResources();

	// スクリーンサイズのゲッター
	float GetScreenWidth() const { return mScreenWidth; }
	float GetScreenHeight() const { return mScreenHeight; }
	Vector2 GetScreenSize() const { return Vector2(mScreenWidth, mScreenHeight); }

	// 透視投影行列
	const Matrix4 &GetViewMatrix() const { return mViewMat; }
	const Matrix4 &GetProjMatrix() const { return mProjMat; }
	void SetViewMatrix(const Matrix4 &viewMat) { mViewMat = viewMat; }
	void SetProjMatrix(const Matrix4 &projMat) { mProjMat = projMat; }

private:
	// ゲームクラスへのポインタ
	class Game *mGame;

	// スクリーンサイズ
	float mScreenWidth;
	float mScreenHeight;

	// 透視投影行列
	Matrix4 mViewMat;
	Matrix4 mProjMat;

	// リソースIDの管理
	ResourceID mNextResourceID;

	// リソースのキャッシュ
	std::unordered_map<ResourceID, class Texture> mTextures;
	std::unordered_map<ResourceID, class Mesh> mMeshes;
	std::unordered_map<ResourceID, class Skeleton> mSkeletons;
	std::unordered_map<ResourceID, class Shader> mShaders;
	// リソース検索用マップ
	std::unordered_map<std::string, ResourceID> mTextureFileNameToID;
	std::unordered_map<std::string, ResourceID> mMeshFileNameToID;
	std::unordered_map<std::string, ResourceID> mSkeletonFileNameToID;
	std::unordered_map<std::pair<std::string, std::string>, ResourceID> mShaderFileNameToID;

	// 描画コマンド
	std::vector<SpriteDrawInfo> mSpriteDrawList;
	std::vector<MeshDrawInfo> mMeshDrawList;
	std::vector<SkinnedMeshDrawInfo> mSkinnedMeshDrawList;
	// 描画コマンドのロック
	std::mutex mSpriteDrawListMutex;
	std::mutex mMeshDrawListMutex;
	std::mutex mSkinnedMeshDrawListMutex;
};