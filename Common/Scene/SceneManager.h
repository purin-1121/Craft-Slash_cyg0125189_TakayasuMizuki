//====================================================//
// ファイル名   : SceneManager.h
// 作成者       : 高安瑞貴
//
// 概要 : 各シーンの制御を行うクラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include <memory>
#include <string>
#include <unordered_map>
#include "Tools/GameContext.h"

//====================================================//
// 宣言
//====================================================//



// 前方参照
class Scene;

// シーンマネージャークラス
class SceneManager
{

private:

	// Sceneを作る関数
    using SceneFactory = std::function<std::unique_ptr<Scene>(GameContext& gameContext)>;
    std::unordered_map<std::string, SceneFactory> m_factories;

    // 切り替え時に破棄
    std::unique_ptr<Scene> m_pCurrentScene;
    
    // 次に作りたいシーン名
    std::string m_requestedSceneName;

	// エイリアス宣言
	using SceneCollection = std::unordered_map<std::string, std::unique_ptr<Scene>>;

	// シーン群
	SceneCollection m_scenes;

	// 自身へのポインタ
	static SceneManager* s_sceneManager;

private:

	// コンストラクタ
	SceneManager();

	// デストラクタ
	~SceneManager() ;

public:

	// インスタンスを取得
	static SceneManager* GetInstanse()
	{
		if (s_sceneManager == nullptr)
			s_sceneManager = new SceneManager();
		return s_sceneManager;
	}

	// シーンの登録
    template<typename T>
    void RegisterScene(const std::string& name, const GameContext& gameContext) {
        m_factories[name] = [&](const GameContext& gameContext) { return std::make_unique<T>(gameContext); };
    }
	// 初期化処理
	void Initialize();

	// 更新処理
	void Update(float elapsedTime, GameContext& gameContext, RenderingContext& renderingContext);

	// 描画処理
	void Render();

	// 終了処理
	void Finalize();

	// 開始シーンの設定
	void SetStartScene(const std::string& startSceneName);

	// シーンの変更
	void RequestSceneChange(const std::string& requestSceneName);

	// 各シーンのデバイス依存のリソース作成
	void EachSceneCreateResources(ID3D11Device1* device, EffectFactory& fx);

private:

	// シーンの変更
	void ChangeScene(GameContext&gameContext);

};

