#include "pch.h"
#include "DropFactory.h"
#include "Game/GamePlay/Item/ItemManager.h"
#include "Game/GamePlay/Blocks/Drop.h"

std::unique_ptr<DropFactory> DropFactory::m_dropFc = nullptr;
std::function<void(GameObject*, const Drop*, int, int)> DropFactory::m_dropMethod = nullptr;

void DropFactory::CreateDropMethod(ItemManager* item)
{
	// Bind to the ItemManager owned by the current gameplay session.
	m_dropMethod = [item](GameObject* obj, const Drop* info, int min, int max) {
		// アイテムを生成する個数を決定
		int r = Random::Range(min, max);
		float speed = 10.0f;
		// 最大個数よりループすることを防ぐ
		if (r > item->MaxItems()) r = item->MaxItems();
		for (int i = 0; i < r; i++)
		{
			// 1周（XM_PI * 2）を個数で割って、i番目の角度を計算
			float angle = (XM_PI * 2 / r) * i;

			SimpleMath::Vector3 launchVelocity{
				cosf(angle) * speed,
				Random::Range(5.0f, 8.0f),
				sinf(angle) * speed
			};

			// アイテムの生成
			uint8_t type = ItemFlag::GetRandomType(info->GetTypeInfo());
			// デバッグ
			if (type & ~ItemFlag::WOOD)
			{
				int a = type;
			}
			item->InitItem(obj->GetBounding()[0]->GetCenter(), type, launchVelocity);
		}
	};
}

void DropFactory::LoadDropData()

{
	std::string str = StorageManager::LoadTextFile("Resources/Jsons/DropData.json");
	try {
		json _json = json::parse(str);
		if (_json.is_array())
		{
			// 配列の各要素をループ処理
			for (const auto& item : _json)
			{
				if (item.is_object() &&
					item.contains("name") &&
					item.contains("type")
					)
				{
					TypeInfo t{};

					t.type = item["type"].get<uint8_t>();
					t.seedSum = static_cast<int>(std::bitset<8>(t.type).count());

					m_dropPool.emplace(item["name"].get<std::string>(), t);
				}
			}
		}
	}
	catch (const json::parse_error& e) {
		std::cerr << "解析に失敗" << e.what() << std::endl;
	}
}
