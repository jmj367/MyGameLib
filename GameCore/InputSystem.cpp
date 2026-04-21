#include "InputSystem.h"
#include "Defines/FileNames.h"
#include <cstring>
#include <fstream>
#include <magic_enum/magic_enum.hpp>
#include <SDL/SDL.h>
#include <sstream>
#include <type_traits>

// キーボード
bool KeyboardState::GetKeyPressed(SDL_Scancode key) const
{
	return !mPrevKeysState[key] && mCurrKeysState[key];
}

bool KeyboardState::GetKeyHeld(SDL_Scancode key) const
{
	return mCurrKeysState[key];
}

bool KeyboardState::GetKeyReleased(SDL_Scancode key) const
{
	return mPrevKeysState[key] && !mCurrKeysState[key];
}

// マウス
// マウスボタンだけ他と感じが違うので整理したほうがいいか？要検討
bool MouseState::GetButtonPressed(MouseButton button) const
{
	int mask = SDL_BUTTON(button);
	return !(mask & mPrevButtonsState) && (mask & mCurrButtonsState);
}

bool MouseState::GetButtonHeld(MouseButton button) const
{
	return SDL_BUTTON(button) & mCurrButtonsState;
}

bool MouseState::GetButtonReleased(MouseButton button) const
{
	int mask = SDL_BUTTON(button);
	return (mask & mPrevButtonsState) && !(mask & mCurrButtonsState);
}

// コントローラー
bool ControllerState::GetButtonPressed(SDL_GameControllerButton button) const
{
	return !mPrevButtonsState[button] && mCurrButtonsState[button];
}

bool ControllerState::GetButtonHeld(SDL_GameControllerButton button) const
{
	return mCurrButtonsState[button];
}

bool ControllerState::GetButtonReleased(SDL_GameControllerButton button) const
{
	return mPrevButtonsState[button] && !mCurrButtonsState[button];
}

bool InputState::GetActionPressed(const std::string& action, int controller) const
{
	auto iter = InputBindings.find(action);
	if (iter == InputBindings.end())
	{
		return false;
	}

	std::string device = iter->second[Device];
	std::string keyButton = iter->second[KeyButton];
	if (device == "Keyboard")
	{
		return Keyboard.GetKeyPressed(KeyboardNameToCode.find(keyButton)->second);
	}
	if (device == "Mouse")
	{
		return Mouse.GetButtonPressed(MouseNameToCode.find(keyButton)->second);
	}
	if (device == "Controller")
	{
		return Controllers[controller].GetButtonPressed(ControllerNameToCode.find(keyButton)->second);
	}

	return false;
}

bool InputState::GetActionHeld(const std::string& action, int controller) const
{
	auto iter = InputBindings.find(action);
	if (iter == InputBindings.end())
	{
		return false;
	}

	std::string device = iter->second[Device];
	std::string keyButton = iter->second[KeyButton];
	if (device == "Keyboard")
	{
		return Keyboard.GetKeyHeld(KeyboardNameToCode.find(keyButton)->second);
	}
	if (device == "Mouse")
	{
		return Mouse.GetButtonHeld(MouseNameToCode.find(keyButton)->second);
	}
	if (device == "Controller")
	{
		return Controllers[controller].GetButtonHeld(ControllerNameToCode.find(keyButton)->second);
	}

	return false;
}

bool InputState::GetActionReleased(const std::string& action, int controller) const
{
	auto iter = InputBindings.find(action);
	if (iter == InputBindings.end())
	{
		return false;
	}

	std::string device = iter->second[Device];
	std::string keyButton = iter->second[KeyButton];
	if (device == "Keyboard")
	{
		return Keyboard.GetKeyReleased(KeyboardNameToCode.find(keyButton)->second);
	}
	if (device == "Mouse")
	{
		return Mouse.GetButtonReleased(MouseNameToCode.find(keyButton)->second);
	}
	if (device == "Controller")
	{
		return Controllers[controller].GetButtonReleased(ControllerNameToCode.find(keyButton)->second);
	}

	return false;
}

bool InputSystem::Initialize()
{
	// キーボード初期化
	// mCurrKeysStateは常にキー状態を取得するようになる
	mState.Keyboard.mCurrKeysState = SDL_GetKeyboardState(NULL);
	memset(mState.Keyboard.mPrevKeysState, 0, SDL_NUM_SCANCODES);

	// マウス初期化
	mState.Mouse.mCurrButtonsState = 0;
	mState.Mouse.mPrevButtonsState = 0;
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// コントローラー初期化
	// 接続されているコントローラー全てをvectorに格納
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		ControllerState cs;

		memset(cs.mCurrButtonsState, 0, SDL_CONTROLLER_BUTTON_MAX);
		memset(cs.mPrevButtonsState, 0, SDL_CONTROLLER_BUTTON_MAX);

		cs.mGameController = SDL_GameControllerOpen(i);

		mState.Controllers.emplace_back(cs);
	}

	Load();

	return true;
}

void InputSystem::Shutdown()
{
	// 現在接続されているコントローラーを全て切断
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		ControllerState controller = mState.Controllers[i];
		SDL_GameControllerClose(controller.mGameController);
	}
}

void InputSystem::Load()
{
	LoadInputBindings						(ACTION_MAP_FILE_NAMES::INPUT_BINDINGS);
	LoadNameToCode<SDL_Scancode>			(ACTION_MAP_FILE_NAMES::KEYBOARD_NAME_TO_CODE,		mState.KeyboardNameToCode	);
	LoadNameToCode<MouseState::MouseButton>	(ACTION_MAP_FILE_NAMES::MOUSE_NAME_TO_CODE,			mState.MouseNameToCode		);
	LoadNameToCode<SDL_GameControllerButton>(ACTION_MAP_FILE_NAMES::CONTROLLER_NAME_TO_CODE,	mState.ControllerNameToCode	);
}

void InputSystem::PrepareForPollEvent()
{
	// 過去の状態を更新する
	// キーボード
	memcpy(mState.Keyboard.mPrevKeysState, mState.Keyboard.mCurrKeysState, SDL_NUM_SCANCODES);

	// マウス
	mState.Mouse.mPrevButtonsState = mState.Mouse.mCurrButtonsState;

	// コントローラー
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		memcpy(
			mState.Controllers[i].mPrevButtonsState,
			mState.Controllers[i].mCurrButtonsState,
			SDL_CONTROLLER_BUTTON_MAX);
	}

	// マウスホイール値初期化
	mState.Mouse.mWheelDelta = Vector2::Zero;
}

void InputSystem::Update()
{
	// キーボードはすでにキーを取得している(Initialize参照)

	// マウス
	int x = 0;
	int y = 0;
	// ボタン状態とマウス位置を取得
	mState.Mouse.mCurrButtonsState = SDL_GetRelativeMouseState(&x, &y);
	mState.Mouse.mRelativePosition.x = static_cast<float>(x);
	mState.Mouse.mRelativePosition.y = static_cast<float>(y);
	SDL_GetMouseState(&x, &y);
	mState.Mouse.mCurrPosition.x = x;
	mState.Mouse.mCurrPosition.y = y;

	// コントローラー
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		ControllerState& c = mState.Controllers[i];
		// ボタン
		for (int button = 0; button < SDL_CONTROLLER_BUTTON_MAX; button++)
		{
			c.mCurrButtonsState[button] = SDL_GameControllerGetButton(
				c.mGameController,
				SDL_GameControllerButton(button));
		}

		// トリガー
		c.mLeftTriggerValue = SnapAndNormalize1D(
			SDL_GameControllerGetAxis(
				c.mGameController,
				SDL_CONTROLLER_AXIS_TRIGGERLEFT));
		c.mRightTriggerValue = SnapAndNormalize1D(
			SDL_GameControllerGetAxis(
				c.mGameController,
				SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

		// スティック
		x = SDL_GameControllerGetAxis(
				c.mGameController, SDL_CONTROLLER_AXIS_LEFTX);
		y = SDL_GameControllerGetAxis(
				c.mGameController, SDL_CONTROLLER_AXIS_LEFTY);
		c.mLeftStickAxis = SnapAndNormalize2D(x, y);

		x = SDL_GameControllerGetAxis(
				c.mGameController, SDL_CONTROLLER_AXIS_RIGHTX);
		y = SDL_GameControllerGetAxis(
				c.mGameController, SDL_CONTROLLER_AXIS_RIGHTY);
		c.mRightStickAxis = SnapAndNormalize2D(x, y);
	}
}

void InputSystem::UpdateMouseWheelDelta(float x, float y)
{
	mState.Mouse.mWheelDelta = Vector2(x, y);
}

void InputSystem::AddController(SDL_JoystickID controller)
{
	// 接続したコントローラーの初期化
	if (SDL_IsGameController(controller))
	{
		SDL_GameController* gc = SDL_GameControllerOpen(controller);
		ControllerState cs;

		memset(cs.mCurrButtonsState, 0, SDL_CONTROLLER_BUTTON_MAX);
		memset(cs.mPrevButtonsState, 0, SDL_CONTROLLER_BUTTON_MAX);

		cs.mGameController = gc;
		mState.Controllers.emplace_back(cs);
	}
}

void InputSystem::RemoveController(SDL_JoystickID controller)
{
	if (SDL_IsGameController(controller))
	{
		// 引数のIDに該当するコントローラーを検索
		int i;
		for (i = 0; i < mState.Controllers.size(); i++)
		{
			SDL_JoystickID compareID = SDL_JoystickInstanceID(
				SDL_GameControllerGetJoystick(
					mState.Controllers[i].mGameController));
			if (compareID == controller)
			{
				break;
			}
		}

		// コントローラーを削除
		SDL_GameControllerClose(mState.Controllers[i].mGameController);
		mState.Controllers.erase(mState.Controllers.begin() + i);
	}
}

void InputSystem::LoadInputBindings(const std::string& fileName)
{
	// csvを読み込む
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("InputBindings file %s is not found.", fileName.c_str());
		return;
	}

	std::string line;

	// 1行目を飛ばす
	if (!std::getline(file, line))
	{
		SDL_Log("InputBinding file %s is empty.", fileName.c_str());
		return;
	}

	// 行の読み取り
	while (std::getline(file, line))
	{
		// 空行は飛ばす
		if (line.empty())
		{
			continue;
		}

		// 値の読み取り
		std::stringstream ss(line);
		std::string action, device, keyBtn;

		if (
			std::getline(ss, action, ',') && 
			std::getline(ss, device, ',') &&
			std::getline(ss, keyBtn, ','))
		{
			std::vector<std::string> deviceKeyBtn;
			deviceKeyBtn.emplace_back(device);
			deviceKeyBtn.emplace_back(keyBtn);
			mState.InputBindings.emplace(action, deviceKeyBtn);
		}
	}
}

template <typename CodeEnum, typename NameToCodeMap>
void InputSystem::LoadNameToCode(const std::string& fileName, NameToCodeMap& nameToCodeMap)
{
	// csvを読み込む
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("NameToCode file %s is not found.", fileName.c_str());
		return;
	}

	std::string line;

	// 1行目は値の名前なので飛ばす
	if (!std::getline(file, line))
	{
		SDL_Log("NameToCode file %s is empty.", fileName.c_str());
		return;
	}

	// 行の読み取り
	while (std::getline(file, line))
	{
		// 空行は飛ばす
		if (line.empty())
		{
			continue;
		}

		// 値の読み取り
		std::stringstream ss(line);
		std::string keyBtnStr, codeStr;

		if (
			std::getline(ss, keyBtnStr,	',') &&
			std::getline(ss, codeStr,	','))
		{
			// ファイルのコード(文字列)を列挙型に変換
			auto codeEnumOpt = magic_enum::enum_cast<CodeEnum>(codeStr);

			if (codeEnumOpt.has_value())
			{
				nameToCodeMap.emplace(keyBtnStr, codeEnumOpt.value());
			}
			else
			{
				SDL_Log("LoadNameToCode didn't find %s", codeStr);
			}
		}
	}
}

float InputSystem::SnapAndNormalize1D(int input)
{
	const int deadZone = 250;
	const int maxValue = 30000;

	// スナップ
	float retVal = 0.0f;
	int absValue = Math::Abs(input);
	if (absValue > deadZone)
	{
		// 正規化
		retVal = static_cast<float>(
			absValue - deadZone) / (maxValue - deadZone);

		retVal = input > 0 ? retVal : -retVal;

		retVal = Math::Clamp(retVal, -1.0f, 1.0f);
	}

	return retVal;
}

Vector2 InputSystem::SnapAndNormalize2D(int inputX, int inputY)
{
	const float deadZone = 8000.0f;
	const float maxValue = 30000.0f;

	Vector2 dir;
	dir.x = static_cast<float>(inputX);
	dir.y = static_cast<float>(inputY);

	float length = dir.Length();

	// スナップ
	if (length < deadZone)
	{
		dir = Vector2::Zero;
	}
	else
	{
		// 正規化
		float f = (length - deadZone) / (maxValue - deadZone);
		f = Math::Clamp(f, 0.0f, 1.0f);
		dir *= f / length;
	}

	return dir;
}
