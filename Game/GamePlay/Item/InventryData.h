//====================================================//
// ファイル名   : InventryData.h
// 作成者       : 高安瑞貴
//
// 概要 : インベントリで扱うデータを管理する構造体
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "Game/GamePlay/Item/DropData.h"

//====================================================//
// 宣言
//====================================================//


namespace ItemFlag
{
	// アイテムの種類識別用
	constexpr uint8_t NONE = 0;
	constexpr uint8_t WOOD = 1 << 0;	// 1
	constexpr uint8_t MEAT = 1 << 1;	// 2
	constexpr uint8_t COAL = 1 << 2;	// 4 
	constexpr uint8_t STONE = 1 << 3;	// 8
	constexpr uint8_t BONE = 1 << 4;	// 16
	constexpr uint8_t LEATHER = 1 << 5;	// 32
	constexpr int MAX_ITEMS = 6;

	static inline uint8_t GetRandomType(const TypeInfo& info)
	{
		int r = Random::Range(0, info.seedSum);
		int same = 0;
		for (int i = 0; i < 8; i++)
		{
			if (1 << i & info.type)
			{
				if (same >= r - 1)
					return 1 << i;
				else
					same++;
			}
		}
	}

	static inline std::wstring GetNameToType(const uint8_t& type)
	{
		if (type & WOOD) return L"木材";
		else if (type & MEAT) return L"肉";
		else if (type & COAL) return L"石炭";
		else if (type & STONE) return L"石";
		else if (type & BONE) return L"骨";
		else if (type & LEATHER) return L"皮";
		else return L"なし";
	}
}

namespace InventryData
{
	struct Slot
	{
		bool active = false;
		uint8_t type = ItemFlag::NONE;
		int num = 0;

		void Reset()
		{
			active = false;
			num = 0;
			type = ItemFlag::NONE;
		}

	};

	// アイテムスロットの数
	static constexpr int MAX_SLOT = 10;
	// 1スロットの最大数
	static constexpr int MAX_ITEM = 256;
}




