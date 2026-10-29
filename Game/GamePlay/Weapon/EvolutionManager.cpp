#include "pch.h"
#include "EvolutionManager.h"
#include <algorithm>

EvolutionManager::EvolutionManager()
{
    Initialize();
}

void EvolutionManager::Initialize()
{
    m_recipes.clear();

    LoadRecipe();

    // 優先度が高い順にソート
    std::sort(m_recipes.begin(), m_recipes.end(), [](const WeaponRecipe& a, const WeaponRecipe& b) {
        return a.priorityLevel > b.priorityLevel;
        });

    auto e =
        [&](void* messanger)
        {
            CheckEvolution(*static_cast<std::array<InventryData::Slot, InventryData::MAX_SLOT>*>(messanger));
        };
    m_evolutionID = EventBus::GetInstance()->Subscribe(Message::EventID::PICKUP_ITEM, e);
}

void EvolutionManager::Finalize()
{
    EventBus::GetInstance()->UnSubscribe(Message::EventID::PICKUP_ITEM, m_evolutionID);
}

void EvolutionManager::CheckEvolution(const std::array<InventryData::Slot, InventryData::MAX_SLOT>& inventory)
{
    // 所持素材のビットマスクと個数を集計
    uint8_t currentMask = 0;
    std::unordered_map<uint8_t, int> currentCounts;

    for (const auto& slot : inventory)
    {
        if (slot.active && slot.num > 0)
        {
            currentMask |= slot.type;
            currentCounts[slot.type] += slot.num;
        }
    }

    std::string targetName = INIT_WEAPON.data();

    for (const auto& recipe : m_recipes)
    {
        // 一次判定：ビットANDで必要素材の種類があるかチェック
        if ((currentMask & recipe.requiestMask) != recipe.requiestMask)
        {
            continue;
        }

        // 二次判定：各素材の個数が足りているかチェック
        bool satisfied = true;
        for (const auto& material : recipe.requiredMaterials)
        {
            if (currentCounts[material.type] < material.num)
            {
                satisfied = false;
                break;
            }
        }

        if (satisfied)
        {
            targetName = recipe.recipeName;
            break;
        }
    }

    // 武器の種類が変わっていたらイベント発行
    if (targetName != m_currentName)
    {
        m_currentName = targetName;

        EventBus::GetInstance()->Publish(
            Message::EventID::WEAPON_EVOLVED,
            &m_currentName
        );
    }
}

void EvolutionManager::LoadRecipe()
{
    // Jsonの読み込み
    std::string str = StorageManager::LoadTextFile("Resources/Jsons/WeaponRecipe.json");
    try {
        json _json = json::parse(str);
        if (_json.is_array())
        {
            // 配列の各要素をループ処理
            for (const auto& weapon : _json)
            {
                if (weapon.is_object() && weapon.contains("recipeName"))
                {
                    // 基本データの取得
                    auto data = weapon.get<WeaponRecipe>();
                    
                    m_recipes.push_back(data);
                }
            }
        }
    }
    catch (const json::parse_error& e) {
        std::cerr << "解析に失敗" << e.what() << std::endl;
    }
}
