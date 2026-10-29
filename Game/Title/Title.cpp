#include "pch.h"
#include "Title.h"
#include "Common/Model/ObjectManager.h"
#include "Common/Collider/CollisionManager.h"
#include "Common/Manager/InputManager.h"
#include "Common/Scene/SceneManager.h"
#include "Tools/GameContext.h"
#include "Common/UI/Button/FunctionButton.h"
#include "Common/UI/Bar/Slider.h"
#include "Common/UI/Image.h"
#include "Common/UI/Common/UIHelper.h"
#include <Croutine/Croutine.h>

using namespace DirectX;

Title::Title(const GameContext& gameContext)
    : Scene(gameContext)
    , InputGroup(InputManager::GetInstance())
    , m_mainCanvas{}
    , m_optionCanvas{}
    , m_bestScoreText{ L"" , {770, 270} }
    , m_bestTimeText{ L"", {270, 270}}
    , m_title{ L"Craft & Slash", {DataManager::GetInstance()->ScreenX / 2 - DataManager::GetInstance()->Font_Size * 2, 150} }
    , m_titleOutLine{ L"Craft & Slash", {DataManager::GetInstance()->ScreenX / 2 - DataManager::GetInstance()->Font_Size * 2, 150}, 1.1f}
    , m_rotValue{}
    , m_isOption{false}
    , m_bgmInstance{nullptr}
    , m_optionState{OptionState::None}
    , m_elapsedTime{}
{
    // ---- カメラの作成 ---- //
    m_gameCamera = std::make_unique<Camera>(DataManager::GetInstance()->ScreenX, 720);
    m_gameCamera->SetWindowSize(DataManager::GetInstance()->ScreenX, 720);

    // タイトルを中心へ
    std::wstring textStr = m_title.GetText();
    XMVECTOR sizeVec = Draw::GetInstance().GetRenderingContext().spriteFont.MeasureString(textStr.c_str());
    SimpleMath::Vector2 textSize(XMVectorGetX(sizeVec), XMVectorGetY(sizeVec));

    SimpleMath::Vector2 textCenter = SimpleMath::Vector2{ DataManager::GetInstance()->Center.x, 200 } - textSize / 2;
    m_title.SetPosition({ textCenter });
    m_title.SetInitPosition({ textCenter });

    m_titleOutLine;
}

Title::~Title()
{
    m_parent->DeleteObserver(this);
}

void Title::Initialize()
{
    // ---- キャンバスの作成 ---- //
    // プレイボタンの作成
    m_mainCanvas.CreateUI(
        "Play",
        new FunctionButton(
            []() {SceneManager::GetInstanse()->RequestSceneChange("GamePlay"); },
            SimpleMath::Vector3{ 200, 400, 0 },
            L"Play",
            this,
            SimpleMath::Vector3{ 400, 100, 1 }
        )
    );
    // 終了ボタンの作成
    m_mainCanvas.CreateUI(
        "Exit",
        new FunctionButton(
            []() { PostQuitMessage(0); },
            SimpleMath::Vector3{ 680, 400, 0 },
            L"Exit",
            this,
            SimpleMath::Vector3{ 400, 100, 1 }
        )
    );

    

    m_mainCanvas.Initialize(m_gameContext);

    CreateOptionCanvas();
}

void Title::Update(float elapsedTime)
{

    // カメラの更新
    //m_gameCamera->Update(elapsedTime);

    m_rotValue += 10.0f * elapsedTime;
    // キャンバスの更新
    m_mainCanvas.Update(m_gameContext, elapsedTime);
    m_optionCanvas.Update(m_gameContext, elapsedTime);

    // フレーム毎秒の更新
    m_elapsedTime = elapsedTime;

    // 入力マネージャの更新
    InputManager::GetInstance()->Update(m_gameContext.keyboardTracker, m_gameContext.mouseButtonTracker);
}

void Title::Render()
{

    auto renderingContext = Draw::GetInstance().GetRenderingContext();

    // 行列の設定 
    renderingContext.effect.SetProjection(renderingContext.proj);
    renderingContext.effect.SetView(SimpleMath::Matrix::Identity); // とりあえずアイデンティティ（原点向き）
    
    auto context = m_gameContext.deviceResources.GetD3DDeviceContext();

    // 1. 通常のビュー行列を取得
    SimpleMath::Matrix view = m_gameCamera->GetCameraView();

    // 2. 【超重要ハック】プロジェクション行列の方に回転を掛け算する！
    // ※これにより、SkyboxEffectの「お節介な自動計算」を完全にスルーして背景を回せます。
    SimpleMath::Matrix skyboxProj =
        SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(m_rotValue)) * renderingContext.proj;

    // 3. エフェクトに適用（ワールドはIdentity、ビューはそのまま渡すのでカメラ位置もバグりません）
    Draw::GetInstance().SetSkyboxEffect(
        SimpleMath::Matrix::Identity,
        view,
        skyboxProj // ← 回転済みのプロジェクション行列を渡す！
    );

    Draw::GetInstance().DrawSkyBox(
        context,
        &m_gameContext.commonStates);

    // パイプラインステートの設定
    context->IASetInputLayout(renderingContext.inputLayout.Get()); // 作成したレイアウトをセット
    
    auto sampler = m_gameContext.commonStates.LinearClamp();
    context->PSSetSamplers(0, 1, &sampler);
    context->OMSetBlendState(m_gameContext.commonStates.Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_gameContext.commonStates.DepthDefault(), 0);

    // --- テスト用にカメラ行列を直接作成 ---
    renderingContext.effect.SetView(m_gameCamera->GetCameraView());
    // エフェクトの適用
    renderingContext.effect.Apply(context);

    Draw::GetInstance().PrimitibBegin();
    m_mainCanvas.Render(m_gameContext, *m_gameCamera);
    Draw::GetInstance().PrimitibEnd();

    Draw::GetInstance().SpriteBegin();

    // UIの描画
    m_mainCanvas.RenderText();
    m_optionCanvas.RenderText();

    // テキストの描画
    m_bestScoreText.RenderText();
    m_bestTimeText.RenderText();
    m_title.RenderText();
    //m_titleOutLine.RenderText();

    Draw::GetInstance().SpriteEnd();
}

void Title::Finalize()
{
    m_mainCanvas.Finalize();
}

void Title::CreateOptionCanvas()
{
    // キャンバス移動
    m_optionCanvas.SetPosition(MENU_CLOSE);
    // 背景作成
    m_optionCanvas.CreateUI("Back",
        new Image(
            SimpleMath::Vector2{ 80, 0 },      // 位置
            SimpleMath::Color{ 0, 0, 0, 1.0f }, // 描画色
            SimpleMath::Vector2{ 300, 720 }     // 大きさ//
        ));

    // サウンドマネージャ取得
    auto sound = ResourceManager::GetInstance().GetSoundManager();
    // マスターボリューム調整スライダーの作成
    m_optionCanvas.CreateUI("MasterControl",
        new Slider(
            sound->GetMasterValue(),
            SimpleMath::Vector3{ 100, 230, 1 },
            VAR_SIZE.x,VAR_SIZE.y,
            Colors::Red,
            sound->MAX_MASTER_VALUE,sound->MIN_MASTER_VALUE,sound->DEFAULT_VALUE));

    // BGMボリューム調整スライダーの作成
    m_optionCanvas.CreateUI("BGMControl",
        new Slider(
            sound->GetBGMValue(),
            SimpleMath::Vector3{ 100, 300, 1 },
            VAR_SIZE.x,VAR_SIZE.y,
            Colors::Red,
            sound->MAX_BGM_VALUE,sound->MIN_BGM_VALUE,sound->DEFAULT_VALUE));

    // ---- ボタンの作成 ---- //
    {
        // オプションボタンの作成
        m_optionCanvas.CreateUI(
            "Option",
            new FunctionButton(
                [&]() {
                    m_isOption = !m_isOption;
                    float time = 0; // UI移動時間
                    SimpleMath::Vector3 targetPos = (m_isOption ? MENU_OPEN : MENU_CLOSE);    // 移動先座標
                    m_optionState = (m_isOption ? OptionState::Value : OptionState::Value);
                    
                    // 説明書を全非アクティブに
                    m_optionCanvas.SammonUI("FirstManual")->SetActive(false);
                    m_optionCanvas.SammonUI("SecondManual")->SetActive(false);
                    m_optionCanvas.SammonUI("ThirdManual")->SetActive(false);

                    Croutine::GetRefarence().StartCroutine(
                        [&, time, targetPos]() mutable
                        {
                            if (time < MENU_MOVE_TIME)
                            {
                                UIAnimation::InsertPos(m_optionCanvas, targetPos, time, MENU_MOVE_TIME);
                                time += m_elapsedTime;
                                return IEmurator::Null;
                            }
                            else
                                return IEmurator::End;
                        });
                },
                SimpleMath::Vector3{ 0, 0, 0 }, L"三", this, SimpleMath::Vector3{ 80, 80, 1 }));

        // マニュアル表示ボタンの作成
        m_optionCanvas.CreateUI(
            "ManualButton",
            new FunctionButton(
                [&]() {
                    bool isManulal = m_optionState == OptionState::Manual;
                    m_optionState = (isManulal ? OptionState::None : OptionState::Manual);
                    m_optionCanvas.SammonUI("FirstManual")->SetActive(!isManulal);
                    m_optionCanvas.SammonUI("SecondManual")->SetActive(!isManulal);
                    m_optionCanvas.SammonUI("ThirdManual")->SetActive(!isManulal);
                },
                SimpleMath::Vector3{ 100, 420, 0 }, L"説明", this, SimpleMath::Vector3{ 200, 100, 1 })
        );
        dynamic_cast<IInputObserver*>(m_optionCanvas.SammonUI("ManualButton"))->SetLayer(1);

        // マニュアル移動ボタンの作成
    }

    // ---- 説明画面の設定 ---- //
    {
        m_optionCanvas.CreateUI(
            "FirstManual",
            new Image(SimpleMath::Vector2(-400, 240), SimpleMath::Color{1.0f, 1.0f , 1.0f ,1.0f }, SimpleMath::Vector2(200, 200), Image::DEFAULT_IMAGE, false));
        m_optionCanvas.CreateUI(
            "SecondManual",
            new Image(SimpleMath::Vector2(-400, 240), SimpleMath::Color{ 1.0f, 0.0f , 0.0f ,1.0f }, SimpleMath::Vector2(200, 200), Image::DEFAULT_IMAGE, false));
        m_optionCanvas.CreateUI(
            "ThirdManual",
            new Image(SimpleMath::Vector2(-400, 240), SimpleMath::Color{ 0.0f, 0.0f , 1.0f ,1.0f }, SimpleMath::Vector2(200, 200), Image::DEFAULT_IMAGE, false));
    }


    m_optionCanvas.Initialize(m_gameContext);
}

