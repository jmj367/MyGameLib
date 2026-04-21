#include "Component.h"
#include "Actors/Actor.h"
#include "Commons/LevelLoader.h"
#include "Inputs/InputSystem.h"

// Componentのサブクラスはここで
// LevelLoaderの種類・生成関数対応マップに登録する

//namespace
//{
//	bool registerd = LevelLoader::RegisterToComponentFactoryMap(
//		"Componentのサブクラス", &Component::Create<Componentのサブクラス>);
//}

Component::Component(Actor* owner, int updateOrder)
	: mOwningActor	(owner)
	, mUpdateOrder	(updateOrder)
	, mStatus		(EEnable)
{
	mOwningActor->AddComponent(this);
}

Component::~Component()
{
	mOwningActor->RemoveComponent(this);
}

// プロパティ名
namespace
{
	const char* UpdateOrder = "updateOrder";
}

void Component::LoadProperties(const rapidjson::Value& inObject)
{
	// プロパティをロードする
	JsonHelper::GetInt(inObject, UpdateOrder, mUpdateOrder);
}

void Component::SaveProperties(rapidjson::Document::AllocatorType& allocator,
	rapidjson::Value& inObject) const
{
	// プロパティをセーブする
	JsonHelper::AddInt(allocator, inObject, UpdateOrder, mUpdateOrder);
}

void Component::Update(float deltaTime)
{
	// コンポーネントが有効な時だけ更新
	if (mStatus == EEnable)
	{
		UpdateComponent(deltaTime);
	}
}

void Component::UpdateComponent(float deltaTime)
{

}

void Component::ProcessInput(const InputState& state)
{

}

void Component::OnUpdateWorldTransform()
{

}