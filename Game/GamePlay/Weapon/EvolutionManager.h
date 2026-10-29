#pragma once
#include <vector>
#include <array>
#include "Game/GamePlay/Data/WeaponData.h"
#include "Game/GamePlay/Data/WeaponRecipe.h"
#include "Message/EventBus.h"
#include "Game/GamePlay/Item/InventryData.h"


class EvolutionManager
{
public:
    EvolutionManager();

    void Initialize();
    void Finalize();
    void CheckEvolution(const std::array<InventryData::Slot, InventryData::MAX_SLOT>& inventory);

private:
    std::vector<WeaponRecipe> m_recipes;
    std::string m_currentName = "Sword";
    SubsprictionID m_evolutionID;

    void LoadRecipe();
};