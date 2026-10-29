#include "pch.h"

#include "SceneManager.h"
#include "Scene.h"

#include "Tools/GameContext.h"

// 実態宣言
SceneManager* SceneManager::s_sceneManager = nullptr;

// コンストラクタ
SceneManager::SceneManager()
	: m_scenes{}
	, m_pCurrentScene{ nullptr }
{
}

// デストラクタ
SceneManager::~SceneManager()
{
	// 実行中のシーンの終了処理
	if (m_pCurrentScene) m_pCurrentScene->Finalize();

	m_pCurrentScene = nullptr;

	m_scenes.clear();

}

void SceneManager::RequestSceneChange(const std::string& name) {
    m_requestedSceneName = name;
}

void SceneManager::Initialize()
{
	if (m_pCurrentScene) m_pCurrentScene->Initialize();
}

void SceneManager::Update(float elapsedTime, GameContext&gameContext, RenderingContext& renderingContext){
    // シーン切り替え要求があったら
    if (!m_requestedSceneName.empty()) {
        // 今のシーンを終了して破棄
        if (m_pCurrentScene) m_pCurrentScene->Finalize();
        
        // シーンを作成
        m_pCurrentScene = m_factories[m_requestedSceneName](gameContext);
        
        // 初期化
        m_pCurrentScene->Initialize();
        
        m_requestedSceneName.clear();
    }

    if (m_pCurrentScene) m_pCurrentScene->Update(elapsedTime);
}

// 描画処理
void SceneManager::Render()
{
	// シーンの描画処理
	if (m_pCurrentScene) m_pCurrentScene->Render();
}

void SceneManager::Finalize()
{
	m_pCurrentScene = nullptr;
	m_scenes.clear();
}

// 開始シーンの設定
void SceneManager::SetStartScene(const std::string& startSceneName)
{
	RequestSceneChange(startSceneName);
}


void SceneManager::EachSceneCreateResources(ID3D11Device1* device, EffectFactory& fx)
{

}

