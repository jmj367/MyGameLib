#pragma once

#include "EventBus.h"
#include "InputSystem.h"
#include "Math.h"
#include "Renderer.h"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

class Game
{
public:
	Game();

	// 初期化・ゲームループ・終了処理
	bool Initialize();
	void RunLoop();
	void Shutdown();

	// ゲーム状態関連
	enum GameState
	{
		EGameplay,
		EPaused,
		EQuit
	};
	GameState GetState() const { return mGameState; }
	void SetState(GameState state) { mGameState = state; }

	// シーン関連
	class Scene *GetScene() { return mCurrentScene; }

	// シーンの変更を予約する
	// 現在のシーンから呼ばれる
	// 更新は現在のシーンの更新が終わった後
	// fileNameはシーンのロードに用いる
	void ReserveChangeScene(class Scene *scene, const std::string &fileName = "");

	// フォント取得
	class Font *GetFont(const std::string &fileName);

	// アクセサ
	Renderer *GetRenderer() { return &mRenderer; }
	//AudioSystem *GetAudioSystem() { return &mAudioSystem; }
	//PhysWorld *GetPhysWorld() { return &mPhysWorld; }
	InputSystem *GetInputSystem() { return &mInputSystem; }
	//EffectSystem *GetEffectSystem() { return &mEffectSystem; }
	EventBus *GetEventBus() { return &mEventBus; }

	// その他ゲッター
	bool GetIsDebug() const { return mIsDebug; }

private:
	// ゲームループヘルパー
	void ProcessInput();
	void GameInput(const struct InputState &state);
	void UpdateGame();
	void GenerateOutput();
	void ChangeCurrentScene();

	// ロード・アンロード
	void LoadData();
	void UnloadData();

	// 現在のゲーム状態
	GameState mGameState;

	// 現在のシーン
	class Scene *mCurrentScene;
	// 次のシーン
	// null参照を回避するためにこうする
	class Scene *mPendingScene;
	bool mIsChangeScene;
	std::string mPendingSceneFileName;

	// フォントのキャッシュ
	std::unordered_map<std::string, class Font *> mFonts;

	// ユーティリティクラス
	Renderer mRenderer;
	//AudioSystem *mAudioSystem;
	//PhysWorld *mPhysWorld;
	InputSystem mInputSystem;
	//EffectSystem *mEffectSystem; // レンダリングに関わるのでRendererに組み込んだ方が良いかもしれない
	EventBus mEventBus;

	// チックカウント
	// deltaTimeを求めるのに使う
	uint32_t mTicksCount;

	// デバッグフラグ
	bool mIsDebug;
};
