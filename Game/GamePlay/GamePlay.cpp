#include "pch.h"
#include "GamePlay.h"
#include "Common/Collider/CollisionManager.h"
#include "Common/Draw/DrawSystem.h"
#include "Common/Model/ObjectManager.h"
#include "Common/Manager/DataManager.h"
#include "Common/Manager/InputManager.h"
#include "Common/Scene/SceneManager.h"
#include "Common/UI/Button/Button.h"
#include "Common/UI/Button/FunctionButton.h"
#include "Common/UI/Bar/Bar.h"
#include "Common/UI/Image.h"
#include "Common/UI/Common/UIHelper.h"
#include "Tools/GameContext.h"
#include "Message/EventBus.h"
#include "Message/AnnouncementManager.h"
#include "Croutine/Croutine.h"
#include "Factory/FadeFactory.h"

using namespace DirectX;

GamePlay::GamePlay(const GameContext& gameContext)
    : Scene(gameContext)
    , InputGroup(InputManager::GetInstance())
    , m_scoreText{ L"現在のスコア:0", {20, 70} }
    , m_timeText{ L"", {20, 20} }
    , m_gameObjectManager{this}
    , m_gamePlayCanvas{}
    , m_PauseCanvas{true, this}
    , m_elapsedTime{}
    , m_editManager{this}
    , m_isPause{false}
    , m_rendState{RendState::All}
    , m_gameState{GameState::Normal}
    , m_multiElapsed{1.0f}
    , m_pauseSubscriptionID{}
    , m_editSubscriptionID{}
    , m_rendSubscriptionID{}
    , m_inventrySubscriptionID{}
{
    // ★ 相対モードをやめて、常に絶対座標モードにする！
    Mouse::Get().SetMode(Mouse::MODE_ABSOLUTE);
}

GamePlay::~GamePlay()
{
    // 自身を削除
    m_parent->DeleteObserver(this);
    // マウスを解放
    Mouse::Get().SetMode(Mouse::MODE_ABSOLUTE);
}

void GamePlay::Initialize()
{   
    // ---- カメラの作成 ---- //
    m_gameCamera = std::make_unique<Camera>(DataManager::GetInstance()->ScreenX, 720);
    m_gameCamera->SetWindowSize(DataManager::GetInstance()->ScreenX, 720);

    // ---- データのリセット ---- //
    DataManager::GetInstance()->Reset();

    // ---- 外部ファイルの読み込み ---- //
    EventBus::GetInstance()->Publish(Message::EventID::RELOAD, nullptr);

    // ---- GameObjectの設定 ---- //
    m_gameObjectManager.Initialize(m_gamePlayCanvas, m_gameContext);

    // ---- キャンバスの作成 ---- //
    CreateGamePlayCanvas();
    CreatePauseCanvas();
    CreateClearCanvas();
    CreateFailCanvas();

    // ---- インベントリの作成 ---- //
    m_inventryMenu = std::make_unique<InventryMenu>(m_gameObjectManager.GetPlayer(), this);
    m_inventryMenu->Initialize(m_gameContext);

    AnnouncementManager::GetInstance()->Initialize(m_gameContext);

    // ---- イベントの作成 ---- //
    SetEvent();

    // ---- 描画設定 ---- //
   
    auto renderingContext = Draw::GetInstance().GetRenderingContext();

    // 行列の設定 
    renderingContext.effect.SetProjection(renderingContext.proj);
    renderingContext.effect.SetView(SimpleMath::Matrix::Identity); // とりあえずアイデンティティ（原点向き）
    
    auto context = m_gameContext.deviceResources.GetD3DDeviceContext();

    // パイプラインステートの設定
    context->IASetInputLayout(renderingContext.inputLayout.Get()); // 作成したレイアウトをセット
    
    auto sampler = m_gameContext.commonStates.LinearClamp();
    context->PSSetSamplers(0, 1, &sampler);
    context->OMSetBlendState(m_gameContext.commonStates.Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_gameContext.commonStates.DepthDefault(), 0);
    
    ShowCursor(FALSE);
    SetCursorPos(DataManager::GetInstance()->CenterX, DataManager::GetInstance()->CenterY);

    m_gameState = GameState::Normal;

}

void GamePlay::Update(float elapsedTime)
{
    auto mouseState = Mouse::Get().GetState();


    m_PauseCanvas.Update(m_gameContext, elapsedTime);

    InputManager::GetInstance()->Update(m_gameContext.keyboardTracker, m_gameContext.mouseButtonTracker);

    switch (m_gameState)
    {
    case GameState::Inventry:
        m_inventryMenu->Update(elapsedTime, m_gameContext);
        break;
    case GameState::Edit:
        m_editManager.Update(m_gameContext, elapsedTime);
        return;
    case GameState::Clear:
        m_clearCanvas.Update(m_gameContext, elapsedTime);
        break;
    case GameState::Fail:
        m_failCanvas.Update(m_gameContext, elapsedTime);
        break;
    case GameState::Pause:
        break;
    default:
        POINT pt;
        GetCursorPos(&pt);
        int centerX = DataManager::GetInstance()->CenterX;
        int centerY = DataManager::GetInstance()->CenterY;

        // 画面中央からの差分（これが純粋なマウス移動量になる）
        int deltaX = pt.x - centerX;
        int deltaY = pt.y - centerY;

        // 無限に回転できるよう、次のフレームのためにマウスを中央に戻す！
        SetCursorPos(centerX, centerY);

        // カメラの更新に、差分(deltaX, deltaY)を渡す
        m_gameCamera->Update(elapsedTime, mouseState, deltaX, deltaY);
        // オブジェクトの更新
        m_gameObjectManager.Update(elapsedTime * m_multiElapsed, m_gameContext, *m_gameCamera);
        // 物理制御機構の更新
        PhysicsUtility::GetInstance()->PhysicsUpdate();
        // UIの更新
        m_gamePlayCanvas.Update(m_gameContext, elapsedTime);
        // アナウンスの更新
        AnnouncementManager::GetInstance()->Update(elapsedTime);

        // 経過時間の加算
        DataManager::GetInstance()->time += elapsedTime;
        std::wstring str = L"現在のタイム:";
        std::wstringstream wss;
        wss << std::fixed << std::setprecision(1) << DataManager::GetInstance()->time;
        m_timeText.SetText(str + wss.str());
        break;
    }    

    DrawSystem::GetInstance()->SortDepth(*m_gameCamera);
}

void GamePlay::Render()
{
    auto context = m_gameContext.deviceResources.GetD3DDeviceContext();
    auto renderingContext = Draw::GetInstance().GetRenderingContext();
    auto camera = (m_gameState == GameState::Edit ? m_editManager.GetCamera() : m_gameCamera.get());

    // ----------------------------------------------------
    // 射影行列の設定
    // ----------------------------------------------------
    renderingContext.effect.SetView(camera->GetCameraView());

    // 射影行列を作成
    renderingContext.proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
        DirectX::XM_PIDIV4,       // 視野角 (45度)
        DataManager::GetInstance()->ScreenX / DataManager::GetInstance()->ScreenY,         // アスペクト比
        0.01f,                    // ニアクリップ
        1000.0f                   // ファークリップ
    );
    renderingContext.effect.SetProjection(renderingContext.proj);

    // エフェクトの適用
    renderingContext.effect.Apply(context);

    // ----------------------------------------------------
    // 半透明描画処理の設定
    // ----------------------------------------------------


    // ----------------------------------------------------
    // モデルの描画
    // ----------------------------------------------------
    // PrimitiveBatchは3Dモデルと競合するため囲まない



    if (m_gameState == GameState::Edit)
    {
        // 入力レイアウトを線画用にセット
        context->IASetInputLayout(renderingContext.inputLayout.Get());

        // 重要】3Dカメラではなく、画面のピクセルサイズに合わせた「2D用の行列（Orthographic）」を作る
        SimpleMath::Matrix view = SimpleMath::Matrix::Identity;
        SimpleMath::Matrix proj = SimpleMath::Matrix::CreateOrthographicOffCenter(
            0.0f, (float)DataManager::GetInstance()->ScreenX, (float)DataManager::GetInstance()->ScreenY, 0.0f, 0.0f, 1.0f); // 画面サイズ(DataManager::GetInstance()->ScreenXx720)に合わせる

        // エフェクトに2D用の行列をセットして適用
        renderingContext.effect.SetWorld(SimpleMath::Matrix::Identity);
        renderingContext.effect.SetView(view);
        renderingContext.effect.SetProjection(proj);
        renderingContext.effect.SetVertexColorEnabled(true);
        renderingContext.effect.Apply(context);

        // 深度テストを無効化
        context->OMSetDepthStencilState(m_gameContext.commonStates.DepthNone(), 0);
        context->OMSetBlendState(m_gameContext.commonStates.Opaque(), nullptr, 0xFFFFFFFF);

        // 描画
        renderingContext.primitiveBatch.Begin();
        m_editManager.Render(m_gameContext);
        renderingContext.primitiveBatch.End();
    }

    // ----------------------------------------------------
    // デバッグ用描画（ワイヤーフレーム等）
    // ----------------------------------------------------
    // 入力レイアウトをセットし直す
    context->IASetInputLayout(renderingContext.inputLayout.Get());

    // エフェクトの再設定と適用
    renderingContext.effect.SetWorld(SimpleMath::Matrix::Identity);
    renderingContext.effect.SetView(camera->GetCameraView());
    renderingContext.effect.SetProjection(renderingContext.proj);
    renderingContext.effect.SetVertexColorEnabled(true);
    renderingContext.effect.Apply(context);

    // ステートを再度適用
    context->OMSetBlendState(m_gameContext.commonStates.Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_gameContext.commonStates.DepthDefault(), 0);
    context->RSSetState(m_gameContext.commonStates.CullCounterClockwise());

    
  

    renderingContext.primitiveBatch.Begin();

    if (m_rendState & RendState::Collision)
    {
        CollisionManager::GetInstance()->RenderCollider();
        camera->Render();
        m_gameObjectManager.GetSpawnManager().RenderEdit();
    }
    if (m_gameState == GameState::Edit)
    {
        m_gameObjectManager.GetSpawnManager().RenderEdit();
        m_gameObjectManager.GetHeightMap().EditRender(m_gameContext, m_gameObjectManager.GetPlayer().GetPosition(), 50, camera->GetCameraView());
    }
    else
    {
        m_gameObjectManager.GetHeightMap().Render(m_gameContext, camera->GetCameraView());
    }

      // オブジェクトの描画
    if (m_rendState & RendState::Object)
    {
        DrawSystem::GetInstance()->Render(m_gameContext, *camera);
    }


    renderingContext.primitiveBatch.End();

    if(m_gameState == GameState::Edit)
        m_editManager.RenderPreview(m_gameContext);

    // ----------------------------------------------------
    // 2D（UI・テキスト）の描画
    // ----------------------------------------------------
    Draw::GetInstance().SpriteBegin();

    switch (m_gameState)
    {
    case GameState::Edit:
        m_editManager.RenderText(m_gameContext);
        break;
    case GameState::Clear:
        m_clearCanvas.RenderText();
        break;
    case GameState::Fail:
        m_failCanvas.RenderText();
        break;
    case GameState::Inventry:
        m_inventryMenu->Render();
        break;
    default:
        m_gameObjectManager.RenderText();
        m_gamePlayCanvas.RenderText();
        m_scoreText.RenderText();
        m_timeText.RenderText();
        m_PauseCanvas.RenderText();
        AnnouncementManager::GetInstance()->RenderText();
        break;
    }
    Draw::GetInstance().SpriteEnd();
}

void GamePlay::Finalize()
{
    EventBus::GetInstance()->UnSubscribe(Message::EventID::PAUSE, m_pauseSubscriptionID);
    EventBus::GetInstance()->UnSubscribe(Message::EventID::EDIT, m_editSubscriptionID);
    EventBus::GetInstance()->UnSubscribe(Message::EventID::RENDMODE, m_rendSubscriptionID);
    EventBus::GetInstance()->UnSubscribe(Message::EventID::INVENTRY, m_inventrySubscriptionID);
    EventBus::GetInstance()->UnSubscribe(Message::EventID::BOSSDEFEATED, m_clearSubscriptionID);
    EventBus::GetInstance()->UnSubscribe(Message::EventID::FAIL, m_failSubscriptionID);

    m_gamePlayCanvas.Finalize();
    m_PauseCanvas.Finalize();
    m_clearCanvas.Finalize();
    m_gameObjectManager.Finalize();
    m_inventryMenu->Finalize();

    CollisionManager::GetInstance()->Reset();
    DropFactory::DisposeInstance();
}

bool GamePlay::OnInputAccept(Message::InputEvent input)
{


    switch (m_gameState)
    {
    case GameState::Normal:
        m_gameObjectManager.OnInputAccept(input);
        break;
    case GameState::Inventry:
        m_inventryMenu->OnInputAccept(input);
        break;
    case GameState::Pause:
        m_PauseCanvas.OnInputAccept(input);
        break;
    case GameState::Edit:
        m_editManager.OnInputAccept(input);
        break;
    case GameState::Debug:
        m_gameObjectManager.OnInputAccept(input);
        break;
    case GameState::Clear:
        m_clearCanvas.OnInputAccept(input);
        break;
    case GameState::Fail:
        m_failCanvas.OnInputAccept(input);
        break;
    default:
        break;
    }

    return false;
}

void GamePlay::CreateGamePlayCanvas()
{
    m_gamePlayCanvas.CreateUI(
        "hp_bar", 
        new Bar<int>(m_gameObjectManager.GetPlayer().GetHP(),
           SimpleMath::Vector3{(float)DataManager::GetInstance()->ScreenX - 200, 30, 0}, 
            170.0f, 
            50.0f, 
            Colors::Green)
    );

    // ---- 操作説明 ---- //
    m_gamePlayCanvas.CreateUI(
        "WASD",
        new Text(
            L"[W,A,S,D] : 移動",
            SimpleMath::Vector2(1000, 680)
        )
    );
    m_gamePlayCanvas.CreateUI(
        "WASD",
        new Text(
           L"[Space] : ジャンプ",
            SimpleMath::Vector2(1000, 640)
        )
    );
    m_gamePlayCanvas.CreateUI(
        "WASD",
        new Text(
            L"[右クリック] : 攻撃",
            SimpleMath::Vector2(0, 680)
        )
    );
    m_gamePlayCanvas.CreateUI(
        "WASD",
        new Text(
            L"[マウスドラッグ] : 視点移動",
            SimpleMath::Vector2(0, 640)
        )
    );
    m_gamePlayCanvas.Initialize(m_gameContext);
    
}

void GamePlay::CreatePauseCanvas()
{

    // Escapeを押したらポーズ画面を上から表示する
    m_PauseCanvas.SetPosition({ 0, -720, 0 });
    m_pauseTargetPos = { 0, -720, 0 };
    m_PauseCanvas.AddFunc("Move", [&, this]() {
        UIAnimation::InsertPos(m_PauseCanvas, m_pauseTargetPos, 0.1f);
     }
    );

     m_PauseCanvas.CreateUI(
        "BackImage",
        new Image(
            SimpleMath::Vector2::Zero,
            SimpleMath::Color(0.1f,0.1f,0.1f,0.3f),
            SimpleMath::Vector2{
                static_cast<float>(DataManager::GetInstance()->ScreenX), 
                static_cast<float>(DataManager::GetInstance()->ScreenY) 
            },
            L"Resources/Textures/white.png"
        )
    );

     m_PauseCanvas.CreateUI(
        "Replay", 
         new FunctionButton(
             []() { SceneManager::GetInstanse()->RequestSceneChange("GamePlay"); },
             SimpleMath::Vector3{ 200, 400, 0 },
             L"Replay",
             &m_PauseCanvas,
             SimpleMath::Vector3{ 400, 100, 1 }
         )
    );
     m_PauseCanvas.CreateUI(
         "Title",
         new FunctionButton(
             []() { SceneManager::GetInstanse()->RequestSceneChange("Title"); },
             SimpleMath::Vector3{ 200, 200, 0 },
             L"Title",
             &m_PauseCanvas,
             SimpleMath::Vector3{ 400, 100, 1 }
         )
     );
     m_PauseCanvas.CreateUI(
         "Exit", 
         new FunctionButton(
             []() {
                 PostQuitMessage(0);
             },
             SimpleMath::Vector3{ 680, 400, 0 },
             L"Exit",
             &m_PauseCanvas,
             SimpleMath::Vector3{ 400, 100, 1 }
         )
     );
     dynamic_cast<IInputObserver*>(m_PauseCanvas.SammonUI("Replay"))->SetLayer(1);
     dynamic_cast<IInputObserver*>(m_PauseCanvas.SammonUI("Exit"))->SetLayer(1);
     dynamic_cast<IInputObserver*>(m_PauseCanvas.SammonUI("Title"))->SetLayer(1);

     m_PauseCanvas.Initialize(m_gameContext);
}

void GamePlay::CreateClearCanvas()
{
    m_clearCanvas.SetPosition({ 0, -720, 0 });
    m_clearTargetPos = { 0, -720, 0 };
    m_clearCanvas.AddFunc("Move", [&, this]() {
        UIAnimation::InsertPos(m_clearCanvas, m_clearTargetPos, 0.1f);
        }
    );

    m_clearCanvas.CreateUI(
        "BackImage",
        new Image(
            SimpleMath::Vector2{
                static_cast<float>(DataManager::GetInstance()->ScreenX / 8),
                static_cast<float>(DataManager::GetInstance()->ScreenY / 8)
            },
            SimpleMath::Color(0.1f, 0.1f, 0.1f, 0.7f),
            SimpleMath::Vector2{
                static_cast<float>(DataManager::GetInstance()->ScreenX / 4 * 3),
                static_cast<float>(DataManager::GetInstance()->ScreenY / 4 * 3)
            },
            L"Resources/Textures/white.png"
        )
    );

    m_clearCanvas.CreateUI(
        "Title",
        new FunctionButton(
            []() { SceneManager::GetInstanse()->RequestSceneChange("Title"); },
            SimpleMath::Vector3{ 200, 400, 0 },
            L"Title",
            &m_clearCanvas,
            SimpleMath::Vector3{ 400, 100, 1 }
        )
    );
    m_clearCanvas.CreateUI(
        "Exit",
        new FunctionButton(
            []() {
                PostQuitMessage(0);
            },
            SimpleMath::Vector3{ 680, 400, 0 },
            L"Exit",
            &m_clearCanvas,
            SimpleMath::Vector3{ 400, 100, 1 }
        )
    );

    dynamic_cast<IInputObserver*>(m_clearCanvas.SammonUI("Exit"))->SetLayer(1);
    dynamic_cast<IInputObserver*>(m_clearCanvas.SammonUI("Title"))->SetLayer(1);

    m_clearCanvas.CreateUI(
        "ClearTime",
        new Text(
            L"",
            SimpleMath::Vector2{420, 230}
        )
    );

    m_clearCanvas.CreateUI(
        "BestTime",
        new Text(
            L"",
            SimpleMath::Vector2{ 420, 280 }
        )
    );

    m_clearCanvas.Initialize(m_gameContext);
}

void GamePlay::CreateFailCanvas()
{
    m_failCanvas.SetPosition({ 0, -720, 0 });
    m_failTargetPos = { 0, -720, 0 };
    m_failCanvas.AddFunc("Move", [&, this]() {
        UIAnimation::InsertPos(m_failCanvas, m_failTargetPos, 0.1f);
        }
    );

    m_failCanvas.CreateUI(
        "BackImage",
        new Image(
            SimpleMath::Vector2{
                static_cast<float>(DataManager::GetInstance()->ScreenX / 8),
                static_cast<float>(DataManager::GetInstance()->ScreenY / 8)
            },
            SimpleMath::Color(0.1f, 0.1f, 0.1f, 0.7f),
            SimpleMath::Vector2{
                static_cast<float>(DataManager::GetInstance()->ScreenX / 4 * 3),
                static_cast<float>(DataManager::GetInstance()->ScreenY / 4 * 3)
            },
            L"Resources/Textures/white.png"
        )
    );

    m_failCanvas.CreateUI(
        "Title",
        new FunctionButton(
            []() { SceneManager::GetInstanse()->RequestSceneChange("Title"); },
            SimpleMath::Vector3{ 200, 400, 0 },
            L"Title",
            &m_failCanvas,
            SimpleMath::Vector3{ 400, 100, 1 }
        )
    );

    m_failCanvas.CreateUI(
        "Replay",
        new FunctionButton(
            [&]() { 
                SceneManager::GetInstanse()->RequestSceneChange("GamePlay"); 
            },
            SimpleMath::Vector3{ 200, 250, 0 },
            L"Replay",
            &m_failCanvas,
            SimpleMath::Vector3{ 400, 100, 1 }
        )
    );

    m_failCanvas.CreateUI(
        "Exit",
        new FunctionButton(
            []() {
                PostQuitMessage(0);
            },
            SimpleMath::Vector3{ 680, 400, 0 },
            L"Exit",
            &m_failCanvas,
            SimpleMath::Vector3{ 400, 100, 1 }
        )
    );

    static_cast<Text*>(m_failCanvas.CreateUI(
            "FailText",
            new Text(
                L"ゲーム失敗...",
                SimpleMath::Vector2{ 0, 180 }
            )
        )
        )->SetCenter({ DataManager::GetInstance()->CenterX, 180 });

    dynamic_cast<IInputObserver*>(m_failCanvas.SammonUI("Exit"))->SetLayer(1);
    dynamic_cast<IInputObserver*>(m_failCanvas.SammonUI("Replay"))->SetLayer(1);
    dynamic_cast<IInputObserver*>(m_failCanvas.SammonUI("Title"))->SetLayer(1);

    m_failCanvas.Initialize(m_gameContext);
}

void GamePlay::SetEvent()
{
    // ポーズイベントの作成
    auto pause = [&](void* Messager) {
            m_isPause = !m_isPause;
            // ポーズ中じゃなかったら
            if (m_gameState != GameState::Pause && m_gameState != GameState::Edit) {
                m_gameState = GameState::Pause;
                m_pauseTargetPos = SimpleMath::Vector3::Zero;    // 画面内にスライドイン
                // カーソルの解放
                ShowCursor(TRUE);
                SetCursorPos(DataManager::GetInstance()->CenterX, DataManager::GetInstance()->CenterY); // カーソル位置を中心に
            }
            // ポーズ中だったら
            else {
                // 通常に
                m_gameState = GameState::Normal;
                // ポーズ画面を移動
                m_pauseTargetPos = SimpleMath::Vector3(0, -DataManager::GetInstance()->ScreenY, 0); // 画面外へスライドアウト
                ShowCursor(FALSE);
                SetCursorPos(DataManager::GetInstance()->CenterX, DataManager::GetInstance()->CenterY);
            }
        };
    m_pauseSubscriptionID = EventBus::GetInstance()->Subscribe(Message::EventID::PAUSE, pause);

    // エディットモードの開始
    auto edit = [&](void* Messager)
        {
            if (m_gameState != GameState::Edit)
            {
                m_gameState = GameState::Edit;
                m_editManager.EditOn(&m_gameObjectManager.GetHeightMap(), m_gameCamera.get(), m_gameContext);
                // カーソルの解放
                ShowCursor(TRUE);
                SetCursorPos(DataManager::GetInstance()->CenterX, DataManager::GetInstance()->CenterY); // カーソル位置を中心に
            }
            else if(Messager == nullptr)
            {
                m_gameState = GameState::Normal;
                if (!m_editManager.EditOff())
                {
                    EventBus::GetInstance()->Publish(Message::EventID::EDIT, nullptr);
                }
                ShowCursor(FALSE);
                SetCursorPos(DataManager::GetInstance()->CenterX, DataManager::GetInstance()->CenterY);
            }
        };
    m_editSubscriptionID = EventBus::GetInstance()->Subscribe(Message::EventID::EDIT , edit);


    // 描画モード変更
    auto rendmode =
        [&](void* Messager) {
        ++m_rendState;
        };
    m_rendSubscriptionID = EventBus::GetInstance()->Subscribe(Message::EventID::RENDMODE, rendmode);

    // インベントリオープン
    auto openInventry =
        [&](void* Messager)
        {
            if (m_gameState == GameState::Inventry)
            {
                m_gameState = GameState::Normal;
                ShowCursor(FALSE);
            }
            else if(m_gameState != GameState::Edit)
            {
                m_gameState = GameState::Inventry;
                ShowCursor(TRUE);
            }
        };
    m_inventrySubscriptionID = EventBus::GetInstance()->Subscribe(Message::EventID::INVENTRY, openInventry);

    // クリアイベント
    auto clear =
        [&](void* Messager)
        {
            m_gameState = GameState::Clear;
            m_clearTargetPos = SimpleMath::Vector3::Zero;

            // クリアタイム表示
            std::wstringstream wss;
            wss << std::fixed << std::setprecision(1) << DataManager::GetInstance()->time;
            auto clearTime = static_cast<Text*>(m_clearCanvas.SammonUI("ClearTime"));
            clearTime->SetText(L"今回のクリアタイム  " + wss.str());
            clearTime->SetCenter({ DataManager::GetInstance()->CenterX, clearTime->GetPosition().y});

            // ベストタイム表示
            std::wstringstream wss2;
            wss2 << std::fixed << std::setprecision(1) << DataManager::GetInstance()->bestTime;
            auto best = static_cast<Text*>(m_clearCanvas.SammonUI("BestTime"));
            best->SetText(L"ベストタイム            " + wss2.str());
            best->SetCenter({ DataManager::GetInstance()->CenterX, best->GetPosition().y});
            
            // 記録の更新
            DataManager::GetInstance()->CheckAndSaveBesttime();
            DataManager::GetInstance()->CheckAndSaveHighscore();


            ShowCursor(TRUE);
        };

    m_clearSubscriptionID = EventBus::GetInstance()->Subscribe(Message::EventID::BOSSDEFEATED, clear);

    auto fail =
        [&](void* Messager)
        {
            m_gameState = GameState::Fail;
            m_failTargetPos = SimpleMath::Vector3::Zero;

            ShowCursor(TRUE);
        };

    m_failSubscriptionID = EventBus::GetInstance()->Subscribe(Message::EventID::FAIL, fail);
}

