#pragma once

#include "Math.h"
#include <rapidjson/document.h>
#include <string>

class JsonHelper
{
public:
	// JSONファイルのロード
	static bool LoadJSON(const std::string& fileName, rapidjson::Document& outDocument);

	// Jsonから値を取得する際のヘルパー
	// 取得に成功すればtrueを返す
	static bool GetInt			(const rapidjson::Value& inObject, const char* inPropertyName, int&			outValue);
	static bool GetFloat		(const rapidjson::Value& inObject, const char* inPropertyName, float&		outValue);
	static bool GetBool			(const rapidjson::Value& inObject, const char* inPropertyName, bool&		outValue);
	static bool GetString		(const rapidjson::Value& inObject, const char* inPropertyName, std::string& outValue);
	static bool GetVector3		(const rapidjson::Value& inObject, const char* inPropertyName, Vector3&		outValue);
	static bool GetQuaternion	(const rapidjson::Value& inObject, const char* inPropertyName, Quaternion&	outValue);

	// Jsonに値を設定する際のヘルパー
	static void AddInt			(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& inObject, const char* propertyName, int					value);
	static void AddFloat		(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& inObject, const char* propertyName, float					value);
	static void AddBool			(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& inObject, const char* propertyName, bool					value);
	static void AddString		(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& inObject, const char* propertyName, const std::string&	value);
	static void AddVector3		(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& inObject, const char* propertyName, const Vector3&		value);
	static void AddQuaternion	(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& inObject, const char* propertyName, const Quaternion&		value);
};