#pragma once
#include "Common/Collider/CollisionManager.h" 
#include "InterFace/IInputObserver.h"
#include "Camera.h"

class EditCamera : public Camera, public IInputObserver
{

public:

	EditCamera(int windowWidth, int windowHeight, InputGroup* group);
	~EditCamera();

	void Update(float elapsedTime);
	void Render();

	// カメラの移動
	void Move(SimpleMath::Vector3 vec);

	bool OnInputAccept(Message::InputEvent input) override;
	void ReverseDash() { m_isDash = !m_isDash; };	// ダッシュモードの反転

private:

	float m_elapsedTime;
	bool m_isDash;

	// 右クリック回転用の「前フレームのマウス座標」
	int m_prevMouseX = 0;
	int m_prevMouseY = 0;
	// 右クリックを押し始めた最初のフレームかどうか
	bool m_isFirstFrameOfRotation = true;

	// 前回のホイールの値
	float m_lastWheelValue;

	// カメラの移動速度
	static constexpr float DASH_SPEED = 3.0f;
	static constexpr float SPEED = 1.0f;
	static constexpr float WHEEL_SPEED = 0.01f;

private:

	// カメラ更新
	void EditCameraUpdate(Mouse::State state);
	// 垂直移動
	void MoveVertical();

};