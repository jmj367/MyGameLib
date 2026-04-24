#include "JsonHelper.h"
#include <fstream>
// SDLのログ機能を使うかは後で考える
// #include <SDL/SDL_log.h>
#include <vector>

bool JsonHelper::LoadJSON(const std::string& fileName, rapidjson::Document& outDocument)
{
	// Jsonファイルを読み込む
	// ファイルサイズを取得するために読み取り位置を末尾に設定
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		//SDL_Log("File %s not found", fileName.c_str());
		return false;
	}

	// ファイルサイズを取得
	std::ifstream::pos_type fileSize = file.tellg();
	// ファイルを読み取るために読み取り位置を先頭に設定する
	file.seekg(0, std::ios::beg);

	// 文字列にファイルの内容を格納
	std::vector<char> bytes(static_cast<size_t>(fileSize) + 1);
	file.read(bytes.data(), static_cast<size_t>(fileSize));

	// 文字列をRapidJSON Doccumentに変換
	outDocument.Parse(bytes.data());
	if (!outDocument.IsObject())
	{
		//SDL_Log("File %s is not valid JSON", fileName.c_str());
		return false;
	}

	return true;
}


bool JsonHelper::GetInt(const rapidjson::Value& inObject, const char* inProperty, int& outValue)
{
	// 引数のプロパティが存在するか確認
	auto iter = inObject.FindMember(inProperty);
	if (iter == inObject.MemberEnd())
	{
		return false;
	}

	// 値を取得、整数か確認
	auto& property = iter->value;
	if (!property.IsInt())
	{
		return false;
	}

	outValue = property.GetInt();

	return true;
}

bool JsonHelper::GetFloat(const rapidjson::Value& inObject, const char* inProperty, float& outValue)
{
	// 引数のプロパティが存在するか確認
	auto iter = inObject.FindMember(inProperty);
	if (iter == inObject.MemberEnd())
	{
		return false;
	}

	// 値を取得、少数か確認
	auto& property = iter->value;
	if (!property.IsDouble())
	{
		return false;
	}

	outValue = property.GetDouble();

	return true;
}

bool JsonHelper::GetBool(const rapidjson::Value& inObject, const char* inProperty, bool& outValue)
{
	// 引数のプロパティが存在するか確認
	auto iter = inObject.FindMember(inProperty);
	if (iter == inObject.MemberEnd())
	{
		return false;
	}

	// 値を取得、boolか確認
	auto& property = iter->value;
	if (!property.IsBool())
	{
		return false;
	}

	outValue = property.GetBool();

	return true;
}

bool JsonHelper::GetString(const rapidjson::Value& inObject, const char* inProperty, std::string& outValue)
{
	// 引数のプロパティが存在するか確認
	auto iter = inObject.FindMember(inProperty);
	if (iter == inObject.MemberEnd())
	{
		return false;
	}

	// 値を取得、文字列か確認
	auto& property = iter->value;
	if (!property.IsString())
	{
		return false;
	}

	outValue = property.GetString();

	return true;
}

bool JsonHelper::GetVector3(const rapidjson::Value& inObject, const char* inProperty, Vector3& outValue)
{
	// 引数のプロパティが存在するか確認
	auto iter = inObject.FindMember(inProperty);
	if (iter == inObject.MemberEnd())
	{
		return false;
	}

	// 値を取得、Vector3か確認
	auto& property = iter->value;
	if (!property.IsArray() || property.Size() != 3)
	{
		return false;
	}

	// x, y, zが少数か確認
	for (rapidjson::SizeType i = 0; i < 3; i++)
	{
		if (!property[i].IsDouble())
		{
			return false;
		}
	}

	outValue.x = property[0].GetDouble();
	outValue.y = property[1].GetDouble();
	outValue.z = property[2].GetDouble();

	return true;
}

bool JsonHelper::GetQuaternion(const rapidjson::Value& inObject, const char* inProperty, Quaternion& outValue)
{
	// 引数のプロパティが存在するか確認
	auto iter = inObject.FindMember(inProperty);
	if (iter == inObject.MemberEnd())
	{
		return false;
	}

	// 値を取得、Quaternionか確認
	auto& property = iter->value;
	if (!property.IsArray() || property.Size() != 4)
	{
		return false;
	}

	// x, y, z, wが少数か確認
	for (rapidjson::SizeType i = 0; i < 4; i++)
	{
		if (!property[i].IsDouble())
		{
			return false;
		}
	}

	outValue.x = property[0].GetDouble();
	outValue.y = property[1].GetDouble();
	outValue.z = property[2].GetDouble();
	outValue.w = property[3].GetDouble();

	return true;
}

void JsonHelper::AddInt(rapidjson::Document::AllocatorType& allocator,
	rapidjson::Value& inObject, const char* name, int value)
{
	rapidjson::Value v(value);
	inObject.AddMember(rapidjson::StringRef(name), v, allocator);
}

void JsonHelper::AddFloat(rapidjson::Document::AllocatorType& allocator,
	rapidjson::Value& inObject, const char* name, float value)
{
	rapidjson::Value v(value);
	inObject.AddMember(rapidjson::StringRef(name), v, allocator);
}

void JsonHelper::AddBool(rapidjson::Document::AllocatorType& allocator,
	rapidjson::Value& inObject, const char* name, bool value)
{
	rapidjson::Value v(value);
	inObject.AddMember(rapidjson::StringRef(name), v, allocator);
}

void JsonHelper::AddString(rapidjson::Document::AllocatorType& allocator,
	rapidjson::Value& inObject, const char* name, const std::string& value)
{
	rapidjson::Value v;
	v.SetString(value.c_str(), static_cast<rapidjson::SizeType>(value.length()), allocator);
	inObject.AddMember(rapidjson::StringRef(name), v, allocator);
}

void JsonHelper::AddVector3(rapidjson::Document::AllocatorType& allocator,
	rapidjson::Value& inObject, const char* name, const Vector3& value)
{
	rapidjson::Value v(rapidjson::kArrayType);

	v.PushBack(rapidjson::Value(value.x).Move(), allocator);
	v.PushBack(rapidjson::Value(value.y).Move(), allocator);
	v.PushBack(rapidjson::Value(value.z).Move(), allocator);

	inObject.AddMember(rapidjson::StringRef(name), v, allocator);
}

void JsonHelper::AddQuaternion(rapidjson::Document::AllocatorType& allocator,
	rapidjson::Value& inObject, const char* name, const Quaternion& value)
{
	rapidjson::Value v(rapidjson::kArrayType);

	v.PushBack(rapidjson::Value(value.x).Move(), allocator);
	v.PushBack(rapidjson::Value(value.y).Move(), allocator);
	v.PushBack(rapidjson::Value(value.z).Move(), allocator);
	v.PushBack(rapidjson::Value(value.w).Move(), allocator);

	inObject.AddMember(rapidjson::StringRef(name), v, allocator);
}