#include "pch.h"
#include "EditCamera.h"
#include "Common/Manager/InputManager.h"
#include "Message/InputGroup.h"

using namespace DirectX;

EditCamera::EditCamera(int windowWidth, int windowHeight, InputGroup* group)
    : Camera(windowWidth, windowHeight)
    , IInputObserver(group)
    , m_elapsedTime{ 0.0f }
    , m_lastWheelValue{}
{
    SetWindowSize(windowWidth, windowHeight);
    Mouse::Get().ResetScrollWheelValue();
}

EditCamera::~EditCamera()
{
    InputManager::GetInstance()->DeleteObserver(this);
}

void EditCamera::Update(float elapsedTime)
{
    m_elapsedTime = elapsedTime;
    EditCameraUpdate(Mouse::Get().GetState());
}

void EditCamera::Render()
{
    //Draw::GetInstance().DrawDot(m_chasePos, Colors::Blue);
}

void EditCamera::EditCameraUpdate(Mouse::State state)
{
    // 右ボタンが押されてる時だけ回転
    if (state.rightButton)
    {
        m_xAngle = m_xTmp;
        m_yAngle = m_yTmp;

        // ★修正：押し始めた瞬間は差分を取らない（巨大なdeltaを防ぐ）
        if (m_isFirstFrameOfRotation)
        {
            m_prevMouseX = state.x;
            m_prevMouseY = state.y;
            m_isFirstFrameOfRotation = false;
        }

        // ★修正：絶対座標ではなく「前フレームからの差分」を渡す
        int deltaX = state.x - m_prevMouseX;
        int deltaY = state.y - m_prevMouseY;

        Motion(deltaX, deltaY);

        m_prevMouseX = state.x;
        m_prevMouseY = state.y;
    }
    else
    {
        // ★修正：ボタンを離したらリセット（次に押した瞬間また差分0から始める）
        m_isFirstFrameOfRotation = true;
    }

    m_x = state.x;
    m_y = state.y;

    SimpleMath::Matrix rx = SimpleMath::Matrix::CreateRotationX(m_xTmp);
    SimpleMath::Matrix ry = SimpleMath::Matrix::CreateRotationY(m_yTmp * MOVE);
    SimpleMath::Matrix rt = ry;

    SimpleMath::Vector3 offset = SimpleMath::Vector3::Transform(DEFAULT_CAMERA_DISTANCE, rt.Invert());
    SimpleMath::Vector3 up(0.0f, 1.0f, 0.0f);

    switch (m_chase)
    {
    case EditCamera::ChasePatern::None:
        m_eye = offset;
        m_target = SimpleMath::Vector3::Zero;
        break;
    case EditCamera::ChasePatern::Object:
        m_eye = m_chasePos + offset;
        break;
    default:
        break;
    }

    m_target = m_chasePos;

    m_view = SimpleMath::Matrix::CreateLookAt(m_eye, m_chasePos, up);
}

void EditCamera::MoveVertical()
{
    auto state = Mouse::Get().GetState();

    Move(SimpleMath::Vector3(0, (state.scrollWheelValue - m_lastWheelValue) * WHEEL_SPEED, 0));

    m_lastWheelValue = state.scrollWheelValue;
}

void EditCamera::Move(SimpleMath::Vector3 vec)
{
    SimpleMath::Vector3 targetVelocity = vec * (m_isDash ? DASH_SPEED : SPEED);
    SimpleMath::Vector3 delta = targetVelocity * m_elapsedTime;

    m_eye += delta;
    // ★修正：注視点も一緒に動かす。これが無いと次のUpdateで
    //         m_eye = m_chasePos + offset に強制上書きされて移動が消える
    m_chasePos += delta;
}

bool EditCamera::OnInputAccept(Message::InputEvent input)
{
    switch (input)
    {
        // ---- 平行移動 ---- //
    case Message::InputEvent::MOVEUP:
        Move(GetForward());
        break;
    case Message::InputEvent::MOVEDOWN:
        Move(SimpleMath::Vector3::Transform(GetForward(), SimpleMath::Matrix::CreateRotationY(XM_PI)));
        break;
    case Message::InputEvent::MOVELEFT:
        Move(SimpleMath::Vector3::Transform(GetForward(), SimpleMath::Matrix::CreateRotationY(XM_PIDIV2)));
        break;
    case Message::InputEvent::MOVERIGHT:
        Move(SimpleMath::Vector3::Transform(GetForward(), SimpleMath::Matrix::CreateRotationY(-XM_PIDIV2)));
        break;
        // 垂直移動
    case Message::InputEvent::CAMERAVVERTICAL:
        MoveVertical();
        break;
        // 加速
    case Message::InputEvent::CAMERADASH:
        ReverseDash();
        break;
    default:
        break;
    }

    return false;
}