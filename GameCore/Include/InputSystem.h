#pragma once
#include "Math.h"
#include <cstdint>
#include <string>
#include <SDL2/SDL_gamecontroller.h>
#include <unordered_map>
#include <vector>

// キーボード入力のヘルパー
class KeyboardState
{
public:
	// 簡便のため
	friend class InputSystem;

	// キーを区別する列挙型(SDL_Scancodeをラップ)
	enum KeyboardButton
	{
		EUnknown = 0,

		EA = 4,
		EB = 5,
		EC = 6,
		ED = 7,
		EE = 8,
		EF = 9,
		EG = 10,
		EH = 11,
		EI = 12,
		EJ = 13,
		EK = 14,
		EL = 15,
		EM = 16,
		EN = 17,
		EO = 18,
		EP = 19,
		EQ = 20,
		ER = 21,
		ES = 22,
		ET = 23,
		EU = 24,
		EV = 25,
		EW = 26,
		EX = 27,
		EY = 28,
		EZ = 29,

		E0 = 30,
		E1 = 31,
		E2 = 32,
		E3 = 33,
		E4 = 34,
		E5 = 35,
		E6 = 36,
		E7 = 37,
		E8 = 38,
		E9 = 39,

		EReturn = 40,
		EEscape = 41,
		EBackspace = 42,
		ETab = 43,
		ESpace = 44,

		EMinus = 45,
		EEquals = 46,
		ELeftBracket = 47,
		ERightBracket = 48,
		EBackslash = 49,
		ENonUSHash = 50,
		ESemicolon = 51,
		EApostrophe = 52,
		EGrave = 53,
		EComma = 54,
		EPeriod = 55,
		ESlash = 56,

		ECapsLock = 57,

		// Function keys
		EF1 = 58,
		EF2 = 59,
		EF3 = 60,
		EF4 = 61,
		EF5 = 62,
		EF6 = 63,
		EF7 = 64,
		EF8 = 65,
		EF9 = 66,
		EF10 = 67,
		EF11 = 68,
		EF12 = 69,

		EPrintScreen = 70,
		EScrollLock = 71,
		EPause = 72,
		EInsert = 73,
		EHome = 74,
		EPageUp = 75,
		EDelete = 76,
		EEnd = 77,
		EPageDown = 78,
		ERight = 79,
		ELeft = 80,
		EDown = 81,
		EUp = 82,

		ENumLockClear = 83,
		EKpDivide = 84,
		EKpMultiply = 85,
		EKpMinus = 86,
		EKpPlus = 87,
		EKpEnter = 88,
		EKp1 = 89,
		EKp2 = 90,
		EKp3 = 91,
		EKp4 = 92,
		EKp5 = 93,
		EKp6 = 94,
		EKp7 = 95,
		EKp8 = 96,
		EKp9 = 97,
		EKp0 = 98,
		EKpPeriod = 99,

		ENonUSBackslash = 100,
		EApplication = 101,
		EPower = 102,
		EKpEquals = 103,
		EF13 = 104,
		EF14 = 105,
		EF15 = 106,
		EF16 = 107,
		EF17 = 108,
		EF18 = 109,
		EF19 = 110,
		EF20 = 111,
		EF21 = 112,
		EF22 = 113,
		EF23 = 114,
		EF24 = 115,
		EExecute = 116,
		EHelp = 117,
		EMenu = 118,
		ESelect = 119,
		EStop = 120,
		EAgain = 121,
		EUndo = 122,
		ECut = 123,
		ECopy = 124,
		EPaste = 125,
		EFind = 126,
		EMute = 127,
		EVolumeUp = 128,
		EVolumeDown = 129,
		/* not sure whether there's a reason to enable these */
		/*     ELockingCapsLock = 130,  */
		/*     ELockingNumLock = 131, */
		/*     ELockingScrollLock = 132, */
		EKpComma = 133,
		EKpEqualsAs400 = 134,

		EInternational1 = 135,
		EInternational2 = 136,
		EInternational3 = 137,
		EInternational4 = 138,
		EInternational5 = 139,
		EInternational6 = 140,
		EInternational7 = 141,
		EInternational8 = 142,
		EInternational9 = 143,
		ELang1 = 144,
		ELang2 = 145,
		ELang3 = 146,
		ELang4 = 147,
		ELang5 = 148,
		ELang6 = 149,
		ELang7 = 150,
		ELang8 = 151,
		ELang9 = 152,

		EAltErase = 153,
		ESysReq = 154,
		ECancel = 155,
		EClear = 156,
		EPrior = 157,
		EReturn2 = 158,
		ESeparator = 159,
		EOut = 160,
		EOper = 161,
		EClearAgain = 162,
		ECrSel = 163,
		EExSel = 164,

		EKp00 = 176,
		EKp000 = 177,
		EKpThousandsSeparator = 178,
		EKpDecimalSeparator = 179,
		EKpCurrencyUnit = 180,
		EKpCurrencySubUnit = 181,
		EKpLeftparen = 182,
		EKpRightparen = 183,
		EKpLeftbrace = 184,
		EKpRightbrace = 185,
		EKpTab = 186,
		EKpBackspace = 187,
		EKpA = 188,
		EKpB = 189,
		EKpC = 190,
		EKpD = 191,
		EKpE = 192,
		EKpF = 193,
		EKpXor = 194,
		EKpPower = 195,
		EKpPercent = 196,
		EKpLess = 197,
		EKpGreater = 198,
		EKpAmpersand = 199,
		EKpDblampersand = 200,
		EKpVerticalbar = 201,
		EKpDblverticalbar = 202,
		EKpColon = 203,
		EKpHash = 204,
		EKpSpace = 205,
		EKpAt = 206,
		EKpExclam = 207,
		EKpMemstore = 208,
		EKpMemrecall = 209,
		EKpMemclear = 210,
		EKpMemadd = 211,
		EKpMemsubtract = 212,
		EKpMemmultiply = 213,
		EKpMemdivide = 214,
		EKpPlusMinus = 215,
		EKpClear = 216,
		EKpClearEntry = 217,
		EKpBinary = 218,
		EKpOctal = 219,
		EKpDecimal = 220,
		EKpHexadecimal = 221,

		ELCtrl = 224,
		ELShift = 225,
		ELAlt = 226,
		ELGui = 227,
		ERCtrl = 228,
		ERShift = 229,
		ERAlt = 230,
		ERGui = 231,

		EMode = 257,

		EAudioNext = 258,
		EAudioPrev = 259,
		EAudioStop = 260,
		EAudioPlay = 261,
		EAudioMute = 262,
		EMediaSelect = 263,
		EWww = 264,
		EMail = 265,
		ECalculator = 266,
		EComputer = 267,
		EAcSearch = 268,
		EAcHome = 269,
		EAcBack = 270,
		EAcForward = 271,
		EAcStop = 272,
		EAcRefresh = 273,
		EAcBookmarks = 274,

		EBrightnessDown = 275,
		EBrightnessUp = 276,
		EDisplaySwitch = 277,
		EKbdIllumToggle = 278,
		EKbdIllumDown = 279,
		EKbdIllumUp = 280,
		EEject = 281,
		ESleep = 282,

		EApp1 = 283,
		EApp2 = 284,

		EMax = 512
	};

	// キー状態の取得
	bool GetKeyPressed(KeyboardButton key) const;
	bool GetKeyHeld(KeyboardButton key) const;
	bool GetKeyReleased(KeyboardButton key) const;

private:
	// キー状態の保持
	const uint8_t *mCurrKeysState;
	uint8_t mPrevKeysState[KeyboardButton::EMax];
};

// マウス入力のヘルパー
class MouseState
{
public:
	// 簡便のため
	friend class InputSystem;

	// キーを区別する列挙型
	enum MouseButton
	{
		ELeft = 1,
		EMiddle,
		ERight,
		EX1,
		EX2,

		EMax
	};

	// ゲッター
	const Vector2 &GetPosition() const { return mCurrPosition; }
	const Vector2 &GetRelativePosition() const { return mRelativePosition; }
	const Vector2 &GetWheelDelta() const { return mWheelDelta; }

	// キー状態を取得
	bool GetButtonPressed(MouseButton button) const;
	bool GetButtonHeld(MouseButton button) const;
	bool GetButtonReleased(MouseButton button) const;

private:
	// マウスの位置を保持
	Vector2 mCurrPosition;
	Vector2 mRelativePosition;

	// マウスホイールの変位を保持
	Vector2 mWheelDelta;

	// ボタン状態の保持
	uint32_t mCurrButtonsState;
	uint32_t mPrevButtonsState;
};

// コントローラー入力のヘルパー
class ControllerState
{
public:
	// 簡便のため
	friend class InputSystem;

	// キーを区別する列挙型(SDL_GameControllerButtonをラップ)
	enum ControllerButton
	{
		EA = 0,
		EB = 1,
		EX = 2,
		EY = 3,
		EBack = 4,
		EGuide = 5,
		EStart = 6,
		ELeftStick = 7,
		ERightStick = 8,
		ELeftShoulder = 9,
		ERightShoulder = 10,
		EDpadUp = 11,
		EDpadDown = 12,
		EDpadLeft = 13,
		EDpadRight = 14,

		EMax
	};

	// ボタン状態の取得
	bool GetButtonPressed(ControllerButton button) const;
	bool GetButtonHeld(ControllerButton button) const;
	bool GetButtonReleased(ControllerButton button) const;

	// スティック入力の取得
	const Vector2 &GetLeftStickAxis() const { return mLeftStickAxis; }
	const Vector2 &GetRightStickAxis() const { return mRightStickAxis; }

	// トリガー入力の取得
	float GetLeftTrigger() const { return mLeftTriggerValue; }
	float GetRightTrigger() const { return mRightTriggerValue; }

private:
	// SDL_GameControllerの保持(それぞれの値の取得に必須)
	SDL_GameController *
		mGameController;

	// ボタン状態の保持
	uint8_t mCurrButtonsState[ControllerButton::EMax];
	uint8_t mPrevButtonsState[ControllerButton::EMax];

	// スティック入力の保持
	Vector2 mLeftStickAxis;
	Vector2 mRightStickAxis;

	// トリガー入力の保持
	float mLeftTriggerValue;
	float mRightTriggerValue;
};

// 複数入力のラッパー
struct InputState
{
	// ヘルパー
	KeyboardState Keyboard;
	MouseState Mouse;
	std::vector<ControllerState> Controllers;

	// 入力をアクション名から取得する
	bool GetActionPressed(const std::string &action, int controller = 0) const;
	bool GetActionHeld(const std::string &action, int controller = 0) const;
	bool GetActionReleased(const std::string &action, int controller = 0) const;

	// 入力マップのインデックスに用いる
	enum InputBindingsValueIndex
	{
		Device = 0,
		KeyButton,
		BindingsIndexNum
	};
	// 入力マップ <アクション名, [入力デバイス, 入力ボタン]>
	std::unordered_map<std::string, std::vector<std::string>> InputBindings;

	// 入力キー文字列とKeyboardButtonを対応させる
	std::unordered_map<std::string, KeyboardState::KeyboardButton>
		KeyboardNameToCode;
	// 入力ボタン文字列とMouseButton列挙型を対応させる
	std::unordered_map<std::string, MouseState::MouseButton>
		MouseNameToCode;
	// 入力ボタン文字列とControllerButtonを対応させる
	std::unordered_map<std::string, ControllerState::ControllerButton>
		ControllerNameToCode;
};

class InputSystem
{
public:
	// 初期化・終了
	bool Initialize();
	void Shutdown();

	// InputBindingsの読み込み
	// 拡張子はcsvを想定
	// 一行目はヘッダーとして読み飛ばす
	void LoadInputBindings(const std::string &fileName);

	// Update前の処理
	// 過去の状態を更新する・マウスホイールの値の初期化など
	// PollEvent前なのは、SDL_PollEventでマウスホイールの変位を取得するため
	void PrepareForPollEvent();

	// 現在の入力の更新
	void Update();

	// マウスホイール変位の更新
	void UpdateMouseWheelDelta(float x, float y);

	// コントローラーの管理
	void AddController(Sint32 controller);
	void RemoveController(Sint32 controller);

	// その他Getter
	const InputState &GetState() const { return mState; }

private:
	// 上限と下限に近い値をそれぞれ限界値に合わせ、
	// それぞれ正規化する
	float SnapAndNormalize1D(int input);
	Vector2 SnapAndNormalize2D(int inputX, int inputY);

	// 入力状態ラッパー
	InputState mState;
};
