#include "pch.h"
#include "Select.h"
#include "Common/Model/ObjectManager.h"
#include "Common/Collider/CollisionManager.h"
#include "Common/Scene/SceneManager.h"
#include "Tools/GameContext.h"
#include "Common/UI/Button/FunctionButton.h"
#include "Common/UI/Bar/Bar.h"
#include "Common/Manager/InputManager.h"

using namespace DirectX;
using namespace SimpleMath;

Select::Select(const GameContext& gameContext)
    : Scene(gameContext)
    , InputGroup(InputManager::GetInstance())
    , m_debugCanvas{}
    , m_scoreText{ L"" ,{270,250} }
    , m_timeText{ L"", {270, 300}}
    , m_bestScoreText{ L"" , {770, 250} }
    , m_bestTimeText{ L"", {770, 300}}
{
    // ---- カメラの作成 ---- //
    m_gameCamera = std::make_unique<Camera>(DataManager::GetInstance()->ScreenX, 720);
    m_gameCamera->SetWindowSize(DataManager::GetInstance()->ScreenX, 720);
}

void Select::Initialize()
{
    // ---- キャンバスの作成 ---- //
    m_debugCanvas.CreateUI(
        "ToTitle", 
        new FunctionButton(
            []() {SceneManager::GetInstanse()->RequestSceneChange("Title");}, 
           SimpleMath::Vector3{ 440, 400, 0 }, 
            L"タイトルヘ", 
            this,
           SimpleMath::Vector3{ 400, 100, 1 }
        )
    );

    // キャンバスの初期化
    m_debugCanvas.Initialize(m_gameContext);

    // ---- テキストの設定 ---- //
    m_timeText.SetText((std::wstringstream() << L"今回のタイム:" << std::fixed << std::setprecision(1) << DataManager::GetInstance()->time).str());
    m_scoreText.SetText(L"今回のスコア:" + std::to_wstring(DataManager::GetInstance()->score));
    m_bestScoreText.SetText(L"ベストスコア:" + std::to_wstring(DataManager::GetInstance()->bestScore));
    m_bestTimeText.SetText((std::wstringstream() << L"ベストタイム:" << std::fixed << std::setprecision(1) << DataManager::GetInstance()->bestTime).str());
}

void Select::Update(float elapsedTime)
{

    // カメラの更新
    m_gameCamera->Update(elapsedTime, Mouse::Get().GetState(), 0, 0);

    // キャンバスの更新
    m_debugCanvas.Update(m_gameContext, elapsedTime);
}

void Select::Render()
{
    m_gameContext.deviceResources.PIXEndEvent();

    auto renderindContext = Draw::GetInstance().GetRenderingContext();

    // 行列の設定 
    renderindContext.effect.SetProjection(renderindContext.proj);
    renderindContext.effect.SetView(SimpleMath::Matrix::Identity); // とりあえずアイデンティティ（原点向き）
    
    auto context = m_gameContext.deviceResources.GetD3DDeviceContext();

    // パイプラインステートの設定
    context->IASetInputLayout(renderindContext.inputLayout.Get()); // 作成したレイアウトをセット
    
    auto sampler = m_gameContext.commonStates.LinearClamp();
    context->PSSetSamplers(0, 1, &sampler);
    context->OMSetBlendState(m_gameContext.commonStates.Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_gameContext.commonStates.DepthDefault(), 0);

    // --- テスト用にカメラ行列を直接作成 ---
    renderindContext.effect.SetView(m_gameCamera->GetCameraView());
    // エフェクトの適用
    renderindContext.effect.Apply(context);
    renderindContext.primitiveBatch.Begin();
    m_debugCanvas.Render(m_gameContext, *m_gameCamera);
    renderindContext.primitiveBatch.End();
    renderindContext.spriteBatch.Begin();

    // UIの描画
    m_debugCanvas.RenderText();

    // テキストの描画
    m_scoreText.RenderText();
    m_timeText.RenderText();
    m_bestScoreText.RenderText();
    m_bestTimeText.RenderText();

    renderindContext.spriteBatch.End();
}

void Select::Finalize()
{
    m_debugCanvas.Finalize();
}

