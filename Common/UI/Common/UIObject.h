//====================================================//
// ファイル名   : UIObject.h
// 作成者       : 高安瑞貴
//
// 概要 : UIが継承する基底クラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include <string>
#include "Common/Manager/Resources/ResourceManager.h"
#include "Lib/Camera.h"
#include "Tools/GameContext.h"

//====================================================//
// 宣言
//====================================================//

using namespace DirectX;

// UIが継承するクラス
class UIObject
{
public:

	// マウスと接触しているか判定
	virtual bool MouseOn();


public:

	// コンストラクタ
	UIObject();
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pos">表示位置</param>
	/// <param name="scale">拡大率</param>
	UIObject(SimpleMath::Vector3 pos, SimpleMath::Vector3 scale = SimpleMath::Vector3::One);

	// デストラクタ
	virtual ~UIObject() = default;

	// ---- 基本関数 ---- //
	virtual void Initialize(const GameContext& gameContext) = 0;
	virtual void Update(const GameContext& gameContext,const float& elapsedTime) = 0;
	virtual void Render(const GameContext& gameContext, const Camera& camera) = 0;
	virtual void RenderText() = 0;
	virtual void Finalize() {};

	// 位置設定
	virtual void SetPosition(SimpleMath::Vector3 pos) { m_worldPosition = pos; }
	// 位置取得
	virtual SimpleMath::Vector3 GetPosition() { return m_worldPosition; }
	// 拡大率の設定
	virtual void SetScale(SimpleMath::Vector3 scale) { m_scale = scale; }
	// スケールの取得
	virtual SimpleMath::Vector3 GetScale() { return m_scale; }
	// オフセットの追加
	void AddOffset(SimpleMath::Vector3 pos) { m_posOffset += pos; };
	// オフセットの設定
	void SetOffset(SimpleMath::Vector3 pos) { m_posOffset = pos; };
	// オフセットの取得
	SimpleMath::Vector3 GetOffset() { return m_posOffset; }
	// アクティブフラグの管理
	void SetActive(bool active) { m_isActive = active; }
	// アクティブかどうか
	bool IsActive() { return m_isActive; }


protected:

	// 描画順
	int m_orderLayer;

	// ワールド座標
	SimpleMath::Vector3 m_worldPosition;
	SimpleMath::Vector3 m_scale;

	// 位置のオフセット
	SimpleMath::Vector3 m_posOffset;

	// 親
	UIObject* m_parent;

	// 画像ハンドル
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ghHundle;

	// アクティブフラグ
	bool m_isActive;
};

