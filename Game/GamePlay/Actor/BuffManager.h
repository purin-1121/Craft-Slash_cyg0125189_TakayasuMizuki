#pragma once

// バフの状態を管理するマネージャ
class BuffManager
{
public:

	enum class StatesEffectType
	{
		Power, 
		Defence,
		Speed,
		Armor,
		Invisible,
	};

	enum class ModifireType
	{
		Add,
		Subtract,
		Multiply,
		Divide
	};

	struct StatesEffect
	{
		// バフの種類
		StatesEffectType type = StatesEffectType::Power;
		// 数値
		float value = 0;
		// 残り時間
		float duration = 0;
		// バフの計算方法
		ModifireType modifireType = ModifireType::Add;
		// バフが永続かどうか
		bool infinitable = false;

		bool operator==(const StatesEffect& effect) const
		{
			return this->modifireType == effect.modifireType &&
				this->value == effect.value &&
				this->type == effect.type &&
				this->infinitable == effect.infinitable;
		}
	};

	BuffManager():m_effective{} {};
	~BuffManager() = default;
	void Update(float elapsedTime);
	void Add(StatesEffect effect) { m_effective.push_back(effect); };
	int& Calculate(int value, StatesEffectType type);

private:

	// 現在あるバフ
	std::vector<StatesEffect> m_effective;
	// 削除予定のバフ
	std::vector<StatesEffect> m_removeEffect;
	// 計算用ヘルパー
	int CalcHelper(float& value, ModifireType type, int& base);
};

