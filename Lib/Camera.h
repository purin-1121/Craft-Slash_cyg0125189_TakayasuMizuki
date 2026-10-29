#pragma once
#include "Common/Collider/CollisionManager.h" 

class Camera
{
public:

	// カメラの追従パターン
	enum class ChasePatern
	{
		None,	// 追従しない
		Pos,	// 位置だけ
		Angle,	// 角度だけ
		Object	// 位置も角度も
	};

public:

	Camera(int windowWidth, int windowHeight);

	void Update(float elapsedTime, Mouse::State mouseState, float deltaX, float deltaY);
	void Render();

	SimpleMath::Matrix GetCameraView() { return m_view; }
	float GetAngleX() { return m_xTmp + 90.0f; };
	float GetAngleY() { return m_yTmp * MOVE; };
	// カメラが向いている水平方向のベクトルを取得
	SimpleMath::Vector3 GetForward();
	void SetWindowSize(int windowWidth, int windowHeight);
	// 射影行列を作るために必要なパラメータを返す関数
	SimpleMath::Vector3 GetEye() { return m_eye; }
	SimpleMath::Vector3 GetTarget() { return m_target; }
	SimpleMath::Vector3 GetUp() { return m_up; }

	// 追従する場所とパターンを設定
	void SetChasePos(SimpleMath::Vector3 pos, ChasePatern chasePatern = ChasePatern::None) 
	{ m_chasePos = pos, m_chase = chasePatern, m_startPos = m_eye; }
	// 法線ベクトルの設定
	void SetNormalVec(SimpleMath::Vector3 normal) { m_chaseNormal = normal; }
	// カメラからレイを飛ばす
	LineCollider ScreenPointToRay(SimpleMath::Vector2 screenPos, SimpleMath::Matrix proj, float screenWidth, float screenHeight);


protected:

	// カメラの距離
	static const SimpleMath::Vector3 DEFAULT_CAMERA_DISTANCE;

	//　カメラの動く倍率
	static const float MOVE;

	// 横回転
	float m_yAngle, m_yTmp;

	// 縦回転
	float m_xAngle, m_xTmp;

	// ドラッグされた座標
	int m_x, m_y;
	float m_sx, m_sy;

	// マウスのホイール
	float m_scrollWheelValue;

	// ビュー行列作成用の変数
	SimpleMath::Vector3 m_eye;	// 視点
	SimpleMath::Vector3 m_target;	// 
	SimpleMath::Vector3 m_up;

	// マウストラッカー
	Mouse::ButtonStateTracker m_tracker;

	// ビュー行列
	SimpleMath::Matrix m_view;

	// 追従するオブジェクトの位置, 法線ベクトル
	SimpleMath::Vector3 m_chasePos;
	SimpleMath::Vector3 m_chaseNormal;

	// 追従パターン
	Camera::ChasePatern m_chase;

	// 追従し始めるまでの位置
	SimpleMath::Vector3 m_startPos;

protected:

	// 行列の生成
	void Motion(int x, int y);
	// ゲームプレイカメラ
	void GameCamera(Mouse::State state, float deltaX, float deltaY);

};

