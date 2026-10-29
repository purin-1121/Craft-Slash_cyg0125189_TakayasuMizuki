#include "pch.h"
#include "ItemManager.h"
#include "Common/Draw/Draw.h"
#include "Lib/Camera.h"
#include "Lib/MathLib.h"
#include "Common/Manager/StorageManager.h"
#include "InventryData.h"

using namespace DirectX;
using json = nlohmann::json;

ItemManager::ItemManager(const int& itemCount)
	: m_items{}
	, m_num{}
	, m_itemCount{itemCount}
{

	LoadItemData();
}

void ItemManager::Initialize(Canvas&, const GameContext& gameContext)
{
	m_items.reserve(m_itemCount) ;
	for (int i = 0; i < m_itemCount; i++)
	{
		m_items.push_back(std::make_unique<Item>(i, SimpleMath::Vector3{0,0,0}, SimpleMath::Vector3{0,0,0}, nullptr));
	}
	for (auto& i : m_items)
	{
		i->Initialize(gameContext);
	}
}

void ItemManager::Update(float elapsedTime, SimpleMath::Vector3 pos, const GameContext& gameContext, Camera& camera)
{
	m_num = 0;
	for (auto& i : m_items)
	{
		if (!i->IsActive()) continue;
		
		if ((i->GetPosition() - pos).Length() < 2.0f)
			i->ChangeState(ItemState::Action);

		i->Update(elapsedTime ,gameContext, camera);	
		i->SetVelocity(MathLib::SetStreeringBehavier(i->GetVelocity(), i->GetPosition(), pos, MAX_SPEED, MAX_FORCE));

		m_num++;
	}
	
}

void ItemManager::Render(const GameContext& gameContext, SimpleMath::Matrix view)
{
}

void ItemManager::RenderText()
{
}

void ItemManager::RenderBounding(const GameContext& gameContext, SimpleMath::Matrix view)
{

}

void ItemManager::Finalize()
{
	for (auto& i : m_items)
	{
		i->Finalize();
	}
}

void ItemManager::InitItem(SimpleMath::Vector3 pos, uint8_t type, SimpleMath::Vector3 vec, int index)
{
	std::string name = "";
	if (type & ItemFlag::WOOD) name = "Wood";
	else if (type & ItemFlag::MEAT) name = "Meat";
	else if (type & ItemFlag::COAL) name = "Coal";
	else if (type & ItemFlag::STONE) name = "Stone";
	else if (type & ItemFlag::BONE) name = "Bone";
	else if (type & ItemFlag::LEATHER) name = "Leather";

	// 番号指定があったらそれを初期化
	if (index >= 0)
	{
		m_items[index]->Create(pos, &m_itemPool[name]);
		return;
	}
	// なかったら探索して初期化
	auto i = std::find_if(m_items.begin(), m_items.end(), [&](auto& a) {return !a->IsActive();});
	if(i != m_items.end())
			i->get()->Create(pos, &m_itemPool[name], vec);
}

void ItemManager::PostUpdate()
{
}

void ItemManager::LoadItemData()
{
	// Jsonの読み込み
	std::string str = StorageManager::LoadTextFile("Resources/Jsons/ItemData.json");
	try {
		json _json = json::parse(str);
		if (_json.is_array())
		{
			// 配列の各要素をループ処理
			for (const auto& item : _json)
			{
				if (item.is_object() &&
					item.contains("type") &&
					item.contains("name") &&
					item.contains("model") 
					)
				{
					// アイテムプールの生成
					uint8_t type = item["type"].get<uint8_t>();
					if (type & ItemFlag::WOOD)
					{
						m_itemPool.emplace( item["name"].get<std::string>(),
							ItemSetting{ ItemFlag::WOOD, item["name"].get<std::string>(), item["model"].get<std::string>() }
						);
					}
					if (type & ItemFlag::COAL)
					{
						m_itemPool.emplace(item["name"].get<std::string>(),
							ItemSetting{ ItemFlag::COAL, item["name"].get<std::string>(), item["model"].get<std::string>() }
						);
					}
					if (type & ItemFlag::STONE)
					{
						m_itemPool.emplace(item["name"].get<std::string>(),
							ItemSetting{ ItemFlag::STONE, item["name"].get<std::string>(), item["model"].get<std::string>() }
						);
					}
					if (type & ItemFlag::LEATHER)
					{
						m_itemPool.emplace( item["name"].get<std::string>(),
							ItemSetting{ ItemFlag::LEATHER, item["name"].get<std::string>(), item["model"].get<std::string>() }
						);					
					}
					if (type & ItemFlag::BONE)
					{
						m_itemPool.emplace( item["name"].get<std::string>(),
							ItemSetting{ ItemFlag::BONE, item["name"].get<std::string>(), item["model"].get<std::string>() }
						);					
					}
				}
			}
		}
	}
	catch(const json::parse_error& e){
		std::cerr << "解析に失敗" << e.what() << std::endl;
	}
}


