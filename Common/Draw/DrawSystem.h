//====================================================//
// ファイル名   : DrawSystem.h
// 作成者       : 高安瑞貴
//
// 概要 : 物理挙動を管理する名前空間
//====================================================//

#pragma once

#include <memory>
#include <vector>

//====================================================//
// インクルードファイル
//====================================================//

//====================================================//
// 宣言
//====================================================//
struct GameContext;
class GameObject;
class Camera;

class DrawSystem
{

public:

	void Render(const GameContext& gameContext, Camera & camera);
	void RenderText();

	
	// 深度順に並び替え
	void SortDepth(Camera& camera);

	// オブジェクトの登録
	void RegisterObject(GameObject* object) { m_objects.push_back(object); };
	// オブジェクトの解除
	void UnRegisterObject(GameObject* object);
	// オブジェクトの登録
	std::vector<GameObject*>& GetObjects() { return m_objects; }

	~DrawSystem() = default;

	static DrawSystem* GetInstance()
	{
		if (s_drawSystem == nullptr)
		{
			s_drawSystem.reset(new DrawSystem);
		}

		return s_drawSystem.get();
	}

	static DrawSystem& GetRefarence()
	{
		if (s_drawSystem == nullptr)
		{
			s_drawSystem.reset(new DrawSystem);
		}

		return *s_drawSystem.get();
	}

	static void Dispose()
	{
		s_drawSystem.reset();
	}


private:

	std::vector<GameObject*> m_objects;	// 実行するオブジェクト

	static std::unique_ptr<DrawSystem> s_drawSystem;

	DrawSystem() :m_objects{} {};

	
};