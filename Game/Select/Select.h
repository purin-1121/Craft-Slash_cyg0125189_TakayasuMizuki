#pragma once

#include "Common/Scene/Scene.h"
#include "Common/UI/Canvas.h"
#include "Common/UI/Text.h"
#include "Common/UI/Button/Button.h"
#include "Message/InputGroup.h"

class Select : public Scene, public InputGroup
{

public:

	// カメラとオブジェクトの距離
	static constexpr float CAMERA_DISTANCE = 2.0f;
	// ボックスの大きさ
	static constexpr float BOX_SIZE = 2.0f;
	// 地面の大きさ
	static constexpr SimpleMath::Vector2 GROUND_SIZE{10, 10};
	// グリッド数(辺)
	static constexpr int GRID = 4;

private:

	// ゲームオブジェクト
	std::vector<GameObject*> m_gameObject;
	// スコアテキスト
	Text m_scoreText;
	// ベストスコア
	Text m_bestScoreText;
	// タイムテキスト
	Text m_timeText;
	// ベストタイム
	Text m_bestTimeText;
	
	// ----- デバッグ用 ----- //
	Canvas m_debugCanvas;


public:

	Select(const GameContext& gameContext);

	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render() override;
	void Finalize() override;
};

