//====================================================//
// ファイル名   : Scene.h
// 作成者       : 高安瑞貴
//
// 概要 : シーンの基底クラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include <iostream>
#include <string>
#include <tchar.h> 
#include <atlstr.h>
#include "Common/Manager/storageManager.h"
#include "Lib/Camera.h"
#include "Tools/GameContext.h"

//====================================================//
// 宣言
//====================================================//

class SceneManager;
class storageManager;

class Scene
{


public:

	// フェードをする時間
	static constexpr float FADE_TIME = 0.5f;

	// フェードするときの回数
	static constexpr int FADE_CYCLE = 50;

protected:

	// マネージャへの参照
	SceneManager* m_pSceneManager;
	std::string m_next;	// 次に移動するシーンの名前

	// 各シーンのカメラ
	std::unique_ptr<Camera> m_gameCamera;

	// コンテキスト
	const GameContext& m_gameContext;

	// ------- フェード用 ------ //

	float m_fadeTime;	// フェードの時間

	// フェード開始のフラグ
	bool m_fadeIn;
	bool m_fadeOut;

	bool m_requestSave;	// 写真保存のリクエスト

	// ----------------------- //

public:

	// コンストラクタ
	Scene(const GameContext& gameContext);
	// デストラクタ
	virtual ~Scene();

	// 基本関数
	virtual void Initialize() = 0;
	virtual void Update(float elapsedTime) = 0;
	virtual void Render() = 0;
	virtual void Finalize() = 0;

	// マネージャを取得
	SceneManager* GetSceneManager() { return m_pSceneManager; }

protected:

	// シーン切り替え
	void ChangeScene(const std::string& nextSceneName);
	// シーン切り替えの準備
	void SetMovingScene(std::string name);
	// フェード命令
	void StartFade(std::wstring name);
};