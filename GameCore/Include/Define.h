#pragma once
#include <cstddef>
#include <utility>
#include <unordered_map>

// 共通の定義

// リソースIDの型
using ResourceID = size_t;

// ペア用のハッシュ関数
struct pair_hash
{
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2> &p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // Boostのhash_combineの実装を参考にしたハッシュ関数
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
}