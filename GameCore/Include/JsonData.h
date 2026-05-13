#pragma once

#include "Math.h"
#include <rapidjson/document.h>

// TODO: セーブ側の処理を実装する

class JsonData
{
public:
    // JSONファイルのロード
    static bool LoadJSON(const std::string &fileName, JsonData &outData);

    // 型チェック
    template <typename T>
    bool IsType() const
    {
        if constexpr (std::is_same_v<T, Vector3>)
        {
            return mDocument.IsArray() && mDocument.Size() == 3;
        }
        else if constexpr (std::is_same_v<T, Quaternion>)
        {
            return mDocument.IsArray() && mDocument.Size() == 4;
        }
        else if constexpr (std::is_same_v<T, JsonObject>)
        {
            return mDocument.IsObject();
        }
        else if constexpr (std::is_same_v<T, JsonArray>)
        {
            return mDocument.IsArray();
        }

        return mDocument.Is<T>();
    }

    // JSONオブジェクトの取得
    const JsonObject GetObject();

    // JSON配列の取得
    const JsonArray GetArray();

private:
    rapidjson::Document mDocument;
};

class JsonArray
{
public:
    JsonArray() = default;

    // 値の取得
    template <typename T>
    bool Get(size_t index, T &outValue) const
    {
        if (index >= mValue.Size())
        {
            return false;
        }

        // Vector3
        if constexpr (std::is_same_v<T, Vector3>)
        {
            if (!JsonData::IsType<Vector3>())
            {
                return false;
            }

            outValue.x = mValue[index][0].GetDouble();
            outValue.y = mValue[index][1].GetDouble();
            outValue.z = mValue[index][2].GetDouble();

            return true;
        }

        // Quaternion
        if constexpr (std::is_same_v<T, Quaternion>)
        {
            if (!JsonData::IsType<Quaternion>())
            {
                return false;
            }

            outValue.x = mValue[index][0].GetDouble();
            outValue.y = mValue[index][1].GetDouble();
            outValue.z = mValue[index][2].GetDouble();
            outValue.w = mValue[index][3].GetDouble();

            return true;
        }

        // Array
        if constexpr (std::is_same_v<T, JsonArray>)
        {
            if (!JsonData::IsType<JsonArray>())
            {
                return false;
            }

            outValue.SetValue(mValue[index]);

            return true;
        }

        // Object
        if constexpr (std::is_same_v<T, JsonObject>)
        {
            if (!JsonData::IsType<JsonObject>())
            {
                return false;
            }

            outValue.SetValue(mValue[index]);

            return true;
        }

        // その他の型
        const auto &element = mValue[index];
        if (!element.Is<T>())
        {
            return false;
        }

        outValue = element.Get<T>();

        return true;
    }

    // 配列のサイズを取得
    size_t Size() const { return mValue.Size(); }

private:
    // JsonData, JsonObjectクラスからのみ値を設定できるようにする
    friend class JsonData;
    friend class JsonObject;
    JsonArray(rapidjson::Value &value) : mValue(value) {}
    void SetValue(rapidjson::Value &value) { mValue = value; }

private:
    rapidjson::Value mValue;
};

class JsonObject
{
public:
    JsonObject() = default;

    // 値の取得
    template <typename T>
    bool Get(const char *propertyName, T &outValue) const
    {
        auto iter = mValue.FindMember(propertyName);
        if (iter == mValue.MemberEnd())
        {
            return false;
        }

        const auto &property = iter->value;

        // Vector3
        if constexpr (std::is_same_v<T, Vector3>)
        {
            if (!JsonData::IsType<Vector3>())
            {
                return false;
            }

            outValue.x = property[0].GetDouble();
            outValue.y = property[1].GetDouble();
            outValue.z = property[2].GetDouble();

            return true;
        }

        // Quaternion
        if constexpr (std::is_same_v<T, Quaternion>)
        {
            if (!JsonData::IsType<Quaternion>())
            {
                return false;
            }

            outValue.x = property[0].GetDouble();
            outValue.y = property[1].GetDouble();
            outValue.z = property[2].GetDouble();
            outValue.w = property[3].GetDouble();

            return true;
        }

        // Array
        if constexpr (std::is_same_v<T, JsonArray>)
        {
            if (!JsonData::IsType<JsonArray>())
            {
                return false;
            }

            outValue.SetValue(property);

            return true;
        }

        // Object
        if constexpr (std::is_same_v<T, JsonObject>)
        {
            if (!JsonData::IsType<JsonObject>())
            {
                return false;
            }

            outValue.SetValue(property);

            return true;
        }

        // その他の型
        if (!property.Is<T>())
        {
            return false;
        }

        outValue = property.Get<T>();

        return true;
    }

private:
    // JsonData, JsonObjectクラスからのみ値を設定できるようにする
    friend class JsonData;
    friend class JsonArray;
    JsonObject(rapidjson::Value &value) : mValue(value) {}
    void SetValue(rapidjson::Value &value) { mValue = value; }

private:
    rapidjson::Value mValue;
};