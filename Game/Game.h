//
// Game.h
//

#pragma once


#include "Common/StepTimer.h"
#include "Common/Draw/Draw.h"
#include "Common/Scene/SceneManager.h"
#include "Common/Collider/CollisionManager.h"
#include "Common/Manager/storageManager.h"
#include "Common/Manager/Resources/ResourceManager.h"
#include "Lib/Camera.h"

#include <memory>
#include <optional>
#include <Common/Draw/SkyBox.h>


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

    // 射影行列
    SimpleMath::Matrix m_proj;

    // コモンステート
    std::unique_ptr<CommonStates> m_states;

    // プリミティブバッチ
    std::unique_ptr<PrimitiveBatch<VertexPositionColor>> m_batch;

    // ベーシックエフェクト
    std::unique_ptr<BasicEffect> m_effect;

    // インプットレイアウト
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

    // スプライトバッチ
    std::unique_ptr<SpriteBatch> m_spriteBatch;
    // スプライトフォント
    std::unique_ptr<SpriteFont> m_spriteFont;

    // コンテキスト
    std::unique_ptr<GameContext> m_gameContext;
    std::unique_ptr<RenderingContext> m_renderingContext;

    // トラッカー
    std::unique_ptr<Keyboard::KeyboardStateTracker> m_keyboardTracker;
    std::unique_ptr<Mouse::ButtonStateTracker> m_mouseTracker;



    // ------ デバッグ用 ------ //

};
