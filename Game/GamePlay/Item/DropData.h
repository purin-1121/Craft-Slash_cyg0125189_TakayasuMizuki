#pragma once
#include <cstdint>

struct TypeInfo
{
	uint8_t type = 0;	// ドロップするアイテムの種類
	uint8_t seedSum = 0;	// 種類の数
};
