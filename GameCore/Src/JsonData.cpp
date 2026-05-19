#include "JsonData.h"
#include <fstream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <vector>

bool JsonData::LoadJSON(const std::string &fileName)
{
    // JSONファイルを読み込む
    // ファイルサイズを取得するために読み取り位置を末尾に設定
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
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
    mDocument.Parse(bytes.data());
    if (!mDocument.IsObject())
    {
        return false;
    }

    return true;
}

bool JsonData::SaveJSON(const std::string &fileName)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    std::ofstream file(fileName);
    if (!file.is_open())
    {
        return false;
    }

    mDocument.Accept(writer);
    file << buffer.GetString();
    return true;
}

bool JsonData::CreateJsonData(const JsonArray &jsonArray)
{
    mDocument.SetArray();
    for (size_t i = 0; i < jsonArray.GetValuePtr()->Size(); i++)
    {
        mDocument.PushBack(jsonArray.GetValuePtr()->operator[](i), mDocument.GetAllocator());
    }

    return true;
}

bool JsonData::CreateJsonData(const JsonObject &jsonObject)
{
    mDocument.SetObject();
    for (auto iter = jsonObject.GetValuePtr()->MemberBegin(); iter != jsonObject.GetValuePtr()->MemberEnd(); iter++)
    {
        mDocument.AddMember(iter->name, iter->value, mDocument.GetAllocator());
    }

    return true;
}

const JsonObject JsonData::GetObject()
{
    return JsonObject(&mDocument);
}

const JsonArray JsonData::GetArray()
{
    return JsonArray(&mDocument);
}