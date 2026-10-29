//====================================================//
// ファイル名   : SoundManager.h
// 作成者       : 高安瑞貴
//
// 概要 : サウンドを管理するクラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include <memory>
#include <string>
#include <tchar.h> 
#include <unordered_map>
#include "Audio.h"

//====================================================//
// 宣言
//====================================================//

class SoundManager
{

public:

	// 初期化
	void Initialize() {};

	/// <summary>
	/// 指定された音の再生(未保存なら保存して再生)
	/// </summary>
	/// <param name="path">キー</param>
	// 効果音用：戻り値を void にして、その場で再生する
	void PlayOneShot(const std::wstring& path) {
		// すでに読み込み済みなら再生
		if (m_sound.count(path) > 0) {
			m_sound[path]->Play();
			return;
		}

		// 未読ならロードして再生
		std::unique_ptr<SoundEffect> s = std::make_unique<SoundEffect>(m_audEngine.get(), path.c_str());
		s->Play();
		m_sound.emplace(path, std::move(s));
	}

	// 制御用：unique_ptr をそのまま返す（所有権の譲渡）
	std::unique_ptr<DirectX::SoundEffectInstance> CreateSoundInstance(const std::wstring& path) {
		if (m_sound.count(path) > 0) {
			return m_sound[path]->CreateInstance();
		}

		// 未読ならロードしてからインスタンスを作る
		std::unique_ptr<SoundEffect> s = std::make_unique<SoundEffect>(m_audEngine.get(), path.c_str());
		auto instance = s->CreateInstance();
		m_sound.emplace(path, std::move(s));

		return instance; // 所有権を安全に呼び出し元へ渡す
	}

	// マスター音量取得
	int& GetMasterValue() { return m_masterValue; }
	// BGM音量取得
	int& GetBGMValue() { return m_bgmValue; }

	SoundManager()
		: m_audEngine{std::make_unique<AudioEngine>()}
		, m_sound{}
		, m_masterValue{}
		, m_bgmValue{}
	{};
	~SoundManager() = default;

	void LoadSound(const std::wstring& path, const std::wstring& key)
	{
		// 未読なら保存して再生
		std::unique_ptr<SoundEffect> s = std::make_unique<SoundEffect>(m_audEngine.get(), path.c_str());
		m_sound.emplace(key, std::move(s));
	}

	// マスター音量の最大最小
	static constexpr int MAX_MASTER_VALUE = 100;
	static constexpr int MIN_MASTER_VALUE = 20;
	// BGM音量の最大最小
	static constexpr int MAX_BGM_VALUE = 100;
	static constexpr int MIN_BGM_VALUE = 20;
	// マスターとBGMのデフォルト音量
	static constexpr int DEFAULT_VALUE = 50;

private:

	// 再生エンジン
	std::unique_ptr<AudioEngine> m_audEngine;
	// サウンドマップ
	std::unordered_map<std::wstring, std::unique_ptr<SoundEffect>> m_sound;
	// マスター音量
	int m_masterValue;
	// BGM音量
	int m_bgmValue;


};

