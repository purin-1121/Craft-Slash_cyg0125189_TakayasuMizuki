#pragma once

#include "Common/Scene/Scene.h"
#include "Common/UI/Canvas.h"
#include "Common/UI/Text.h"
#include "Factory/ObjectFactory.h"
#include "Game\GamePlay\GameObject\GameObjectManager.h"
#include "Game/GamePlay/Maps/EditManager.h"
#include "Item/InventryMenu.h"
#include "Message/EventBus.h"

enum class GameState : uint16_t
{
	Normal = 0,
	Inventry = 1 << 0,
	Pause = 1 << 1,
	Edit = 1 << 2,
	Clear = 1 << 3,
	Fail = 1 << 4,
	Debug = 1 << 5,
};


class GamePlay : public Scene, public InputGroup
{

public:

	// カメラとオブジェクトの距離
	static constexpr float CAMERA_DISTANCE = 2.0f;

public:

	GamePlay(const GameContext& gameContext);
	~GamePlay();

	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render() override;
	void Finalize() override;

	bool CanPlayed() {
		return m_gameState != GameState::Pause &&
			m_gameState != GameState::Inventry &&
			m_gameState != GameState::Edit &&
			m_gameState != GameState::Clear;
	}

	bool OnInputAccept(Message::InputEvent input) override;

private:

	// ---- イベントID ---- //
	// ポーズ
	SubsprictionID m_pauseSubscriptionID;
	// エディット
	SubsprictionID m_editSubscriptionID;
	// 描画
	SubsprictionID m_rendSubscriptionID;
	// インベントリ
	SubsprictionID m_inventrySubscriptionID;
	// クリア
	SubsprictionID m_clearSubscriptionID;
	// 失敗
	SubsprictionID m_failSubscriptionID;


	// ----- テキスト ----- //
	// スコアテキスト
	Text m_scoreText;
	// タイムテキスト
	Text m_timeText;

	// ゲームオブジェクトマネージャ
	GameObjectManager m_gameObjectManager;
	// エディットマネージャ
	EditManager m_editManager;
	
	// ----- キャンバス ----- //
	Canvas m_gamePlayCanvas;
	Canvas m_PauseCanvas;
	Canvas m_clearCanvas;
	Canvas m_failCanvas;

	// ポーズキャンバスの位置
	SimpleMath::Vector3 m_pauseTargetPos;
	// クリアキャンバスの位置
	SimpleMath::Vector3 m_clearTargetPos;
	SimpleMath::Vector3 m_failTargetPos;

	// ポーズ中
	bool m_isPause;

	// 描画状態
	RendState m_rendState;

	// ゲーム状態
	GameState m_gameState;

	// インベントリ
	std::unique_ptr<InventryMenu> m_inventryMenu;

	// ゲームの速度倍率
	float m_multiElapsed;
	// フレーム速度
	float m_elapsedTime;

	// ---- キャンバスの設定 ---- //
	void CreateGamePlayCanvas();
	void CreatePauseCanvas();
	void CreateClearCanvas();
	void CreateFailCanvas();

	// ---- イベントの設定 ---- //
	void SetEvent();
};

