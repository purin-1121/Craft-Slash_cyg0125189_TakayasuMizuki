//====================================================//
// ファイル名   : Draw.h
// 作成者       : 高安瑞貴
//
// 概要 : 描画制御用シングルトンクラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include <memory>
#include <functional>
#include "Tools/GameContext.h"
#include "Model.h"
#include "SimpleMath.h"
#include "VertexTypes.h"
#include "Effects.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "PrimitiveBatch.h"
#include "SkyBox.h"

//====================================================//
// 宣言
//====================================================//
using namespace DirectX;
class Draw
{

	// 描画関係
public:

	/// <summary>
	/// 3Dモデルの描画
	/// </summary>
	/// <param name="gameContext">コンテキスト</param>
	/// <param name="renderingContext">描画用コンテキスト</param>
	/// <param name="world">ワールド行列</param>
	/// <param name="view">ビュー行列</param>
	/// <param name="model">モデルのポインタ</param>
	/// <param name="func">描画の関数オブジェクト</param>
	inline void DrawModel(
		const GameContext& gameContext,
		const SimpleMath::Matrix& world,
		const SimpleMath::Matrix& view,
		const Model* model,
		std::function<void()> func = nullptr)
	{
		if (!func) func = []() {};
		model->Draw(
			gameContext.deviceResources.GetD3DDeviceContext(),
			gameContext.commonStates,
			world,
			view,
			renderingContext->proj,
			false,
			func
		);
	}

	/// <summary>
	/// 3D矩形描画
	/// </summary>
	/// <param name="batch">プリミティブバッチ</param>
	/// <param name="start">始点</param>
	/// <param name="end">終点</param>
	/// <param name="color">描画色</param>
	void DrawBox(
		const SimpleMath::Vector3 start,
		const SimpleMath::Vector3 end,
		FXMVECTOR color)
	{
		auto [x1, y1, z1] = start;
		auto [x2, y2, z2] = end;
		// 頂点情報
		VertexPositionColor v[8] = {
		    { {x1, y1, z1}, color }, { {x2, y1, z1}, color }, // 0,1
		    { {x2, y2, z1}, color }, { {x1, y2, z1}, color }, // 2,3
		    { {x1, y1, z2}, color }, { {x2, y1, z2}, color }, // 4,5
		    { {x2, y2, z2}, color }, { {x1, y2, z2}, color }  // 6,7
		};

		// 面情報
		static const uint16_t indices[] = {
		    0, 1, 1, 2, 2, 3, 3, 0, // 手前面
		    4, 5, 5, 6, 6, 7, 7, 4, // 奥面
		    0, 4, 1, 5, 2, 6, 3, 7  // 接続
		};

		renderingContext->primitiveBatch.DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, indices, 24, v, 8);
	}

	/// <summary>
	/// 3D塗りつぶし矩形描画
	/// </summary>
	/// <param name="start"></param>
	/// <param name="end"></param>
	/// <param name="color"></param>
	void DrawBoxSolid(
		const SimpleMath::Vector3 start,
		const SimpleMath::Vector3 end,
		FXMVECTOR color) 
	{
		auto [x1, y1, z1] = start;
		auto [x2, y2, z2] = end;

		// 頂点情報
		VertexPositionColor v[8] = {
			{ {x1, y1, z1}, color }, { {x2, y1, z1}, color }, // 0, 1
			{ {x2, y2, z1}, color }, { {x1, y2, z1}, color }, // 2, 3
			{ {x1, y1, z2}, color }, { {x2, y1, z2}, color }, // 4, 5
			{ {x2, y2, z2}, color }, { {x1, y2, z2}, color }  // 6, 7
		};

		// 三角形リスト用のインデックス
		static const uint16_t indices[] = {
			0, 2, 1, 0, 3, 2, // 前面
			4, 5, 6, 4, 6, 7, // 背面
			0, 1, 5, 0, 5, 4, // 底面
			2, 3, 7, 2, 7, 6, // 上面
			0, 4, 7, 0, 7, 3, // 左面
			1, 2, 6, 1, 6, 5  // 右面
		};

		// 第1引数を TRIANGLELIST に変更
		renderingContext->primitiveBatch.DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indices, 36, v, 8);
	}

	/// <summary>
/// 3D OBB描画
/// </summary>
/// <param name="center">中心座標</param>
/// <param name="axisX">X軸方向の半軸ベクトル</param>
/// <param name="axisY">Y軸方向の半軸ベクトル</param>
/// <param name="axisZ">Z軸方向の半軸ベクトル</param>
/// <param name="color">描画色</param>
	void DrawOBB(
		const SimpleMath::Vector3& center,
		const SimpleMath::Vector3& axisX,
		const SimpleMath::Vector3& axisY,
		const SimpleMath::Vector3& axisZ,
		const FXMVECTOR& color = Colors::White)
	{
		//========================================
		// OBBの8頂点を作る
		//========================================

		SimpleMath::Vector3 v[8] =
		{
			center - axisX - axisY - axisZ,
			center + axisX - axisY - axisZ,
			center + axisX + axisY - axisZ,
			center - axisX + axisY - axisZ,

			center - axisX - axisY + axisZ,
			center + axisX - axisY + axisZ,
			center + axisX + axisY + axisZ,
			center - axisX + axisY + axisZ
		};

		//========================================
		// 辺の接続情報
		//========================================

		static const uint16_t indices[] =
		{
			// 手前
			0, 1,
			1, 2,
			2, 3,
			3, 0,

			// 奥
			4, 5,
			5, 6,
			6, 7,
			7, 4,

			// 手前と奥を接続
			0, 4,
			1, 5,
			2, 6,
			3, 7
		};

		//========================================
		// 頂点情報
		//========================================

		VertexPositionColor vertices[8];

		for (int i = 0; i < 8; ++i)
		{
			vertices[i] = { v[i], color };
		}

		//========================================
		// ワイヤーフレーム描画
		//========================================

		renderingContext->primitiveBatch.DrawIndexed(
			D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
			indices,
			24,
			vertices,
			8
		);
	}

	/// <summary>
	/// 3D球体描画
	/// </summary>
	/// <param name="segments">分割する頂点数</param>
	/// <param name="radius">半径</param>
	/// <param name="center">座標</param>
	/// <param name="color">描画色</param>
	/// <param name="shaft">分割する軸の数</param>
	void DrawSphere(
		const int& segments,
		const float& radius,
		const SimpleMath::Vector3& center,
		const FXMVECTOR& color = Colors::White,
		const int& shaft = 3
	)
	{
		std::vector<DirectX::VertexPositionColor> vertices;
		vertices.reserve(segments * 2 * shaft); // 3軸分

		// 3つのリング（XZ, XY, YZ）の線分データを一気に生成
		for (int i = 0; i < segments; ++i)
		{
			float theta1 = (float)i / segments * DirectX::XM_2PI;
			float theta2 = (float)(i + 1) / segments * DirectX::XM_2PI;

			// XZ平面の円（真横の輪）
			vertices.push_back({ center + SimpleMath::Vector3(radius * cosf(theta1), 0.0f, radius * sinf(theta1)), color });
			vertices.push_back({ center + SimpleMath::Vector3(radius * cosf(theta2), 0.0f, radius * sinf(theta2)), color });

			// XY平面の円（正面の輪）
			vertices.push_back({ center + SimpleMath::Vector3(radius * cosf(theta1), radius * sinf(theta1), 0.0f), color });
			vertices.push_back({ center + SimpleMath::Vector3(radius * cosf(theta2), radius * sinf(theta2), 0.0f), color });

			// YZ平面の円（縦の輪）
			vertices.push_back({ center + SimpleMath::Vector3(0.0f, radius * cosf(theta1), radius * sinf(theta1)), color });
			vertices.push_back({ center + SimpleMath::Vector3(0.0f, radius * cosf(theta2), radius * sinf(theta2)), color });
		}

		// DirectXTKの PrimitiveBatch を使って線分の集まり（LINELIST）を描画
		Draw::GetInstance().GetRenderingContext().primitiveBatch.Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, vertices.data(), vertices.size());
	}

	/// <summary>
/// 3Dカプセル描画
/// </summary>
/// <param name="segments">円を分割する数</param>
/// <param name="radius">半径</param>
/// <param name="start">始点(下側の半球の中心)</param>
/// <param name="end">終点(上側の半球の中心)</param>
/// <param name="color">描画色</param>
	void DrawCapsule(
		const int& segments,
		const float& radius,
		const SimpleMath::Vector3& start,
		const SimpleMath::Vector3& end,
		const FXMVECTOR& color = Colors::White
	)
	{
		SimpleMath::Vector3 axis = end - start;
		float height = axis.Length();

		// 軸の長さがほぼ0なら、適当な軸を仮定しておく(ゼロ除算防止)
		if (height < 0.0001f)
		{
			axis = SimpleMath::Vector3::Up;
		}
		else
		{
			axis.Normalize();
		}

		// 軸に垂直な2つの単位ベクトル(u, v)を作る
		// 軸がほぼUpと平行な場合はRightを使い、そうでなければUpを使う(平行判定の回避)
		SimpleMath::Vector3 arbitrary = (fabsf(axis.Dot(SimpleMath::Vector3::Up)) > 0.99f)
			? SimpleMath::Vector3::Right
			: SimpleMath::Vector3::Up;

		SimpleMath::Vector3 u = axis.Cross(arbitrary);
		u.Normalize();
		SimpleMath::Vector3 v = axis.Cross(u); // 単位ベクトル同士のクロスなので正規化不要

		std::vector<DirectX::VertexPositionColor> vertices;
		vertices.reserve(segments * 10);

		// ---- 始点側・終点側の円(胴体の輪っか) ---- //
		for (int i = 0; i < segments; ++i)
		{
			float theta1 = (float)i / segments * DirectX::XM_2PI;
			float theta2 = (float)(i + 1) / segments * DirectX::XM_2PI;

			SimpleMath::Vector3 offset1 = (u * cosf(theta1) + v * sinf(theta1)) * radius;
			SimpleMath::Vector3 offset2 = (u * cosf(theta2) + v * sinf(theta2)) * radius;

			vertices.push_back({ start + offset1, color });
			vertices.push_back({ start + offset2, color });

			vertices.push_back({ end + offset1, color });
			vertices.push_back({ end + offset2, color });
		}

		// ---- 両端の半球アーチ(u方向・v方向) ---- //
		const int halfSegments = (segments / 2 > 0) ? segments / 2 : 1;
		for (int i = 0; i < halfSegments; ++i)
		{
			float theta1 = (float)i / halfSegments * DirectX::XM_PI;
			float theta2 = (float)(i + 1) / halfSegments * DirectX::XM_PI;

			// u方向のアーチ(始点は-axis側、終点は+axis側に膨らむ)
			vertices.push_back({ start + u * (radius * cosf(theta1)) - axis * (radius * sinf(theta1)), color });
			vertices.push_back({ start + u * (radius * cosf(theta2)) - axis * (radius * sinf(theta2)), color });

			vertices.push_back({ end + u * (radius * cosf(theta1)) + axis * (radius * sinf(theta1)), color });
			vertices.push_back({ end + u * (radius * cosf(theta2)) + axis * (radius * sinf(theta2)), color });

			// v方向のアーチ
			vertices.push_back({ start + v * (radius * cosf(theta1)) - axis * (radius * sinf(theta1)), color });
			vertices.push_back({ start + v * (radius * cosf(theta2)) - axis * (radius * sinf(theta2)), color });

			vertices.push_back({ end + v * (radius * cosf(theta1)) + axis * (radius * sinf(theta1)), color });
			vertices.push_back({ end + v * (radius * cosf(theta2)) + axis * (radius * sinf(theta2)), color });
		}

		// ---- 胴体の側面を結ぶ直線(4本、シルエット用) ---- //
		for (int i = 0; i < 4; ++i)
		{
			float theta = (float)i / 4 * DirectX::XM_2PI;
			SimpleMath::Vector3 offset = (u * cosf(theta) + v * sinf(theta)) * radius;
			vertices.push_back({ start + offset, color });
			vertices.push_back({ end + offset, color });
		}

		renderingContext->primitiveBatch.Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, vertices.data(), vertices.size());
	}

	/// <summary>
/// 3D円柱描画(蓋が平らなワイヤーフレーム)
/// </summary>
	void DrawCylinder(
		const int& segments,
		const float& radius,
		const SimpleMath::Vector3& bottom,
		const SimpleMath::Vector3& top,
		const FXMVECTOR& color = Colors::White
	)
	{
		SimpleMath::Vector3 axis = top - bottom;
		float height = axis.Length();

		if (height < 0.0001f)
		{
			axis = SimpleMath::Vector3::Up;
		}
		else
		{
			axis.Normalize();
		}

		SimpleMath::Vector3 arbitrary = (fabsf(axis.Dot(SimpleMath::Vector3::Up)) > 0.99f)
			? SimpleMath::Vector3::Right
			: SimpleMath::Vector3::Up;

		SimpleMath::Vector3 u = axis.Cross(arbitrary);
		u.Normalize();
		SimpleMath::Vector3 v = axis.Cross(u);

		std::vector<DirectX::VertexPositionColor> vertices;
		vertices.reserve(segments * 4 + 8);

		// ---- 上下の蓋(円) ---- //
		for (int i = 0; i < segments; ++i)
		{
			float theta1 = (float)i / segments * DirectX::XM_2PI;
			float theta2 = (float)(i + 1) / segments * DirectX::XM_2PI;

			SimpleMath::Vector3 offset1 = (u * cosf(theta1) + v * sinf(theta1)) * radius;
			SimpleMath::Vector3 offset2 = (u * cosf(theta2) + v * sinf(theta2)) * radius;

			vertices.push_back({ bottom + offset1, color });
			vertices.push_back({ bottom + offset2, color });

			vertices.push_back({ top + offset1, color });
			vertices.push_back({ top + offset2, color });
		}

		// ---- 側面の縦線(4本) ---- //
		for (int i = 0; i < 4; ++i)
		{
			float theta = (float)i / 4 * DirectX::XM_2PI;
			SimpleMath::Vector3 offset = (u * cosf(theta) + v * sinf(theta)) * radius;
			vertices.push_back({ bottom + offset, color });
			vertices.push_back({ top + offset, color });
		}

		renderingContext->primitiveBatch.Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, vertices.data(), vertices.size());
	}

	/// <summary>
	/// 頂点描画
	/// </summary>
	/// <param name="point">描画位置</param>
	/// <param name="olor">描画色</param>
	void DrawDot(
		const SimpleMath::Vector3 point,
		FXMVECTOR color
		)
	{
		VertexPositionColor v[1] = { {point, color} };
		renderingContext->primitiveBatch.Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, v, 1);

	}

	RenderingContext& GetRenderingContext()
	{
		return *renderingContext.get();
	}

	/// <summary>
	/// 描画用コンテキストの設定
	/// </summary>
	/// <param name="proj">射影行列</param>
	/// <param name="effect">ベーシックエフェクト</param>
	/// <param name="inputLayout">入力レイアウト</param>
	/// <param name="spriteBatch">スプライトバッチ</param>
	/// <param name="spriteFont">スプライトフォント</param>
	/// <param name="primitiveBatch">プリミティブバッチ</param>
	void SetRenderingContext(
		SimpleMath::Matrix& proj,
		BasicEffect& effect,
		Microsoft::WRL::ComPtr<ID3D11InputLayout>& inputLayout,
		SpriteBatch& spriteBatch,
		SpriteFont& spriteFont,
		PrimitiveBatch<VertexPositionColor>& primitiveBatch
	)
	{
		renderingContext = std::make_unique<RenderingContext>(
			proj,
			effect,
			inputLayout,
			spriteBatch,
			spriteFont,
			primitiveBatch
		);
	}

	void SetRenderingContext(RenderingContext context)
	{
		renderingContext.reset(new RenderingContext(context));
	}

	// ---- SpriteBatch ---- //
	/// <summary>
	/// 文字描画
	/// </summary>
	/// <param name="renderingContext">描画用コンテキスト</param>
	/// <param name="text">テキスト</param>
	/// <param name="pos">表示位置</param>
	/// <param name="pos">描画色</param>
	/// <param name="scale">拡大率</param>
	/// <param name="pos">表示位置</param>
	void DrawText(const std::wstring& text, SimpleMath::Vector3 pos, XMVECTORF32 color = Colors::White, SimpleMath::Vector3 scale = SimpleMath::Vector3::One)
	{
		renderingContext->spriteFont.DrawString(
			&renderingContext->spriteBatch,
			text.c_str(),
			SimpleMath::Vector2(pos),
			color,
			0.0f,
			SimpleMath::Vector2::Zero,
			SimpleMath::Vector2(scale)
		);
	}
	/// <summary>
	/// スプライトバッチのボックス描画
	/// </summary>
	/// <param name="hundle">画像のハンドル</param>
	/// <param name="worldPos">ワールドポジション</param>
	/// <param name="scale">スケール</param>
	/// <param name="color">描画色</param>
	/// <param name="viewPos">画像の描画開始位置</param>
	void DrawSpriteBox(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> hundle, SimpleMath::Vector2 worldPos, SimpleMath::Vector2 scale, XMVECTOR color = Colors::White, SimpleMath::Vector2 viewPos = SimpleMath::Vector2::Zero)
	{
		// 下地の部分の描画
		renderingContext->spriteBatch.Draw(
			hundle.Get(), 
			worldPos,           // 表示位置
			nullptr,              // ソース矩形（nullで全体）
			color,    // ここで指定した色になる！
			0.0f, 
			viewPos, 
			scale     // scaleに「幅100, 高さ50」を指定して引き延ばす
		);
	}

	/// <summary>
	/// キャッシュされた頂点データを使って線を描画する
	/// </summary>
	/// <param name="vertices">頂点配列</param>
	/// <param name="vertexCount">使用する頂点数</param>
	void DrawLineList(
		const VertexPositionColor* vertices,
		size_t vertexCount)
	{
		if (vertexCount == 0)
			return;

		renderingContext->primitiveBatch.Draw(
			D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
			vertices,
			vertexCount);
	}

	// 初期化
	void Initielize(ID3D11DeviceContext* context)
	{
		UINT num = 1;
		context->RSGetViewports(&num, &m_initViewPort);
	}

	D3D11_VIEWPORT& GetDefaultViewport() { return m_initViewPort; }

	// ---- Begin・End ---- //
	void PrimitibBegin()
	{
		renderingContext->primitiveBatch.Begin();
	}
	void PrimitibEnd()
	{
		renderingContext->primitiveBatch.End();
	}
	void SpriteBegin()
	{
		renderingContext->spriteBatch.Begin();
	}
	void SpriteEnd()
	{
		renderingContext->spriteBatch.End();
	}

	// ---- SkyBox ---- //
	void InitSkyBox(DX::DeviceResources* pDR)
	{
		auto device = pDR->GetD3DDevice();
		auto context = pDR->GetD3DDeviceContext();

		m_sky = GeometricPrimitive::CreateGeoSphere(context, 2.f, 3,
			false /*invert for being inside the shape*/);

		m_effect = std::make_unique<DX::SkyboxEffect>(device);

		m_sky->CreateInputLayout(m_effect.get(),
			m_skyInputLayout.ReleaseAndGetAddressOf());

		DX::ThrowIfFailed(
			CreateDDSTextureFromFile(device, L"Resources/Textures/cloudy_skybox.dds",
				nullptr, m_cubemap.ReleaseAndGetAddressOf()));

		m_effect->SetTexture(m_cubemap.Get());
	}

	void ResetSkyBox()
	{
		m_sky.reset();
		m_effect.reset();
		m_skyInputLayout.Reset();
		m_cubemap.Reset();
	}

	void SetSkyboxEffect(
		const SimpleMath::Matrix& world,
		const SimpleMath::Matrix& view,
		const SimpleMath::Matrix& proj)
	{
		m_effect->SetWorld(world);
		m_effect->SetView(view);
		m_effect->SetProjection(proj);
	}

	void SetSkyBoxView(const SimpleMath::Matrix& view)
	{
		m_effect->SetView(view);
	}

	void SetSkyBoxProj(const SimpleMath::Matrix& proj)
	{
		m_effect->SetProjection(proj);
	}

	void DrawSkyBox(
		ID3D11DeviceContext* context,
		DirectX::CommonStates* states)
	{
		context->OMSetDepthStencilState(
			states->DepthRead(),
			0);

		context->RSSetState(
			states->CullNone());

		auto sampler = states->LinearClamp();

		context->PSSetSamplers(
			0,
			1,
			&sampler);

		m_sky->Draw(
			m_effect.get(),
			m_skyInputLayout.Get());
	}

	// インスタンス処理
public:

	// インスタンスを返す
	static Draw& GetInstance()
	{
		CreateInstance();
		return *m_draw;
	}

	// インスタンスを作成
	static void CreateInstance()
	{
		if (!m_draw)
		{
			m_draw = new Draw();
		}
	}

private:

	// コンストラクタ
	Draw() = default;
	// デストラクタ
	~Draw() = default;

	// 描画用コンテキスト
	std::unique_ptr<RenderingContext> renderingContext = nullptr;
	
	// 初期状態のビューポート
	D3D11_VIEWPORT m_initViewPort;

	// スカイボックス
	std::unique_ptr<DirectX::GeometricPrimitive> m_sky;
	std::unique_ptr<DX::SkyboxEffect> m_effect;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_skyInputLayout;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemap;

	static Draw* m_draw;

};