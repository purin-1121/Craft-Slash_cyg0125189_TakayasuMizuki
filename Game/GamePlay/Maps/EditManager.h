//====================================================//
// ファイル名   : EditManager.h
// 作成者       : 高安瑞貴
// 作成日時     : 2026/06/19 11:42:11
//
// 概要 : 地形の判定をエディットする
//
// 更新履歴 :
// 2026/06/19 11:42:11 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "Game/GamePlay/Maps/HeightMap.h"
#include "Lib/EditCamera.h"
#include "Common/UI/Canvas.h"
#include "Common/UI/Button/InputField.h"
#include "SimpleMath.h"
#include "Message/InputGroup.h"
#include "Message/EventBus.h"

//====================================================//
// クラス宣言
//====================================================//
class EditManager : public InputGroup
{

public:

    
    //-----------------------------------------------------
    // エディットのON・OFF
    //-----------------------------------------------------
    void EditOn(HeightMap* map, Camera* camera, const GameContext& gameContext);
    bool EditOff();

    // カメラの取得
    EditCamera* GetCamera() { return m_editCamera.get(); }

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    EditManager(InputGroup* group);
    ~EditManager();

    //-----------------------------------------------------
    // 基本処理
    //-----------------------------------------------------
    void Update(const GameContext& gameContext, float& elapsedTime);
    void Render(const GameContext& gameContext);
    void RenderBoundnig(const GameContext& gameContext);
    void RenderText(const GameContext& gameContext);
    void RenderPreview(const GameContext& gameContext);
    void Finalize();

    // 入力処理
    bool OnInputAccept(Message::InputEvent input) override;


private:

    // 項目三つの入力フィールド
    template<typename T>
    struct InputFor
    {
        InputField* inputx = nullptr;
        InputField* inputy = nullptr;
        InputField* inputz = nullptr;

        T* target = nullptr;

        // ポインタの設定
        void SetField(InputField* x, InputField* y, InputField* z)
        {
            inputx = x;
            inputy = y;
            inputz = z;
        }

        // 対象値の設定
        template<typename T>
        void SetTarget(T* t)
        {
            target = t;
            inputx->Set(std::format(L"{:.2f}", t->x));
            inputy->Set(std::format(L"{:.2f}", t->y));
            inputz->Set(std::format(L"{:.2f}", t->z));
        }

        // 変数の更新
        void DataUpdate()
        {
            target->x = inputx->GetNum();
            target->y = inputy->GetNum();
            target->z = inputz->GetNum();
        }

        // 入力フィールドの表示
        void DisplayField(bool active)
        {
            inputx->SetActive(active);
            inputy->SetActive(active);
            inputz->SetActive(active);
        }
    };

    // 変更を加えたか
    bool m_isChanged;

    // 対象のマップ
    HeightMap* m_targetMap;
    // 対象のカメラ
    std::unique_ptr<EditCamera> m_editCamera;
    // ドラッグ中
    bool m_isDrag;
    // ドラッグ開始位置
    SimpleMath::Vector2 m_startPos;
    // ドラッグ終了位置
    SimpleMath::Vector2 m_endPos;
    // 選択中の頂点
    std::vector<std::pair<int, int>> m_heightPositions;
    // エディットの視点
    SimpleMath::Vector3 m_editEye;
    // コマンドのキャンバス
    Canvas m_editCanvas;
    // 現在選択中のコマンド
    std::string mode;

    // コマンドボタンの大きさ
    static constexpr int BUTTON_SIZE = 100;
    // ボタンに使う文字列
    const std::vector<std::string> BUTTON_TEXT = {
        "Height",
        "Select",
        "Enemy",
        "End"
    };

    // 入力フィールド選択用列挙型
    enum Input 
    {
        vertical, depth, 
        size, 
        height, 
        posX, posY, posZ, 
        sizeX, sizeY, sizeZ,
        rotationX, rotationY, rotationZ,
        path,
        max
    };
    // 入力フィールドに使う文字列
    const std::string INPUT_TEXT[Input::max] = {
        "Vertical",
        "Depth",
        "GridSize",
        "PointHeight",
        "PosX",
        "PosY",
        "PosZ",
        "SizeX",
        "SizeY",
        "SizeZ",
        "RotationX",
        "RotationY",
        "RotationZ",
        "ModelPath"
    };
    // 今入力中か
    InputField* m_currentInput;
    // 入力イベントID
    SubsprictionID m_inputID;
    // カメラのダッシュ
    bool m_isDashed;
    // マウスがクリックされたときUIの上にあったか
    bool m_isOnUI;
    // オブジェクトを動かすとき用の
    InputFor<SimpleMath::Vector3> m_fieldPos;
    InputFor<SimpleMath::Quaternion> m_fieldRot;
    // 選択中のオブジェクト
    BreakableObject* m_target;

    // ---- モデルオブジェクトを設置するとき用 ---- //
    enum class EditMode
    {
        ModelInstall,
        Input,
        ModelChange,
    };
    // 選択中のモデルパス
    std::string* m_modelName;
    // モデルの名前を選択する用のボタンの大きさ
    static constexpr SimpleMath::Vector3 MODEL_BUTTON_SIZE = { 200.0f, 50.0f, 0 };
    // モデルのパスすべて
    std::vector<std::string> m_modelPathes;
    // 選択中のモデルの表示用
    Model* m_displayModel;
    // モデルを表示する場所(四隅)
    static constexpr SimpleMath::Vector3 MODEL_DISPLAY_POSITION_ORIGIN = {600, 20, 0};
    static constexpr int MODEL_DISPLAY_POSITION_SIZE = 100;
    // モデルを表示する3D座標
    static constexpr SimpleMath::Vector3 MODEL_DISPLAY_POSITION = { 0, 0, 0 };
    // モデル表示用のビューポート
    D3D11_VIEWPORT m_previewViewport;
    EditMode m_inspector;
    // スライダーでモデル選択用ボタンを動かす用の変数
    float m_positionForCanvas;
    // オブジェクト用データ

private:
    // キャンバスの作成
    void CreateCanvas();

    // ハイトマップ頂点の変更
    void EditHeight(const GameContext& gameContext, float& elapsedTime);
    // モデルの配置
    void SetModel(const GameContext& gameContext, float& elapsedTime);
    // モデルの選択
    void SelectedModel(const GameContext& gameContext);
    // 敵スポーンエリアの設置
    void SetSpawnSrea(const GameContext& gameContext);

};