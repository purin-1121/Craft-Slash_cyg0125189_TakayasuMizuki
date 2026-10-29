//--------------------------------------------------------------------------------------
// File: GameContext.h
//
// ゲームで使用する機能をまとめる構造体
//--------------------------------------------------------------------------------------
#pragma once

#include <Keyboard.h>
#include <Mouse.h>
#include "Common/StepTimer.h"
#include "CommonStates.h"
#include "Common/DeviceResources.h"
#include "SimpleMath.h"

// 各シーンに渡す共通リソースを記述してください
struct GameContext
{
	// ステップタイマー
	DX::StepTimer& timer;
	// デバイスリソース
	DX::DeviceResources& deviceResources;
	// キーボードステートトラッカー
	Keyboard::KeyboardStateTracker& keyboardTracker;
	// マウスステートトラッカー
	Mouse::ButtonStateTracker& mouseButtonTracker;
	// コモンステート
	CommonStates& commonStates;

	// コンストラクタ
	GameContext(
		DX::StepTimer& timer, 
		DX::DeviceResources& device, 
		Keyboard::KeyboardStateTracker& keyboradTracker,
		Mouse::ButtonStateTracker& mouseTracker,
		CommonStates& commonState
	)
		: timer{timer}
		, deviceResources{device}
		, keyboardTracker{keyboradTracker}
		, mouseButtonTracker{mouseTracker}
		, commonStates{commonState}
	{
	
	};
};
// 描画用ゲームコンテキスト
struct RenderingContext
{
	// ---- カメラ行列関係 ---- //
	// 射影行列
	SimpleMath::Matrix& proj;

	// ---- シェーダー・リソース関係 ---- //
	// ベーシックエフェクト
	BasicEffect& effect;
	// インプットレイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout>& inputLayout;
	// スプライトバッチ
	SpriteBatch& spriteBatch;
	// スプライトフォント
	SpriteFont& spriteFont;
	// プリミティブバッチ
	PrimitiveBatch<VertexPositionColor>& primitiveBatch;

	// コンストラクタ
	RenderingContext(
		SimpleMath::Matrix& proj,
		BasicEffect& effect,
		Microsoft::WRL::ComPtr<ID3D11InputLayout>& inputLayout,
		SpriteBatch& spriteBatch,
		SpriteFont& spriteFont,
		PrimitiveBatch<VertexPositionColor>& primitiveBatch
		)
		: proj{proj}
		, effect{effect}
		, inputLayout{inputLayout}
		, spriteBatch{spriteBatch}
		, spriteFont{spriteFont}
		, primitiveBatch{primitiveBatch}
	{};
};