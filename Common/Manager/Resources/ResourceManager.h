//====================================================//
// ファイル名   : ResourcesManager.h
// 作成者       : 高安瑞貴
//
// 概要 : このゲームで扱う素材を管理するクラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include<memory>
#include "pch.h"
#include <string>
#include <tchar.h> 
#include <unordered_map>
#include "SoundManager.h"
#include "ImageManager.h"

//====================================================//
// 宣言
//====================================================//

class ResourceManager
{

public:

	// 初期化
	void Initialize(ID3D11Device* device);

    // 画像取得
	ID3D11ShaderResourceView* GetTexture(const std::wstring_view& name) {
        return m_imageManager->GetTexture(m_device, name.data());
    }

    // サウンドマネージャー取得
	SoundManager* GetSoundManager() { return m_soundManager.get(); }

    	// インスタンス処理
public:

	// インスタンスを返す
	static ResourceManager& GetInstance()
	{
		CreateInstance();
		return *m_ResourceManager;
	}

	// インスタンスを作成
	static void CreateInstance()
	{
		if (!m_ResourceManager)
		{
			m_ResourceManager = new ResourceManager();
		}
	}

	

private:

	// コンストラクタ・デストラクタ
	ResourceManager();
	~ResourceManager();

	// 各リソースを管理するクラス
	std::unique_ptr<ImageManager> m_imageManager;
    std::unique_ptr<SoundManager> m_soundManager;
	// デバイス
    ID3D11Device* m_device;

	static ResourceManager* m_ResourceManager;
};

