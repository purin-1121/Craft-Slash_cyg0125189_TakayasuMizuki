
#include "pch.h"
#include "Scene.h"
#include "SceneManager.h"

// コンストラクタ
Scene::Scene(const GameContext& gameContext)
	: m_pSceneManager{SceneManager::GetInstanse()}
	, m_fadeIn{ false }
	, m_fadeOut{ false }
	, m_fadeTime{ 0.0f }
	, m_requestSave{ false }
	, m_next{"none"}
	, m_gameContext{gameContext}
{

}

// デストラクタ
Scene::~Scene()
{

}

// シーンの変更
void Scene::ChangeScene(const std::string& nextSceneName)
{
	m_pSceneManager->RequestSceneChange(nextSceneName);
}

void Scene::SetMovingScene(std::string name)
{
	m_requestSave = true;
	m_next = name;
}

void Scene::StartFade(std::wstring name)
{
	m_fadeIn = true;
}
