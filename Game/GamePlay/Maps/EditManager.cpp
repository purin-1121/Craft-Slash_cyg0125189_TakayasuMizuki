#include "pch.h"
#include "EditManager.h"
#include "Common/Collider/RayCast.h"
#include "Common/Manager/DataManager.h"
#include "Common/UI/Button/FunctionButton.h"
#include "Common/UI/Button/InputField.h"
#include "Common/UI/Bar/Slider.h"
#include "Tools/TextConverter.h"
#include "Game/GamePlay/Data/BreakableObjectData.h"
#include "Game/GamePlay/Factory/DropFactory.h"


EditManager::EditManager(InputGroup* group)
	: InputGroup(group)
	, m_editCamera{std::make_unique<EditCamera>(DataManager::GetInstance()->ScreenX, 720, nullptr)}
	, m_targetMap{}
	, m_isDrag{}
	, mode{"Height"}
	, m_editCanvas{}
	, m_currentInput{}
	, m_isDashed{}
	, m_modelName{}
	, m_inspector{}
	, m_previewViewport{}
	, m_isChanged{}
{
	for (auto& model : JsonDataManager<BreakableObjectData>::GetInstance().GetDatas())
	{
		m_modelPathes.push_back(model.first);
	}
	CreateCanvas();

	m_previewViewport.TopLeftX = MODEL_DISPLAY_POSITION_ORIGIN.x;
	m_previewViewport.TopLeftY = MODEL_DISPLAY_POSITION_ORIGIN.y;
	m_previewViewport.Width = MODEL_DISPLAY_POSITION_SIZE;
	m_previewViewport.Height = MODEL_DISPLAY_POSITION_SIZE;
	m_previewViewport.MaxDepth = 1.0f;
	m_previewViewport.MinDepth = 0.0f;
}

EditManager::~EditManager()
{
	DeleteObservers();
}

void EditManager::EditOn(HeightMap* map, Camera* camera, const GameContext& gameContext)
{
	m_targetMap = map;
	// 今までのメインカメラの注視点を取得して、エディットカメラの初期注視点にする
	m_editEye = camera->GetTarget();
	m_editCamera->SetChasePos(m_editEye, Camera::ChasePatern::Object);
	m_editCanvas.Initialize(gameContext);
	m_inputID = EventBus::GetInstance()->Subscribe(Message::EventID::INPUT_START, [&](void* messanger) {
		m_currentInput = (messanger ? static_cast<InputField*>(messanger) : nullptr);
		});
}

bool EditManager::EditOff()
{
	//マップを変更後にセーブしてなかったら
	if (m_isChanged)
	{
		int result = MessageBox(
			nullptr,                           // 親ウィンドウのハンドル（nullptrでも可）
			L"セーブ内容を保存しますか？", // メッセージ本文
			L"確認",                        // タイトルバーの文字
			MB_YESNOCANCEL | MB_ICONWARNING          // OKボタン + 警告アイコン
		);
		// 保存処理の実行（はい）
		if (result == IDYES)
		{
			// 地形オブジェクトの保存
			if (!m_targetMap->SaveMap())
                return false;
			// 地形の保存
			m_targetMap->SaveModel();
			// 選択コマンドのリセット
			mode = BUTTON_TEXT[0];
			// 敵のスポーン地点の保存
			EventBus::GetInstance()->Publish(Message::EventID::SAVE_SPAWN_AREA, nullptr);
		}
		// 何もせずエディットモード続行（キャンセル）
		else if (result == IDCANCEL)
		{
			// 「キャンセル」：何もせずイベントを終了（ウィンドウは閉じない）
			return false;
		}
	}

	mode = "";
	m_isChanged = false;
	// イベント解除
	EventBus::GetInstance()->UnSubscribe(Message::EventID::INPUT_START, m_inputID);	

	return true;
}

void EditManager::Update(const GameContext& gameContext, float& elapsedTime)
{
	m_editCamera->Update(elapsedTime);
	m_editCanvas.Update(gameContext, elapsedTime);

	m_isOnUI = m_editCanvas.IsOnUIs();
	if (mode == BUTTON_TEXT[0]) EditHeight(gameContext, elapsedTime);
	else if (mode == BUTTON_TEXT[1]) SelectedModel(gameContext);
	else if (mode == BUTTON_TEXT[2]) SetSpawnSrea(gameContext);
	else if (mode == BUTTON_TEXT[3]) EventBus::GetInstance()->Publish(Message::EventID::EDIT, nullptr);
	else if (mode == "")SetModel(gameContext, elapsedTime);

	for (const auto& t : BUTTON_TEXT)
	{
		if (mode == t)
			m_editCanvas.SammonUI<FunctionButton>(t)->SetColor(XMVECTORF32{0.0f, 0.0f, 0.0f, 1.0f}, XMVECTORF32{ 1.0f, 1.0f, 1.0f, 1.0f });
		else
			m_editCanvas.SammonUI<FunctionButton>(t)->SetColor(XMVECTORF32{ 0.3f, 0.3f, 0.3f, 1.0f }, XMVECTORF32{ 1.0f, 1.0f, 1.0f, 1.0f });
	}
}

void EditManager::Render(const GameContext& gameContext)
{
	Draw::GetInstance().DrawBox((SimpleMath::Vector3)m_startPos, (SimpleMath::Vector3)m_endPos, Colors::Red);
}

void EditManager::RenderBoundnig(const GameContext& gameContext)
{
	if (mode == "Object")
	{
		for (auto& m : m_targetMap->GetModels())
		{
			//Draw::GetInstance().DrawBox(m.pos, m.size, Colors::Yellow);
		}
	}
}

void EditManager::RenderText(const GameContext& gameContext)
{

	m_editCanvas.RenderText();

	SimpleMath::Vector3 pos = {980, 200, 0};
	if (!m_heightPositions.empty() && m_inspector == EditMode::Input)
	{
		std::wstring str = L"";
		for (size_t i = 0; i < m_heightPositions.size(); i++)
		{
			pos.y = (i + 1) * 30;
			if (i == 3)
			{
				str = L"etc...";
				Draw::GetInstance().DrawText(str, pos);
				return;
			}
			else
			{
				str = L"x:" + std::to_wstring(m_heightPositions[i].first) + L"z:" + std::to_wstring(m_heightPositions[i].second);
				Draw::GetInstance().DrawText(str, pos);
			}
		}
	}

}

void EditManager::RenderPreview(const GameContext& gameContext)
{
	if (!m_modelName) return;

	auto it = ObjectLib::m_models.find(*m_modelName);
	if (it == ObjectLib::m_models.end() || !it->second) return;
	Model* model = it->second.get();
	if (model->meshes.empty()) return;

	auto context = gameContext.deviceResources.GetD3DDeviceContext();

	// 現在のビューポートを保存（Draw の m_initViewPort には頼らない）
	UINT num = 1;
	D3D11_VIEWPORT prev{};
	context->RSGetViewports(&num, &prev);

	context->RSSetViewports(1, &m_previewViewport);
	context->ClearDepthStencilView(gameContext.deviceResources.GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	// 全メッシュを含むバウンディング球
	BoundingSphere total = model->meshes[0]->boundingSphere;
	for (size_t k = 1; k < model->meshes.size(); ++k)
		BoundingSphere::CreateMerged(total, total, model->meshes[k]->boundingSphere);

	const float fov = XMConvertToRadians(45.0f);
	const float radius = std::max(total.Radius, 0.001f);
	const float dist = radius / sinf(fov * 0.5f) * 1.1f;

	SimpleMath::Vector3 center = total.Center;
	SimpleMath::Vector3 eye = center + SimpleMath::Vector3{ 0.0f, dist * 0.4f, -dist };
	SimpleMath::Matrix view = SimpleMath::Matrix::CreateLookAt(eye, center, SimpleMath::Vector3::Up);
	SimpleMath::Matrix proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(fov, 1.0f, 0.01f, dist * 10.0f);

	model->Draw(context, gameContext.commonStates, SimpleMath::Matrix::Identity, view, proj);

	context->RSSetViewports(1, &prev);
}

void EditManager::Finalize()
{
}

bool EditManager::OnInputAccept(Message::InputEvent input)
{
	switch (input)
	{
		// カメラの加速
	case Message::InputEvent::CAMERADASH:
		m_isDashed = !m_isDashed;
		return true;
		// カメラの移動
	case Message::InputEvent::MOVERIGHT:
	case Message::InputEvent::MOVELEFT:
	case Message::InputEvent::MOVEUP:
	case Message::InputEvent::MOVEDOWN:
	case Message::InputEvent::CAMERAVVERTICAL:
		m_editCamera->OnInputAccept(input);
		if(m_currentInput)m_currentInput->SelectOff();
		break;
	default:
		break;
	}

	auto o = this->m_observers;

	if (InputGroup::OnInputAccept(input))
		return true;

	return false;
}

void EditManager::CreateCanvas()
{
	// オブジェクト選択用のボタンを設定
	for (int i = 0; i < BUTTON_TEXT.size(); i++)
	{
		m_editCanvas.CreateUI(
			BUTTON_TEXT[i],
			new FunctionButton{
				[this, i]() { this->mode = BUTTON_TEXT[i]; },
				SimpleMath::Vector3{50.0f + (BUTTON_SIZE + 10.0f) * i, (float)BUTTON_SIZE / 2, 0},
				ToWString(BUTTON_TEXT[i]),
				this,
				SimpleMath::Vector3{BUTTON_SIZE, BUTTON_SIZE, 1}
			}
		);
	}

	// 入力用の背景を作成
	m_editCanvas.CreateUI(
		"InputBack",
		new Image(
			SimpleMath::Vector2{ 980, 0 },
			SimpleMath::Color{ 0, 0, 0, 0.6f },
			SimpleMath::Vector2{ 300, 720 }
		)
	);

	// 入力用の文字
	m_editCanvas.CreateUI(
		"InputText",
		new Text(L"選択中の頂点座標", { 980, 0 })
	);

	// 入力ボックス
	m_editCanvas.CreateUI(
		INPUT_TEXT[Input::height],
		new InputField(
			InputField::Type::num,
			{ 1000, 300, 0 },
			L"頂点の高さ",
			{ 250, 50, 0 }
		)
	);


	// ---- 床用の入力ボックス ---- //
	m_editCanvas.CreateUI(
		INPUT_TEXT[Input::size],
		new InputField(
			InputField::Type::num,
			{ 1160, 200, 0 },
			L"サイズ",
			{ 70, 50, 0 }
		)
	);

	m_editCanvas.CreateUI(
		INPUT_TEXT[Input::vertical],
		new InputField(
			InputField::Type::num,
			{ 1000, 200, 0 },
			L"横",
			{ 70, 50, 0 }
		)
	);
	m_editCanvas.CreateUI(
		INPUT_TEXT[Input::depth],
		new InputField(
			InputField::Type::num,
			{ 1080, 200, 0 },
			L"縦",
			{ 70, 50, 0 }
		)
	);

	// ---- オブジェクト用の入力ボックス ---- //
	// -- 位置 -- //
	{
		InputField* x = new InputField(InputField::Type::num, { 990, 100, 0 }, L"X", { 90, 50, 0 });
		InputField* y = new InputField(InputField::Type::num, { 1090, 100, 0 }, L"Y", { 90, 50, 0 });
		InputField* z = new InputField(InputField::Type::num, { 1190, 100, 0 }, L"Z", { 90, 50, 0 });
		m_fieldPos.SetField(x, y, z);
		m_fieldPos.DisplayField(false);
		m_editCanvas.CreateUI(
			INPUT_TEXT[Input::posX],
			x
		);
		m_editCanvas.CreateUI(
			INPUT_TEXT[Input::posY],
			y
		);
		m_editCanvas.CreateUI(
			INPUT_TEXT[Input::posZ],
			z
		);
	}
	// -- 回転 -- //
	{
		InputField* x = new InputField(InputField::Type::num, { 990, 250, 0 }, L"X", { 90, 50, 0 });
		InputField* y = new InputField(InputField::Type::num, { 1090, 250, 0 }, L"Y", { 90, 50, 0 });
		InputField* z = new InputField(InputField::Type::num, { 1190, 250, 0 }, L"Z", { 90, 50, 0 });
		m_fieldRot.SetField(x, y, z);
		m_fieldRot.DisplayField(false);
		m_editCanvas.CreateUI(
			INPUT_TEXT[Input::rotationX],
			x
		);
		m_editCanvas.CreateUI(
			INPUT_TEXT[Input::rotationY],
			y
		);
		m_editCanvas.CreateUI(
			INPUT_TEXT[Input::rotationZ],
			z
		);
	}
	// -- モデルデータ -- //
	m_editCanvas.CreateUI(
		INPUT_TEXT[Input::path],
		new InputField(
			InputField::Type::text,
			{ 1000, 400, 0 },
			L"モデルデータ",
			{ 260, 70, 0 }
		)
	);

	// モデル選択用のボタン
	int count = 0;
	for (int i = 0; i < m_modelPathes.size(); i++)
	{
		m_editCanvas.CreateUI(
			m_modelPathes[i],
			new FunctionButton(
				[this, i]() {
					m_modelName = &m_modelPathes[i];
					mode = "";
				},
				SimpleMath::Vector3{ 1000, MODEL_BUTTON_SIZE.y * count + 20, 0 },
				ToWString(m_modelPathes[i]),
				this,
				MODEL_BUTTON_SIZE
			)
		)->SetActive(false);
		count++;
	}

	// モデル選択ボタンをスクロールするためのバー
	m_editCanvas.CreateUI(
		"Position",
		new Slider(
			m_positionForCanvas,
			SimpleMath::Vector3{ 1220, 20, 0 },
			40.0f, DataManager::GetRefInstance().ScreenY - 20.0f,
			XMVECTORF32{ 0.2, 0.2, 0.2, 1.0f },
			-static_cast<float>(m_modelPathes.size()) / 2 * MODEL_BUTTON_SIZE.y, 0.0f, 0.0f,
			false
		)
	)->SetActive(false);

	m_editCanvas.AddFunc(
		"MoveButton",
		[&]()
		{
			for (auto& model : m_modelPathes)
			{
				auto ui = m_editCanvas.SammonUI(model);
				if (ui)
					ui->SetOffset({ 0.0f, m_positionForCanvas, 0.0f });
			}
		}
	);

	// ---- モード変更用のボタン設定 ---- //
	m_editCanvas.CreateUI(
		"ModelSelect",
		new FunctionButton
		(
			[&]()
			{
				// 入力フィールドを全非表示
				for (int i = 0; i < Input::max; i++)
				{
					auto ui = m_editCanvas.SammonUI(INPUT_TEXT[i]);
					if (ui)ui->SetActive(false);
				}
				m_editCanvas.SammonUI("InputText")->SetActive(false);
				// モデル選択用ボタンを表示
				for (auto& model : m_modelPathes)
				{
					auto ui = m_editCanvas.SammonUI(model);
					if (ui)ui->SetActive(true);
				}
				m_editCanvas.SammonUI("Position")->SetActive(true);
				m_inspector = EditMode::ModelInstall;
			},
			SimpleMath::Vector3{ 900, 0, 0 }, L"モデル設置", this, SimpleMath::Vector3{ 80, 80, 0 }
		)
	);
	m_editCanvas.SammonUI<FunctionButton>("ModelSelect")->InVoke();
	m_editCanvas.CreateUI(
		"Stage",
		new FunctionButton
		(
			[&]()
			{
				m_inspector = EditMode::Input;
				// 入力フィールドを表示
				for (int i = 0; i < Input::max; i++)
				{
					auto ui = m_editCanvas.SammonUI<InputField>(INPUT_TEXT[i]);
					if (ui)
						ui->SetActive(i < Input::posX);
				}
				m_editCanvas.SammonUI<InputField>(INPUT_TEXT[Input::vertical])->Set(std::to_wstring(m_targetMap->GetSize().first));
				m_editCanvas.SammonUI<InputField>(INPUT_TEXT[Input::depth])->Set(std::to_wstring(m_targetMap->GetSize().first));
				m_editCanvas.SammonUI<InputField>(INPUT_TEXT[Input::size])->Set(std::to_wstring(m_targetMap->GetGridSize()));
				m_editCanvas.SammonUI<InputField>(INPUT_TEXT[Input::height])->Set(L"0");
				m_editCanvas.SammonUI("InputText")->SetActive(true);
				// モデル選択用ボタンを非表示
				for (auto& model : m_modelPathes)
				{
					m_modelName = nullptr;
					auto ui = m_editCanvas.SammonUI(model);
					if (ui)ui->SetActive(false);
				}
				m_editCanvas.SammonUI("Position")->SetActive(false);
			},
			SimpleMath::Vector3{ 900, 80, 0 }, L"ステージ", this, SimpleMath::Vector3{ 80, 80, 0 }
		)
	);
	m_editCanvas.CreateUI(
		"Change",
		new FunctionButton
		(
			[&]()
			{
				m_inspector = EditMode::ModelChange;
				// 入力フィールドを非表示
				for (int i = 0; i < Input::max; i++)
				{
					auto ui = m_editCanvas.SammonUI(INPUT_TEXT[i]);
					if (ui)ui->SetActive((i > Input::height));
				}
				m_editCanvas.SammonUI("InputText")->SetActive(false);
				// モデル選択用ボタンを非表示
				for (auto& model : m_modelPathes)
				{
					m_modelName = nullptr;
					auto ui = m_editCanvas.SammonUI(model);
					if (ui)ui->SetActive(false);
				}
				m_editCanvas.SammonUI("Position")->SetActive(false);
			},
			SimpleMath::Vector3{ 900, 160, 0 }, L"モデル数値", this, SimpleMath::Vector3{ 80, 80, 0 }
		)
	);
}

void EditManager::EditHeight(const GameContext& gameContext, float&)
{
	auto state = Mouse::Get().GetState();

	// スペース押したら保留中の内容を更新
	if (gameContext.keyboardTracker.IsKeyPressed(Keyboard::Space))
	{

		// 入力された高さを取得
		auto p = m_editCanvas.SammonUI(INPUT_TEXT[Input::height]);
		auto i = dynamic_cast<InputField*>(m_editCanvas.SammonUI(INPUT_TEXT[Input::height]));
		float r = (i ? i->GetNum() : 0);
		// 頂点の高さを変更
		for (auto& pos : m_heightPositions)
		{
			m_targetMap->SetHeight(pos.first, pos.second, r);
		}

		// グリッドの広さを変更
		float x = dynamic_cast<InputField*>(m_editCanvas.SammonUI(INPUT_TEXT[Input::vertical ]))->GetNum();
		float z = dynamic_cast<InputField*>(m_editCanvas.SammonUI(INPUT_TEXT[Input::depth]))->GetNum();
		float size = dynamic_cast<InputField*>(m_editCanvas.SammonUI(INPUT_TEXT[Input::size]))->GetNum();
		m_targetMap->SetGrid(x, z);
		m_targetMap->SetGridSize(size);

		m_isChanged = true;
	}

    // 左クリックされたら
	if (gameContext.mouseButtonTracker.leftButton == Mouse::ButtonStateTracker::ButtonState::PRESSED)
	{
		// UIに触れていたら処理をしない
		if (m_editCanvas.IsOnUIs())
		{
			m_isOnUI = true;
			return;
		}
		// マウスの開始位置の設定
		m_startPos = { (float)state.x, (float)state.y };
		m_endPos = m_startPos;
	}

	// ホールド中だったら
	if (gameContext.mouseButtonTracker.leftButton == Mouse::ButtonStateTracker::ButtonState::HELD)
	{
		if (m_isOnUI)
			return;
		// 終了位置の更新
		m_endPos = { (float)state.x, (float)state.y };
	}

	
	// 離されたら
	if (gameContext.mouseButtonTracker.leftButton == Mouse::ButtonStateTracker::ButtonState::RELEASED)
	{
		if (m_isOnUI)
		{
			m_isOnUI = false;
			return;
		}
		SimpleMath::Vector2 topLeft(m_startPos.x, m_startPos.y);
		SimpleMath::Vector2 topRight(m_endPos.x, m_startPos.y);
		SimpleMath::Vector2 bottomLeft(m_startPos.x, m_endPos.y);
		SimpleMath::Vector2 bottomRight(m_endPos.x, m_endPos.y);

		SimpleMath::Vector2 screenCorners[4] = { topLeft, topRight, bottomLeft, bottomRight };
		SimpleMath::Vector3 hitPoints[4];

		bool isHit = false; // 1つでもヒットしたかチェック用

		for (int i = 0; i < 4; ++i)
		{
			LineCollider line = m_editCamera->ScreenPointToRay(screenCorners[i], Draw::GetInstance().GetRenderingContext().proj, DataManager::GetInstance()->ScreenX, DataManager::GetInstance()->ScreenY);
			SimpleMath::Vector3 hitPoint;

			// 実際の地形と判定
			if (m_targetMap->RayCast(line, hitPoint))
			{
				hitPoints[i] = hitPoint;
				isHit = true;
			}
			else
			{
				// 地形に当たらなかった場合の救済措置（Y=0の平面との判定）
				PlaneCollider groundPlane;
				if (RayCaster::RayCast(line, groundPlane))
				{
					hitPoints[i] = line.GetCrossPoint();
					isHit = true;
				}
			}
		}

		// 平面に当たっていれば範囲計算を行う
		if (isHit)
		{
			// 外周を順番に結んで四角形を作るため、「左上、右上、右下、左下」の順番で渡す
			m_heightPositions = m_targetMap->GetVerticesInQuad(
				hitPoints[0], // 左上
				hitPoints[1], // 右上
				hitPoints[3], // 右下
				hitPoints[2]  // 左下
			);
		}
		m_endPos = m_startPos;
	}
}

void EditManager::SetModel(const GameContext& gameContext, float&)
{
	auto state = Mouse::Get().GetState();

	if (gameContext.mouseButtonTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
	{
		// UIに触れていたら処理をしない
		if (m_isOnUI || !m_modelName)
			return;

		// ---- マウス位置からレイを飛ばして座標を計算 ---- //
		LineCollider ray = m_editCamera->ScreenPointToRay(
			SimpleMath::Vector2{ (float)state.x, (float)state.y },
			Draw::GetInstance().GetRenderingContext().proj,
			DataManager::GetInstance()->ScreenX,
			DataManager::GetInstance()->ScreenY
		);

		SimpleMath::Vector3 hitPoint;
		// ★修正：RayCast が成功した（床に当たった）ときだけ AddModel する
		if (m_targetMap->RayCast(ray, hitPoint))
		{
			auto pos = hitPoint;
			auto info = DropFactory::GetInstance().GetInfo(*m_modelName);
			auto data = JsonDataManager<BreakableObjectData>::GetInstance().GetData(*m_modelName);
			auto obj = std::make_unique<BreakableObject>
				(pos, SimpleMath::Vector3::Zero, info,data);
			obj->Initialize(gameContext);
			m_targetMap->AddModel(std::move(obj));
		}

		m_isChanged = true;
	}
}

void EditManager::SelectedModel(const GameContext& gameContext)
{
	auto state = Mouse::Get().GetState();

	// モデル選択
	if (gameContext.mouseButtonTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
	{
		// UIに触れていたら処理をしない
		if (m_isOnUI)
			return;

		// ---- マウス位置からレイを飛ばして座標を計算 ---- //
		LineCollider ray = m_editCamera->ScreenPointToRay(
			SimpleMath::Vector2{ (float)state.x, (float)state.y },
			Draw::GetInstance().GetRenderingContext().proj,
			DataManager::GetInstance()->ScreenX,
			DataManager::GetInstance()->ScreenY
		);

        BreakableObject* selected = nullptr;
        float nearestDistanceSquared = ray.GetSize().LengthSquared();
        for (auto& model : m_targetMap->GetModels())
        {
            // Editing can move objects while normal collision updates are paused.
            model->UpdateCollider();
            for (auto* collider : model->GetBounding())
            {
                if (RayCaster::RayCast(ray, collider))
                {
                    const float distanceSquared =
                        (ray.GetCrossPoint() - ray.GetStart()).LengthSquared();
                    if (!selected || distanceSquared < nearestDistanceSquared)
                    {
                        nearestDistanceSquared = distanceSquared;
                        selected = model.get();
                    }
                }
            }
        }

        if (selected)
        {
            m_fieldPos.SetTarget(&selected->GetPosition());
            m_fieldPos.DisplayField(true);
            m_fieldRot.SetTarget(&selected->GetQuaternion());
            m_fieldRot.DisplayField(true);
            m_editCanvas.SammonUI<InputField>(INPUT_TEXT[Input::path])->Set(ToWString(selected->GetData().name));
            m_target = selected;
			m_editCanvas.SammonUI<FunctionButton>("Change")->InVoke();
        }
	}

	// 選択中のオブジェクトの更新処理
	if (gameContext.keyboardTracker.IsKeyPressed(Keyboard::Space))
	{
		if (!m_target)
			return;
		// 位置と角度の更新
		m_fieldPos.DataUpdate();
		SimpleMath::Vector3 rotationDegrees = { 
			m_fieldRot.inputx->GetNum(), 
			m_fieldRot.inputy->GetNum(), 
			m_fieldRot.inputz->GetNum()
		};

		auto q = SimpleMath::Quaternion::CreateFromYawPitchRoll(
			DirectX::XMConvertToRadians(rotationDegrees.y), // Y軸：Yaw
			DirectX::XMConvertToRadians(rotationDegrees.x), // X軸：Pitch
			DirectX::XMConvertToRadians(rotationDegrees.z)  // Z軸：Roll
		);

		m_target->SetQuaternion(q);

		// データの変更
		auto text = m_editCanvas.SammonUI<InputField>(INPUT_TEXT[Input::path])->GetText();
		// wstring -> string
		std::string str = ToString(text);
		for (auto& data : JsonDataManager<BreakableObjectData>::GetInstance().GetDatas())
		{
			// 名前が一致していたら更新

			for (auto& d : data.second)
			{
				if (str == d->name)
				{
					m_target->SetData(&(*d));
					m_target->CreateWorldMatrix();
					m_target->UpdateCollider();
					return;
				}
			}
		}
	}
}

void EditManager::SetSpawnSrea(const GameContext& gameContext)
{
	auto state = Mouse::Get().GetState();


	if (gameContext.mouseButtonTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
	{
		// UIに触れていたら処理をしない
		if (m_isOnUI)
			return;

		// ---- マウス位置からレイを飛ばして座標を計算 ---- //
		LineCollider ray = m_editCamera->ScreenPointToRay(
			SimpleMath::Vector2{ (float)state.x, (float)state.y },
			Draw::GetInstance().GetRenderingContext().proj,
			DataManager::GetInstance()->ScreenX,
			DataManager::GetInstance()->ScreenY
		);

		SimpleMath::Vector3 hitPoint;
		if (m_targetMap->RayCast(ray, hitPoint))
		{
			EventBus::GetInstance()->Publish(Message::EventID::SET_SPAWN_AREA, &hitPoint);
		}
	}

}
