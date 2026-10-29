#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <Tools/nlohmann/json.hpp>
#include <bitset>
#include <iostream>
#include "Common/Manager/StorageManager.h"
#include "Game/GamePlay/Item/InventryData.h"
#include "Game/GamePlay/Item/DropData.h"

using json = nlohmann::json;

class Drop;
class ItemManager;

// ドロップする内容を作るファクトリークラス
class DropFactory
{

public:

	TypeInfo* GetInfo(std::string key) { return &m_dropPool[key]; }

	// インスタンスを返す
	static DropFactory& GetInstance()
	{
		CreateInstance();
		return *m_dropFc;
	}

	// インスタンスを作成
	static void CreateInstance()
	{
		if (!m_dropFc)
		{
			m_dropFc.reset(new DropFactory);
		}
	}

	// インスタンスを破棄
	static void DisposeInstance()
	{
		m_dropMethod = nullptr;
		if (m_dropFc)
		{
			m_dropFc.reset();
		}
	}

	~DropFactory(){};

	// アイテムのドロップメソッド
	static inline std::function<void(GameObject*, const Drop*, int, int)> GetDropMethod() { return m_dropMethod; };
	static void CreateDropMethod(ItemManager* item);

private:

	// ドロップアイテム情報プール
	std::unordered_map<std::string, TypeInfo> m_dropPool;
	// ドロップメソッド
	static std::function<void(GameObject*, const Drop*, int, int)> m_dropMethod;

	static std::unique_ptr<DropFactory> m_dropFc;

private:
	DropFactory() { LoadDropData(); };

	// ドロップの情報を読み込む
	void LoadDropData();
	
};


