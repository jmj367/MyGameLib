#pragma once
#include "Math.h"
#include <typeindex>
#include <rapidjson/document.h>
#include <string>

// プレーンなコンポーネント
// このクラスのサブクラスは.cppファイルで
// LevelLoaderの種類・生成関数対応マップに自身の生成関数を登録する
// 登録関数は関数定義の前に書くこととする
class Component
{
public:
	Component(class Actor* owner, int updateOrder = 0);
	virtual ~Component();

	// ロード・セーブ処理
	virtual void LoadProperties(const rapidjson::Value& inObject);
	virtual void SaveProperties(rapidjson::Document::AllocatorType& allocator,
		rapidjson::Value& inObject) const;

	// コンポーネントの生成
	// 詳細は Actor::Create を参照
	template <typename ComponentType>
	static Component* Create(class Actor* owner, const rapidjson::Value& inObject)
	{
		// コンポーネントを生成する
		ComponentType* comp = new ComponentType(owner);
		comp->LoadProperties(inObject);
		return comp;
	}

	// 更新処理
	void Update(float deltaTime);
	virtual void UpdateComponent(float deltaTime);

	 // 入力処理
	virtual void ProcessInput(const struct InputState& state);

	// アクターのワールドトランスフォームが更新されたときに呼ばれる
	virtual void OnUpdateWorldTransform();

	// 状態関係
	enum ComponentStatus
	{
		EEnable,
		EDisable
	};
	ComponentStatus	GetStatus() const					{ return mStatus;	}
	void			SetStatus(ComponentStatus state)	{ mStatus = state;	}

	// コンポーネントの種類を表す文字列を返す
	// 各サブクラスでこの関数をオーバーライドし、対応した文字列を返すようにする
	// 文字列は別で定義した方がいいかもしれない
	// とりあえず動くことを優先
	virtual const std::string GetType() { return "Component"; }

	// その他ゲッター
	class Actor*	GetActor()				{ return mOwningActor; }
	int				GetUpdateOrder() const	{ return mUpdateOrder; }
private:
	// このコンポーネントの所有アクター
	class Actor* mOwningActor;

	// このコンポーネントの更新順
	// この値が小さいほど更新が早い
	int mUpdateOrder;

	ComponentStatus mStatus;
};