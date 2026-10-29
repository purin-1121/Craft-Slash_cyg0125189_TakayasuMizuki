//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "GamePlay/GamePlay.h"
#include "Common/Model/ObjectManager.h"
#include "Common/Manager/DataManager.h"
#include "Common/Draw/ShaderManager.h"
#include "Common/Draw/ShadowRenderer.h"
#include "Result/Result.h"
#include "Title/Title.h"
#include "Common/Manager/StorageManager.h"
#include "Croutine/Croutine.h"


extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
    : m_gameContext{}
    , m_renderingContext{}
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    //m_gamePlay = std::make_unique<GamePlay>();
    // TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
    //   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
    //   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // 入力トラッカーの作成
    m_keyboardTracker = std::make_unique<Keyboard::KeyboardStateTracker>();
    m_mouseTracker = std::make_unique<Mouse::ButtonStateTracker>();

    // ゲームコンテキスト作成
    m_gameContext = std::make_unique<GameContext>(
        m_timer,
        *m_deviceResources,
        *m_keyboardTracker,
        *m_mouseTracker,
        *m_states
    );

    // ---- シングルトンクラスの初期化 ---- //
    Random::Initialize();
    DataManager::GetInstance();
    StorageManager::GetInstance();
    Draw::CreateInstance();
    ResourceManager::GetInstance().Initialize(m_deviceResources->GetD3DDevice());
    Croutine::GetInstace();
    ShaderManager::GetInstance()->CreateResources(m_gameContext->deviceResources.GetD3DDevice());
    ShadowRenderer::GetInstance()->Initialize(m_gameContext->deviceResources.GetD3DDevice());
    
    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

    

    Draw::GetInstance();
    Draw::GetInstance().Initielize(m_gameContext->deviceResources.GetD3DDeviceContext());
    Draw::GetInstance().SetRenderingContext(
        m_proj,
        *m_effect,
        m_inputLayout,
        *m_spriteBatch,
        *m_spriteFont,
        *m_batch
    );

    auto sm = SceneManager::GetInstanse();
    sm->RegisterScene<Title>("Title", *m_gameContext.get());
    sm->RegisterScene<GamePlay>("GamePlay", *m_gameContext.get());
    sm->RegisterScene<Result>("Result", *m_gameContext.get());
    sm->SetStartScene("GamePlay");

    sm->Initialize();
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.

    // キー入力を取得
    auto key = Keyboard::Get().GetState();
    auto mouse = Mouse::Get().GetState();


    m_keyboardTracker->Update(key);
    m_mouseTracker->Update(mouse);

    bool isPlayed = Croutine::GetRefarence().Update(elapsedTime);
    SceneManager::GetInstanse()->Update(elapsedTime, *m_gameContext, *m_renderingContext);

}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");

    // TODO: Add your rendering code here.

    SceneManager::GetInstanse()->Render();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    const auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    //SceneManager::GetInstanse()->Finalize();

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    const auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange()
{
    m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = (int)DataManager::GetInstance()->ScreenX;
    height = (int)DataManager::GetInstance()->ScreenY;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    // コモンステートの作成
    m_states = std::make_unique<CommonStates>(device);

    // TODO: Initialize device dependent ObjectLib here (independent of window size).
    EffectFactory fx(device);
    fx.SetDirectory(L"Resources/Models");

    // モデルの読み込み
    ObjectLib::CreateResources(device, fx);
    ObjectLib::m_animatedModels;

    // プリミティブバッチの作成
    m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

    // エフェクトの作成
    m_effect = std::make_unique<BasicEffect>(device);
    m_effect->SetVertexColorEnabled(true);

    // 入力レイアウトの作成
    void const* shaderByteCode;
    size_t byteCodeLength;
    m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    DX::ThrowIfFailed(
        device->CreateInputLayout(
            VertexPositionColor::InputElements,
            VertexPositionColor::InputElementCount,
            shaderByteCode, byteCodeLength,
            m_inputLayout.GetAddressOf()
        )
    );

    // スプライトバッチ、スプライトのフォントの初期設定
    m_spriteBatch = std::make_unique<SpriteBatch>(context);
    m_spriteFont = std::make_unique<SpriteFont>(device, L"Resources/Fonts/myfont.spritefont");

    // ---- スカイボックスの初期設定 ---- //
    Draw::GetInstance().InitSkyBox(m_deviceResources.get());
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent ObjectLib here.
    // 画面サイズを取得する
    int w, h;
    GetDefaultSize(w, h);

    // 射影行列の作成
    m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
        // 画角　　　　　　　　　　アスペクト比
        XMConvertToRadians(45.0f), static_cast<float>(w) / static_cast<float>(h),
        // Near Far
        0.1f, 100.0f);

    Draw::GetInstance().SetSkyBoxProj(m_proj);
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
    Draw::GetInstance().ResetSkyBox();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
