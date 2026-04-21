#pragma once

#include <algorithm>
#include <atomic>
#include <functional>
#include <mutex>
#include <typeindex>
#include <unordered_map>
#include <vector>

class EventBus
{
public:
	// ハンドラのID型
	using HandlerId = uint32_t;

	// 自動解除のためのトークンクラス
	class SubscriptionToken
	{
	public:
		SubscriptionToken() = default;
		// コピー禁止
		SubscriptionToken(const SubscriptionToken&) = delete;
		SubscriptionToken& operator=(const SubscriptionToken&) = delete;
		// ムーブ許可
		SubscriptionToken(SubscriptionToken&& other) noexcept
			: mBus		(other.mBus		)
			, mTypeIdx	(other.mTypeIdx	)
			, mId		(other.mId		)
		{
			other.mBus = nullptr;
		}

		SubscriptionToken& operator=(SubscriptionToken&& other) noexcept
		{
			if (this != &other)
			{
				// 古い購読を解除
				if (mBus)
				{
					mBus->Unsubscribe(mTypeIdx, mId);
				}

				// メンバをムーブ
				mBus = other.mBus;
				mTypeIdx = other.mTypeIdx;
				mId = other.mId;

				// もう一方を無効化
				other.mBus = nullptr;
			}
			return *this;
		}

		~SubscriptionToken()
		{
			// 購読解除
			if(mBus)
			{
				mBus->Unsubscribe(mTypeIdx, mId);
			}
		}
	private:
		friend class EventBus;
		SubscriptionToken(EventBus* bus, std::type_index idx, HandlerId id)
			: mBus		(bus)
			, mTypeIdx	(idx)
			, mId		(id	)
		{ }
		
		EventBus* mBus = nullptr;
		std::type_index mTypeIdx = std::type_index(typeid(void));
		HandlerId mId = 0;
	};

	// 購読
	template<typename EventType>
	[[nodiscard]]
	SubscriptionToken Subscribe(std::function<void(EventType&)> handler)
	{
		std::lock_guard<std::mutex> lock(mMutex);

		HandlerId id = mNextId++;
		// void* を使って型消去
		auto wrapper = [handler](void* event)
		{
			handler(*static_cast<EventType*>(event));
		};

		// ハンドラ登録
		mSubscribers[std::type_index(typeid(EventType))].push_back({ id, wrapper });

		return SubscriptionToken(this, std::type_index(typeid(EventType)), id);
	}

	// 発行
	template<typename EventType>
	void Publish(EventType event)
	{
		std::lock_guard<std::mutex> lock(mMutex);

		auto typeIdx = std::type_index(typeid(EventType));
		if (mSubscribers.find(typeIdx) != mSubscribers.end())
		{
			// 登録されている全ハンドラを呼び出し
			for (const auto& entry : mSubscribers[typeIdx])
			{
				entry.handler(&event);
			}
		}
	}

	// 発行(キューイング)
	template<typename EventType>
	void EnqueueEvent(EventType event)
	{
		std::lock_guard<std::mutex> lock(mQueueMutex);

		// イベントを値コピーしてラムダ内に保持
		mEventQueue.push_back([this, event]() mutable 
			{
				this->Publish(event);
			}
		);
	}

	// キュー内のイベントをすべて発行
	void ProcessEventQueue()
	{
		std::vector<std::function<void()>> processingQueue;

		// 処理中に新しいイベントが追加されるのを防ぐため
		// 現在のキューをローカル変数にスワップしてロックを即解放する
		{
			std::lock_guard<std::mutex> lock(mQueueMutex);
			processingQueue.swap(mEventQueue);
		}

		for(auto& task : processingQueue)
		{
			task();
		}
	}
private:
	// 購読解除(SubscriptionToken用)
	void Unsubscribe(std::type_index typeIdx, HandlerId id)
	{
		std::lock_guard<std::mutex> lock(mMutex);

		auto it = mSubscribers.find(typeIdx);
		if(it != mSubscribers.end())
		{
			auto& handlers = it->second;

			// 指定されたIDのハンドラを削除
			handlers.erase(
				std::remove_if(
					handlers.begin(),
					handlers.end(),
					[id](const HandlerEntry& entry)
					{
						return entry.id == id;
					}
				),
				handlers.end()
			);
		}
	}

	// ハンドラ登録用エントリ構造体
	struct HandlerEntry
	{
		HandlerId id;
		std::function<void(void*)> handler;
	};

	std::unordered_map<std::type_index, std::vector<HandlerEntry>> mSubscribers;
	std::mutex mMutex;
	std::atomic<HandlerId> mNextId{ 1 };

	// キュー用
	std::vector<std::function<void()>> mEventQueue;
	std::mutex mQueueMutex;
};