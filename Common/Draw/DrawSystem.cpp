#include "pch.h"
#include "DrawSystem.h"
#include "Game/GamePlay/GameObject/GameObject.h"
#include "Tools/GameContext.h"
#include "Lib/Camera.h"

std::unique_ptr<DrawSystem> DrawSystem::s_drawSystem = nullptr;

void DrawSystem::Render(const GameContext& gameContext, Camera & camera)
{
	auto view = camera.GetCameraView();
	for (auto& obj : m_objects)
	{
		if (obj->IsActive())
		{
			obj->Render(gameContext, view);
		}
	};
}

void DrawSystem::RenderText()
{
	for (auto& obj : m_objects)
	{
		if (obj->IsActive())
		{
			obj->RenderText();
		}
	};
}

void DrawSystem::UnRegisterObject(GameObject* object)
{
	auto it = std::find(m_objects.begin(), m_objects.end(), object);
	if (it != m_objects.end())
	{
		m_objects.erase(it);
	}
}

void DrawSystem::SortDepth(Camera& camera)
{
	// 深度順に並び替えのロジックを実装
	std::sort(m_objects.begin(), m_objects.end(), [&](GameObject* a, GameObject* b) {
		return (camera.GetEye() - a->GetPosition()).LengthSquared() > (camera.GetEye() - b->GetPosition()).LengthSquared();
		});
}