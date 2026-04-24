#include "JsonData.h"
#include <fstream>
#include <vector>

bool JsonData::LoadJSON(const std::string &fileName, JsonData &outData)
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
    outData.mDocument.Parse(bytes.data());
    if (!outData.mDocument.IsObject())
    {
        return false;
    }

    return true;
}

const JsonObject JsonData::GetObject()
{
    // 値をコピーして渡す
    rapidjson::Document::AllocatorType &allocator = mDocument.GetAllocator();
    rapidjson::Value value;
    value.CopyFrom(mDocument, allocator);

    return JsonObject(value);
}

const JsonArray JsonData::GetArray()
{
    // 値をコピーして渡す
    rapidjson::Document::AllocatorType &allocator = mDocument.GetAllocator();
    rapidjson::Value value;
    value.CopyFrom(mDocument, allocator);

    return JsonArray(value);
}