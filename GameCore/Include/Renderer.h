#pragma once

#include "Define.h"
#include "Math.h"
#include "MatrixPalette.h"
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

// 前方宣言
class RendererBackend;

class Renderer
{
public:
	// APIの種類
	enum class GraphicsAPI
	{
		OpenGL,
	};

	Renderer(class Game *game);
	virtual ~Renderer();

	virtual bool PrepareWindow(); // ウィンドウ作成前の初期化処理(必要なら)
	virtual bool Initialize(void* windowHandle, float screenWidth, float screenHeight, GraphicsAPI apiType = GraphicsAPI::OpenGL);
	virtual void Shutdown();
	virtual void Draw();

	// 各種描画コマンドの構造体と送信関数
	// RendererBackendをラップする

	// スプライト描画の為の構造体
	struct SpriteDrawInfo
	{
		size_t 		TextureID;
		Vector2 	TextureSize;
		Matrix4 	WorldTransform;
		ResourceID 	ShaderID;
		int 		DrawOrder;
	};
	// スプライトの描画コマンドを送信
	void DrawSprite(const SpriteDrawInfo &spriteInfo);

	// メッシュ描画の為の構造体
	struct MeshDrawInfo
	{
		ResourceID 	MeshID;
		ResourceID 	TextureID;
		Matrix4 	WorldTransform;
		ResourceID 	ShaderID;
	};
	// メッシュの描画コマンドを送信
	void DrawMesh(const MeshDrawInfo &meshInfo);

	// スキンメッシュ描画の為の構造体
	struct SkinnedMeshDrawInfo
	{
		ResourceID 		MeshID;
		ResourceID 		TextureID;
		MatrixPalette 	MatrixPalette;
		Matrix4 		WorldTransform;
		ResourceID 		ShaderID;
	};
	// スキンメッシュの描画コマンドを送信
	void DrawSkinnedMesh(const SkinnedMeshDrawInfo &skinnedMeshInfo);

	// ポイントライトの構造体
	struct PointLightDrawInfo
	{
		Vector3 Position;
		float 	Radius;
		Vector3 Color;
		float 	Intensity;
	};
	// ポイントライトの描画コマンドを送信
	void DrawPointLight(const PointLightDrawInfo &pointLight);

	// スポットライトの構造体
	// InnerAngleとOuterAngleはラジアンで、光量最大の範囲と減衰する範囲を表す
	struct SpotLightDrawInfo
	{
		Vector3 Position;
		float 	Range;
		Vector3 Color;
		float 	Intensity;
		Vector3 Direction;
		float 	InnerAngle;
		float 	OuterAngle;
	};
	// スポットライトの描画コマンドを送信
	void DrawSpotLight(const SpotLightDrawInfo &spotLight);

	// ディレクショナルライトの構造体
	struct DirectionalLightDrawInfo
	{
		Vector3 Direction;
		Vector3 DiffuseColor;
		Vector3 SpecularColor;
	};
	// ディレクショナルライトの描画コマンドを送信
	void DrawDirectionalLight(const DirectionalLightDrawInfo &directionalLight);

	// 環境光の構造体(Vector3直接でもいいが拡張性を残しておく)
	struct AmbientLightDrawInfo
	{
		Vector3 Color;
	};
	// 環境光の描画コマンドを送信
	void DrawAmbientLight(const AmbientLightDrawInfo &ambientLight);

	// ポストプロセスの構造体(一応構造体にしておく)
	struct PostProcessDrawInfo
	{
		ResourceID ShaderID;
	};
	// ポストプロセスの描画コマンドを送信
	void DrawPostProcess(const PostProcessDrawInfo &postProcessInfo);

	// リソースの取得/解放
	bool GetTexture		(const std::string &fileName, ResourceID& outID);
	bool GetMesh		(const std::string &fileName, ResourceID& outID);
	bool GetSkeleton	(const std::string &fileName, ResourceID& outID);
	bool GetShader		(const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName, ResourceID& outID);
	void ReleaseTexture	(ResourceID textureID	);
	void ReleaseMesh	(ResourceID meshID		);
	void ReleaseSkeleton(ResourceID skeletonID	);
	void ReleaseShader	(ResourceID shaderID	);
	void ReleaseTexture	(const std::string &fileName);
	void ReleaseMesh	(const std::string &fileName);
	void ReleaseSkeleton(const std::string &fileName);
	void ReleaseShader	(const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName);
	void ReleaseAllResources();

	// リソースIDからリソース名を取得
	bool GetTexture	(ResourceID textureID	, std::string &outFileName);
	bool GetMesh	(ResourceID meshID		, std::string &outFileName);
	bool GetSkeleton(ResourceID skeletonID	, std::string &outFileName);
	bool GetShader	(ResourceID shaderID	, std::string &outVertexShaderFileName, std::string &outFragmentShaderFileName);

	// スクリーンサイズのゲッター
	float GetScreenWidth	() const { return mScreenWidth	; }
	float GetScreenHeight	() const { return mScreenHeight	; }
	Vector2 GetScreenSize	() const { return Vector2(mScreenWidth, mScreenHeight); }

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

	// リソース検索用マップ
	std::unordered_map<std::string, ResourceID> mTextureFileNameToID	;
	std::unordered_map<std::string, ResourceID> mMeshFileNameToID		;
	std::unordered_map<std::string, ResourceID> mSkeletonFileNameToID	;
	std::unordered_map<std::pair<std::string, std::string>, ResourceID> mShaderFileNameToID;

	// バックエンドのインスタンス
	std::unique_ptr<RendererBackend> mBackend;

	// 描画コマンド
	std::vector<SpriteDrawInfo> 			mSpriteDrawList				;
	std::vector<MeshDrawInfo> 				mMeshDrawList				;
	std::vector<SkinnedMeshDrawInfo> 		mSkinnedMeshDrawList		;
	std::vector<PointLightDrawInfo> 		mPointLightDrawList			;
	std::vector<SpotLightDrawInfo> 			mSpotLightDrawList			;
	// ディレクショナルライトと環境光は数が少ないことが想定されるが、一応描画コマンドのリストにしておく
	std::vector<DirectionalLightDrawInfo> 	mDirectionalLightDrawList	;
	std::vector<AmbientLightDrawInfo> 		mAmbientLightDrawList		;
	std::vector<PostProcessDrawInfo> 		mPostProcessDrawList		;

	// 描画コマンドのロック
	std::mutex mSpriteDrawListMutex				;
	std::mutex mMeshDrawListMutex				;
	std::mutex mSkinnedMeshDrawListMutex		;
	std::mutex mPointLightDrawListMutex			;
	std::mutex mSpotLightDrawListMutex			;
	std::mutex mDirectionalLightDrawListMutex	;
	std::mutex mAmbientLightDrawListMutex		;
	std::mutex mPostProcessDrawListMutex		;
};