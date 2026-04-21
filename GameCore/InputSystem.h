#pragma once
#include "Commons/Math.h"
#include <string>
#include <SDL/SDL_scancode.h>
#include <SDL/SDL_gamecontroller.h>
#include <SDL/SDL_mouse.h>
#include <unordered_map>
#include <vector>

// キーボード入力のヘルパー
class KeyboardState
{
public:
	// 簡便のため
	friend class InputSystem;

	// キー状態の取得
	bool GetKeyPressed	(SDL_Scancode key) const;
	bool GetKeyHeld		(SDL_Scancode key) const;
	bool GetKeyReleased	(SDL_Scancode key) const;
private:
	// キー状態の保持
	const Uint8*	mCurrKeysState;
	Uint8			mPrevKeysState[SDL_NUM_SCANCODES];
};

// マウス入力のヘルパー
class MouseState
{
public:
	// 簡便のため
	friend class InputSystem;

	// ゲッター
	const Vector2& GetPosition()			const { return mCurrPosition; }
	const Vector2& GetRelativePosition()	const { return mRelativePosition; }
	const Vector2& GetWheelDelta ()			const { return mWheelDelta; }

	// キーを区別する列挙型
	// SDLがマウスボタンの列挙型を提供していない(代わりにマクロを提供している)
	// NameToCodeのロードの処理を他と合わせるために定義している
	enum MouseButton
	{
		Button_Left = 1,
		Button_Middle,
		Button_Right,
		Button_X1,
		Button_X2,
	};
	// キー状態を取得
	bool GetButtonPressed	(MouseButton button) const;
	bool GetButtonHeld		(MouseButton button) const;
	bool GetButtonReleased	(MouseButton button) const;
private:
	// マウスの位置を保持
	Vector2	mCurrPosition;
	Vector2 mRelativePosition;

	// マウスホイールの変位を保持
	Vector2 mWheelDelta;

	// ボタン状態の保持
	Uint32	mCurrButtonsState;
	Uint32	mPrevButtonsState;
};

// コントローラー入力のヘルパー
class ControllerState
{
public:
	// 簡便のため
	friend class InputSystem;

	// ボタン状態の取得
	bool GetButtonPressed	(SDL_GameControllerButton button) const;
	bool GetButtonHeld		(SDL_GameControllerButton button) const;
	bool GetButtonReleased	(SDL_GameControllerButton button) const;
	
	// スティック入力の取得
	const Vector2& GetLeftStickAxis()	const { return mLeftStickAxis; }
	const Vector2& GetRightStickAxis()	const { return mRightStickAxis; }

	// トリガー入力の取得
	float GetLeftTrigger()	const { return mLeftTriggerValue; }
	float GetRightTrigger()	const { return mRightTriggerValue; }
private:
	//SDL_GameControllerの保持(それぞれの値の取得に必須)
	SDL_GameController*	
			mGameController;

	// ボタン状態の保持
	Uint8	mCurrButtonsState[SDL_CONTROLLER_BUTTON_MAX];
	Uint8	mPrevButtonsState[SDL_CONTROLLER_BUTTON_MAX];

	// スティック入力の保持
	Vector2 mLeftStickAxis;
	Vector2 mRightStickAxis;

	// トリガー入力の保持
	float	mLeftTriggerValue;
	float	mRightTriggerValue;
};

// 複数入力のラッパー
struct InputState
{
	// ヘルパー
	KeyboardState					Keyboard;
	MouseState						Mouse;
	std::vector<ControllerState>	Controllers;

	// 入力をアクション名から取得する
	bool GetActionPressed	(const std::string& action, int controller = 0) const;
	bool GetActionHeld		(const std::string& action, int controller = 0) const;
	bool GetActionReleased	(const std::string& action, int controller = 0) const;

	// 入力マップのインデックスに用いる
	enum InputBindingsValueIndex
	{
		Device = 0,
		KeyButton,
		BindingsIndexNum
	};
	// 入力マップ <アクション名, [入力デバイス, 入力ボタン]>
	std::unordered_map<std::string, std::vector<std::string>> InputBindings;

	// 入力キー文字列とSDL_Scancodeを対応させる
	std::unordered_map<std::string, SDL_Scancode>	
		KeyboardNameToCode;
	// 入力ボタン文字列とマウスボタン列挙型を対応させる
	std::unordered_map<std::string, MouseState::MouseButton>
		MouseNameToCode;
	// 入力ボタン文字列とSDL_GameControllerButtonを対応させる
	std::unordered_map<std::string, SDL_GameControllerButton>
		ControllerNameToCode;
};

class InputSystem
{
public:
	// 初期化・終了
	bool Initialize();
	void Shutdown();
	
	// 入力マップ・文字列とコードの対応マップの読み込み
	void Load();

	// Update前の処理
	// 過去の状態を更新する・マウスホイールの値の初期化など
	// PollEvent前なのは、SDL_PollEventでマウスホイールの変位を取得するため
	void PrepareForPollEvent();

	// 現在の入力の更新
	void Update();

	// マウスホイール変位の更新
	void UpdateMouseWheelDelta(float x, float y);

	// コントローラーの管理
	void AddController		(SDL_JoystickID controller);
	void RemoveController	(SDL_JoystickID controller);

	// その他Getter
	const InputState& GetState() const { return mState; }
private:
	// InputBindingsの読み込み
	void LoadInputBindings	(const std::string& fileName);

	// mXxxNameToCodeの読み込み
	// LoadNameToCode<キー・ボタン列挙型>(ファイル名, NameToCodeマップ)みたいに使う
	// 個人メモ: templateは.hに書くべき(Actor::Create参照)だが、
	//			 呼ばれるのが同クラス内だけなら.cppで実装しても問題ない
	//			 -> template関数が同じファイル内でインスタンス化されるから
	template <typename CodeEnum, typename NameToCodeMap>
	void LoadNameToCode		(const std::string& fileName, NameToCodeMap& nameToCodeMap);

	// 上限と下限に近い値をそれぞれ限界値に合わせ、
	// それぞれ正規化する
	float	SnapAndNormalize1D(int input);
	Vector2 SnapAndNormalize2D(int inputX, int inputY);

	// 入力状態ラッパー
	InputState mState;
};

