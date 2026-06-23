#pragma once

#include "Define.h"
#include "EventBus.h"
#include "InputSystem.h"
#include "Math.h"
#include "Renderer.h"
#include <chrono>
#include <cstdint>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

class Game
{
public:
	Game();

	// 初期化・ゲームループ・終了処理
	bool Initialize(WindowBackendType windowBackend, GraphicsAPI graphicsAPI);
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

	// シーンのロードの予約
	struct PendingSceneLoad
	{
		std::string SceneName;
		bool IsActive; // ロード後すぐにアクティブにするか
	};
	void ReserveSceneLoad(const PendingSceneLoad &scene);

	// シーンをアクティブにする予約
	void ReserveSceneActivation(const std::string &sceneName);

	// フォント取得
	// class Font *GetFont(const std::string &fileName);

	// アクセサ
	Renderer *GetRenderer() { return &mRenderer; }
	// AudioSystem *GetAudioSystem() { return &mAudioSystem; }
	// PhysWorld *GetPhysWorld() { return &mPhysWorld; }
	InputSystem *GetInputSystem() { return &mInputSystem; }
	// EffectSystem *GetEffectSystem() { return &mEffectSystem; }
	EventBus *GetEventBus() { return &mEventBus; }

	// その他ゲッター
	bool GetIsDebug() const { return mIsDebug; }

private:
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = Clock::time_point;

	// ゲームループヘルパー
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void InnactivateScenes();
	void UnloadScenes();
	void LoadScenes();
	void ActivateScenes();

	// 現在のゲーム状態
	GameState mGameState;

	// ウィンドウクラス
	std::unique_ptr<class Window> mWindow;

	// ロードされたシーンのリスト
	std::unordered_map<std::string, std::unique_ptr<class Scene>> mScenes;
	// 現在アクティブなシーン
	// 先頭から順に更新・描画される想定
	std::vector<class Scene *> mActiveScenes;
	// ロード予約リスト
	std::vector<PendingSceneLoad> mPendingSceneLoads;
	// シーンのアクティブ予約リスト
	std::vector<std::string> mReservedSceneActivations;

	// フォントのキャッシュ
	// std::unordered_map<std::string, class Font *> mFonts;

	// ユーティリティクラス
	Renderer mRenderer;
	// AudioSystem *mAudioSystem;
	// PhysWorld *mPhysWorld;
	InputSystem mInputSystem;
	// EffectSystem *mEffectSystem; // レンダリングに関わるのでRendererに組み込んだ方が良いかもしれない
	EventBus mEventBus;

	// deltaTimeを求めるのに使う
	TimePoint mLastFrameTime;
	// デバッグフラグ
	bool mIsDebug;
};
