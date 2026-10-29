#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <Tools/nlohmann/json.hpp>
#include <bitset>
#include <iostream>
#include "Common/Manager/StorageManager.h"
#include <Common/UI/Image.h>
using json = nlohmann::json;

// ドロップする内容を作るファクトリークラス
class FadeFactory
{

public:
	// インスタンスを返す
	static FadeFactory& GetInstance()
	{
		CreateInstance();
		return *m_fadeFactory;
	}

	// インスタンスを作成
	static void CreateInstance()
	{
		if (!m_fadeFactory)
		{
			m_fadeFactory = new FadeFactory();
		}
	}

	void FadeUpdate(float elapsedTime);
	std::function<void()> SetFade(float timer, bool isIn, float elapsedTime);


private:

	static FadeFactory* m_fadeFactory;
	float m_value;
	float m_raito;
	float m_currnet;
	float m_timer;
	bool m_isIn;

	Image m_fade;

private:
	FadeFactory() {};
	~FadeFactory() {};

};


