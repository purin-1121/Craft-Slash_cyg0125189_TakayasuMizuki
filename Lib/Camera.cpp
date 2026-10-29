#include "pch.h"
#include "Camera.h"

using namespace DirectX;

// 注視点とカメラとの最短の距離
const SimpleMath::Vector3 Camera::DEFAULT_CAMERA_DISTANCE = SimpleMath::Vector3(0, 1.6f, 4.0f);
const float Camera::MOVE = 5.0f;

Camera::Camera(int windowWidth, int windowHeight)
	: m_yAngle(0.0f), m_yTmp(0.0f), m_xAngle(0.0f), m_xTmp(0.0f), 
	m_x(0), m_y(0), m_eye{}, m_target{},m_up{0, 1, 0},
	m_chase(ChasePatern::None), m_chaseNormal(0.0f, 1.0f, 0.0f), m_chasePos{}
{
	SetWindowSize(windowWidth, windowHeight);
	// マウスのフォイール値をリセット
	Mouse::Get().ResetScrollWheelValue();
}

void Camera::Update(float elapsedTime, Mouse::State mouseState, float deltaX, float deltaY)
{
	auto keystate = Keyboard::Get().GetState();

	m_tracker.Update(mouseState);
	
	GameCamera(mouseState, deltaX, deltaY);
}

void Camera::Render()
{
	Draw::GetInstance().DrawDot(m_chasePos, Colors::Blue);
}

void Camera::Motion(int x, int y)
{
	// ここに来る x は必ず「前フレームからの移動量(delta)」であること。
	// 絶対座標を渡すと、押しっぱなしのたびに角度が際限なく加算され続けて発散する。
	float dx = (float)x * m_sx;

	if (dx != 0.0f)
	{
		m_yTmp += dx * DirectX::XM_PI;
	}
}

void Camera::GameCamera(Mouse::State state, float deltaX, float deltaY)
{
	m_xAngle = m_xTmp;
	m_yAngle = m_yTmp;

	// ★ クリック判定なし！ 手動で計算した移動量をそのまま渡す
	Motion(deltaX, deltaY);

	m_scrollWheelValue = state.scrollWheelValue;

	// ★ X軸を完全に無視し、Y軸（左右）の行列だけを作成
	SimpleMath::Matrix ry = SimpleMath::Matrix::CreateRotationY(m_yTmp * MOVE);
	SimpleMath::Matrix rt = ry;

	SimpleMath::Vector3 offset = SimpleMath::Vector3::Transform(DEFAULT_CAMERA_DISTANCE, rt.Invert());
	SimpleMath::Vector3 up(0.0f, 1.0f, 0.0f);

	// デフォルトの距離を回転させる（オフセットの作成）

	// カメラの追従パターンによってパラメータを制御
	switch (m_chase)
	{
	case Camera::ChasePatern::None:
		m_eye = offset;
		m_target = SimpleMath::Vector3::Zero;
		break;
	case Camera::ChasePatern::Pos:
		break;
	case Camera::ChasePatern::Angle:
		break;
	case Camera::ChasePatern::Object:
		m_eye = m_chasePos + offset;
		break;
	default:
		break;
	}

	m_target = m_chasePos;

	// 今の視点を作成
	m_view = SimpleMath::Matrix::CreateLookAt(m_eye, m_chasePos, up);
}

SimpleMath::Vector3 Camera::GetForward()
{
	// Y軸回転（左右）のみを考慮した行列を作成
    SimpleMath::Matrix ry = SimpleMath::Matrix::CreateRotationY(-m_yTmp * MOVE);

    return SimpleMath::Vector3::TransformNormal(SimpleMath::Vector3(0, 0, -1), ry);
}

void Camera::SetWindowSize(int windowWidth, int windowHeight)
{
	// 画面サイズに対する相対的なスケールに調整
	m_sx = 1.0f / float(windowWidth);
	m_sy = 1.0f / float(windowHeight);
}

// 画面の2D座標(マウス位置)を渡すと、3Dのレイ(Ray)を返してくれる関数
LineCollider Camera::ScreenPointToRay(SimpleMath::Vector2 screenPos, SimpleMath::Matrix proj, float screenWidth, float screenHeight)
{
    // ★ 引数の proj (UIで上書きされて壊れている可能性大) を無視し、
    // ここで確実な3D用の透視投影(Perspective)行列を作り直す！
    SimpleMath::Matrix trueProj = 
		SimpleMath::Matrix::CreatePerspectiveFieldOfView(
			DirectX::XMConvertToRadians(45.0f), // 一般的な視野角(FOV)
			screenWidth / screenHeight,         // アスペクト比
			0.1f,                               // ニアクリップ
			1000.0f                             // ファークリップ
		);

    // ビューポート（画面のサイズ情報）を作成
    RECT rect = { 0, 0, (long)screenWidth, (long)screenHeight };
    SimpleMath::Viewport viewport(rect);

    // マウス位置の「一番手前（Z=0）」と「一番奥（Z=1）」の3D座標を逆算(Unproject)する
    SimpleMath::Vector3 sourceNear(screenPos.x, screenPos.y, 0.0f);
    SimpleMath::Vector3 sourceFar(screenPos.x, screenPos.y, 1.0f);
    
    // ★ 偽物の proj ではなく、上で作った trueProj を使って逆算する
    SimpleMath::Vector3 nearPoint = viewport.Unproject(sourceNear, trueProj, m_view, SimpleMath::Matrix::Identity);
    SimpleMath::Vector3 farPoint  = viewport.Unproject(sourceFar,  trueProj, m_view, SimpleMath::Matrix::Identity);

    // レイの向き（手前から奥へのベクトル）を計算して長さを1にする
    SimpleMath::Vector3 direction = farPoint - nearPoint;
    direction.Normalize();

    // 始点と、そこから伸びるベクトル(サイズ)で構成される LineCollider として返す
    return LineCollider(nearPoint, direction * 1000.0f);
}


