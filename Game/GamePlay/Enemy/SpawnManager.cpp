#include "pch.h"
#include "SpawnManager.h"

SpawnManager::SpawnManager(EnemyManager* pManager)
	: m_pEnemyManager{pManager}
	, m_areas{}
{
}

SpawnManager::~SpawnManager()
{
	Finalize();
	SaveArea();
}

void SpawnManager::Initialize()
{
	LoadArea();
	m_areas.reserve(100);
	for (auto& s : m_areas)
	{
		s->Initialize();
	}

	auto defeat =
		[&](void* messager)
		{
			auto it = std::find_if(m_areas.begin(), m_areas.end(), [messager](const auto& a) 
				{return a->GetID() == *static_cast<int*>(messager); });
			if(it != m_areas.end())
				it->get()->EnemyDeath();
		};

	m_defeatSubscriptionID = EventBus::GetInstance()->Subscribe(Message::EventID::ENEMY_TAKE_DAMAGE, defeat);

	auto set =
		[&](void* messanger)
		{
			std::unique_ptr<SpawnArea> a = 
				m_of.CreateForJson<SpawnArea>("Normal",m_pEnemyManager,*static_cast<SimpleMath::Vector3*>(messanger));
			a->Initialize();
			m_areas.push_back(std::move(a));
		};

	m_setAreaSubscriptionID = EventBus::GetInstance()->Subscribe(Message::EventID::SET_SPAWN_AREA, set);
	m_saveAreaSubscriptionID = EventBus::GetInstance()->Subscribe(Message::EventID::SET_SPAWN_AREA, [this](void*) {SaveArea(); });
	m_eventsSubscribed = true;
}

void SpawnManager::Update(float elapsedTime, const GameContext& gameContext, Camera& camera)
{
	for (auto& s : m_areas)
	{
		s->Update(elapsedTime, gameContext, camera);
		if (s->CanSpawn())
		{
			float t = s->GetSpawnTime();
			m_pEnemyManager->CreateEnemy(s->GetRandomType(), s->GetSpawnPosition(), s->GetID());
			s->CompleteSpanw();
		}
	}
}

void SpawnManager::Render(const GameContext& gameContext, Camera&)
{

}
void SpawnManager::RenderText()
{

}
void SpawnManager::RenderEdit()
{
	for (auto& a : m_areas)
	{
		Draw::GetInstance().DrawSphere(16, 0.3, a->GetPosition());
	}
}

void SpawnManager::Finalize()
{
	if (m_eventsSubscribed)
	{
		auto* events = EventBus::GetInstance();
		events->UnSubscribe(Message::EventID::ENEMY_TAKE_DAMAGE, m_defeatSubscriptionID);
		events->UnSubscribe(Message::EventID::SET_SPAWN_AREA, m_setAreaSubscriptionID);
		events->UnSubscribe(Message::EventID::SET_SPAWN_AREA, m_saveAreaSubscriptionID);
		m_eventsSubscribed = false;
	}

	for (auto& a : m_areas)
	{
		a->Finalize();
	}
}

void SpawnManager::LoadArea()
{
	// Jsonの読み込み
	std::string str = StorageManager::LoadTextFile("Resources/Jsons/SpawnAreaPosData.json");
	try {
		json _json = json::parse(str);
		if (_json.is_array())
		{
			// 配列の各要素をループ処理
			for (const auto& j : _json)
			{
				// コメントの場合は次へ
				if (j.contains("Comment")) continue;

				SimpleMath::Vector3 pos;
				if (j.is_object())
				{
					if (j["pos"].is_array())
					{
						pos.x = j["pos"][0].get<float>();
						pos.y = j["pos"][1].get<float>();
						pos.z = j["pos"][2].get<float>();
					}
					auto a = m_of.CreateForJson<SpawnArea>("Normal", m_pEnemyManager, pos);
					m_areas.push_back(std::move(a));
				}
			}
		}
	}
	catch (const json::parse_error& e) {
		std::cerr << "解析に失敗" << e.what();
	}
}

void SpawnManager::SaveArea()
{
	auto ofs = StorageManager::GetInstance()->LoadInputTextFile("Resources/Jsons/SpawnAreaPosData.json");
	try
	{
		json j = json::array();
		for (auto& o : m_areas)
		{
			json i;
			auto pos = o->GetPosition();
			i["pos"] = { MathLib::RoundPosition(pos.x),
						MathLib::RoundPosition(pos.y),
						MathLib::RoundPosition(pos.z) };
			j.push_back(i);
		}
		ofs << j.dump(4) << std::endl;
	}
	catch (const std::exception&)
	{
		ofs.close();
	}
	ofs.close();
}
