#include "pch.h"
#include "BuffManager.h"

void BuffManager::Update(float elapsedTime)
{
	for (auto& e : m_effective)
	{
		e.duration -= elapsedTime;
		// バフの時間が無くなったら削除候補に入れておく
		if (e.duration < 0 && !e.infinitable)
		{
			m_removeEffect.push_back(e);
		}
	}

	// 削除用データがあったら
	if (!m_removeEffect.empty())
	{
		for (auto& e : m_removeEffect)
		{
			std::remove(m_effective.begin(), m_effective.end(), e);
		}
	}
}

int& BuffManager::Calculate(int base, StatesEffectType type)
{
	int value = base;

	if (m_effective.empty())
		value;

	// 加算減算
	for (auto& m : m_effective)
	{
		if (type != m.type)
			continue;

		if (m.modifireType == ModifireType::Add || m.modifireType == ModifireType::Subtract)
			CalcHelper(m.value, m.modifireType, value);
	}

	// 乗算除算
	for (auto& m : m_effective)
	{
		if (type != m.type)
			continue;

		if (m.modifireType == ModifireType::Multiply || m.modifireType == ModifireType::Divide)
			CalcHelper(m.value, m.modifireType, value);
	}

	return value;
}

int BuffManager::CalcHelper(float& value, ModifireType type, int& base)
{
	switch (type)
	{
	case BuffManager::ModifireType::Add:
		base += value;
		break;
	case BuffManager::ModifireType::Subtract:
		base -= value;
		break;
	case BuffManager::ModifireType::Multiply:
		base *= value;
		break;
	case BuffManager::ModifireType::Divide:
		base /= value;
		break;
	default:
		break;
	}

	return value;
}
