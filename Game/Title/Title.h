#pragma once

#include "Common/Scene/Scene.h"
#include "Common/UI/Canvas.h"
#include "Common/UI/Text.h"
#include "Common/UI/Button/Button.h"
#include "Message/InputGroup.h"

class Title : public Scene, public InputGroup
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

	Title(const GameContext& gameContext);
	~Title();

	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render() override;
	void Finalize() override;

private:

	// ベストスコア
	Text m_bestScoreText;
	// ベストタイム
	Text m_bestTimeText;
	// タイトル
	Text m_title;
	// タイトルの縁取り
	Text m_titleOutLine;
	// 回転量
	float m_rotValue;
	// 主要なキャンバス
	Canvas m_mainCanvas;
	// オプション用キャンバス
	Canvas m_optionCanvas;
	// オプション開いているか
	bool m_isOption;
	// フレーム毎秒
	float m_elapsedTime;
	// メニュー移動時間
	static constexpr float MENU_MOVE_TIME = 0.5f;
	// メニュー移動先座標
	static constexpr SimpleMath::Vector3 MENU_CLOSE = { 1200, 0, 0 };
	static constexpr SimpleMath::Vector3 MENU_OPEN = { 900, 0, 0 };
	// BGMインスタンス
	std::unique_ptr<SoundEffectInstance> m_bgmInstance;
	// 音量バーの大きさ
	static constexpr SimpleMath::Vector2 VAR_SIZE = { 200, 25 };

	// オプション状態
	enum OptionState
	{
		None = -1,
		Value,
		Manual,
	};
	OptionState m_optionState;

	// ----- デバッグ用 ----- //

private:

	// オプションキャンバス作成
	void CreateOptionCanvas();


};

