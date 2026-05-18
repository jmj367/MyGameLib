#pragma once

#include "Math.h"
#include <rapidjson/document.h>

// TODO: セーブ側の処理を実装する

class JsonData
{
public:
    // JSONファイルのロード
    static bool LoadJSON(const std::string &fileName, JsonData &outData);

    // JSON配列、JSONオブジェクトからJsonDataを作成する
    bool CreateJsonData(const JsonArray &jsonArray);
    bool CreateJsonData(const JsonObject &jsonObject);

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

    template <typename T>
    static bool IsType(const T &value)
    {
        if constexpr (std::is_same_v<T, Vector3>)
        {
            return value.IsArray() && value.Size() == 3;
        }
        else if constexpr (std::is_same_v<T, Quaternion>)
        {
            return value.IsArray() && value.Size() == 4;
        }
        else if constexpr (std::is_same_v<T, JsonObject>)
        {
            return value.IsObject();
        }
        else if constexpr (std::is_same_v<T, JsonArray>)
        {
            return value.IsArray();
        }

        return value.Is<T>();
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
        if (index >= mValuePtr->Size())
        {
            return false;
        }

        const auto &element = mValuePtr->operator[](index);

        // Vector3
        if constexpr (std::is_same_v<T, Vector3>)
        {
            if (!JsonData::IsType<Vector3>(element))
            {
                return false;
            }

            outValue.x = element[0].GetDouble();
            outValue.y = element[1].GetDouble();
            outValue.z = element[2].GetDouble();

            return true;
        }

        // Quaternion
        if constexpr (std::is_same_v<T, Quaternion>)
        {
            if (!JsonData::IsType<Quaternion>(element))
            {
                return false;
            }

            outValue.x = element[0].GetDouble();
            outValue.y = element[1].GetDouble();
            outValue.z = element[2].GetDouble();
            outValue.w = element[3].GetDouble();

            return true;
        }

        // Array
        if constexpr (std::is_same_v<T, JsonArray>)
        {
            if (!JsonData::IsType<JsonArray>(element))
            {
                return false;
            }

            outValue.SetValuePtr(element);

            return true;
        }

        // Object
        if constexpr (std::is_same_v<T, JsonObject>)
        {
            if (!JsonData::IsType<JsonObject>(element))
            {
                return false;
            }

            outValue.SetValuePtr(element);

            return true;
        }

        // その他の型
        if (!element.Is<T>())
        {
            return false;
        }

        outValue = element.Get<T>();

        return true;
    }

    // 値の追加
    template <typename T>
    bool PushBack(const T &value)
    {
        // Vector3
        if constexpr (std::is_same_v<T, Vector3>)
        {
            rapidjson::Value vecValue(rapidjson::kArrayType);
            vecValue.PushBack(value.x, mValuePtr->GetAllocator());
            vecValue.PushBack(value.y, mValuePtr->GetAllocator());
            vecValue.PushBack(value.z, mValuePtr->GetAllocator());

            mValuePtr->PushBack(vecValue, mValuePtr->GetAllocator());

            return true;
        }

        // Quaternion
        if constexpr (std::is_same_v<T, Quaternion>)
        {
            rapidjson::Value quatValue(rapidjson::kArrayType);
            quatValue.PushBack(value.x, mValuePtr->GetAllocator());
            quatValue.PushBack(value.y, mValuePtr->GetAllocator());
            quatValue.PushBack(value.z, mValuePtr->GetAllocator());
            quatValue.PushBack(value.w, mValuePtr->GetAllocator());

            mValuePtr->PushBack(quatValue, mValuePtr->GetAllocator());

            return true;
        }

        // Array
        if constexpr (std::is_same_v<T, JsonArray>)
        {
            rapidjson::Value arrValue(rapidjson::kArrayType);
            for (size_t i = 0; i < value.Size(); ++i)
            {
                const auto &element = value.GetValuePtr()->operator[](i);
                arrValue.PushBack(element, mValuePtr->GetAllocator());
            }
            mValuePtr->PushBack(arrValue, mValuePtr->GetAllocator());

            return true;
        }

        // Object
        if constexpr (std::is_same_v<T, JsonObject>)
        {
            rapidjson::Value objValue(rapidjson::kObjectType);
            for (auto iter = value.GetValuePtr()->MemberBegin(); iter != value.GetValuePtr()->MemberEnd(); ++iter)
            {
                const auto &name = iter->name;
                const auto &property = iter->value;
                objValue.AddMember(name, property, mValuePtr->GetAllocator());
            }
            mValuePtr->PushBack(objValue, mValuePtr->GetAllocator());

            return true;
        }

        // その他の型
        if (!mValuePtr->Is<T>())
        {
            return false;
        }

        mValuePtr->PushBack(value, mValuePtr->GetAllocator());

        return true;
    }

    // 配列のサイズを取得
    size_t Size() const { return mValuePtr->Size(); }

private:
    // JsonData, JsonObjectクラスからのみ値を設定できるようにする
    friend class JsonData;
    friend class JsonObject;

    JsonArray(rapidjson::Value *valuePtr) : mValuePtr(valuePtr) {}

    void SetValuePtr(rapidjson::Value *valuePtr) { mValuePtr = valuePtr; }
    rapidjson::Value *GetValuePtr() const { return mValuePtr; }

private:
    rapidjson::Value *mValuePtr = nullptr;
};

class JsonObject
{
public:
    JsonObject() = default;

    // 値の取得
    template <typename T>
    bool Get(const char *propertyName, T &outValue) const
    {
        auto iter = mValuePtr->FindMember(propertyName);
        if (iter == mValuePtr->MemberEnd())
        {
            return false;
        }

        const auto &property = iter->value;

        // Vector3
        if constexpr (std::is_same_v<T, Vector3>)
        {
            if (!JsonData::IsType<Vector3>(property))
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
            if (!JsonData::IsType<Quaternion>(property))
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
            if (!JsonData::IsType<JsonArray>(property))
            {
                return false;
            }

            outValue.SetValuePtr(property);

            return true;
        }

        // Object
        if constexpr (std::is_same_v<T, JsonObject>)
        {
            if (!JsonData::IsType<JsonObject>(property))
            {
                return false;
            }

            outValue.SetValuePtr(property);

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

    // 値の追加
    template <typename T>
    bool AddMember(const char *propertyName, const T &value)
    {
        // Vector3
        if constexpr (std::is_same_v<T, Vector3>)
        {
            rapidjson::Value vecValue(rapidjson::kArrayType);
            vecValue.PushBack(value.x, mValuePtr->GetAllocator());
            vecValue.PushBack(value.y, mValuePtr->GetAllocator());
            vecValue.PushBack(value.z, mValuePtr->GetAllocator());

            mValuePtr->AddMember(rapidjson::StringRef(propertyName), vecValue, mValuePtr->GetAllocator());

            return true;
        }

        // Quaternion
        if constexpr (std::is_same_v<T, Quaternion>)
        {
            rapidjson::Value quatValue(rapidjson::kArrayType);
            quatValue.PushBack(value.x, mValuePtr->GetAllocator());
            quatValue.PushBack(value.y, mValuePtr->GetAllocator());
            quatValue.PushBack(value.z, mValuePtr->GetAllocator());
            quatValue.PushBack(value.w, mValuePtr->GetAllocator());

            mValuePtr->AddMember(rapidjson::StringRef(propertyName), quatValue, mValuePtr->GetAllocator());

            return true;
        }

        // Array
        if constexpr (std::is_same_v<T, JsonArray>)
        {
            rapidjson::Value arrValue(rapidjson::kArrayType);
            for (size_t i = 0; i < value.Size(); ++i)
            {
                const auto &element = value.GetValuePtr()->operator[](i);
                arrValue.PushBack(element, mValuePtr->GetAllocator());
            }
            mValuePtr->AddMember(rapidjson::StringRef(propertyName), arrValue, mValuePtr->GetAllocator());

            return true;
        }

        // Object
        if constexpr (std::is_same_v<T, JsonObject>)
        {
            rapidjson::Value objValue(rapidjson::kObjectType);
            for (auto iter = value.GetValuePtr()->MemberBegin(); iter != value.GetValuePtr()->MemberEnd(); ++iter)
            {
                const auto &name = iter->name;
                const auto &property = iter->value;
                objValue.AddMember(name, property, mValuePtr->GetAllocator());
            }
            mValuePtr->AddMember(rapidjson::StringRef(propertyName), objValue, mValuePtr->GetAllocator());

            return true;
        }

        // その他の型
        if (!mValuePtr->Is<T>())
        {
            return false;
        }

        mValuePtr->AddMember(rapidjson::StringRef(propertyName), value, mValuePtr->GetAllocator());
        
        return true;
    }

private:
    // JsonData, JsonObjectクラスからのみ値を設定できるようにする
    friend class JsonData;
    friend class JsonArray;

    JsonObject(rapidjson::Value *valuePtr) : mValuePtr(valuePtr) {}

    void SetValuePtr(rapidjson::Value *valuePtr) { mValuePtr = valuePtr; }
    rapidjson::Value *GetValuePtr() const { return mValuePtr; }

private:
    rapidjson::Value *mValuePtr = nullptr;
};