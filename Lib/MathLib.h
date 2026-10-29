//====================================================//
// ファイル名   : Math.h
// 作成者       : 高安瑞貴
// 作成日時     : 2026/05/29 18:58:04
//
// 概要 : 計算関係を管理する名前空間
//
// 更新履歴 :
// 2026/05/29 18:58:04 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include <cmath>
#include "SimpleMath.h"
using namespace DirectX;

//====================================================//
// 宣言
//====================================================//
namespace MathLib
{
	template <typename T, typename U>
	inline bool IsClamp(T a, U min, U max)
	{
		return a >= min && a < max;
	}

	template<typename T>
	inline T Clamp(T a, T min, T max)
	{
		return std::min(max, std::max(a, min));
	}

	template<typename T>
	inline T AddClamp(T a, T add, T min, T max)
	{
		//a += add;
		return Clamp(a, min, max);
	}
	template<typename T>
	inline T AddClamp(T a, T min, T max)
	{
		a++;
		return Clamp(a, min, max);
	}

	template<typename... Args>
	inline float Average(Args... next)
	{
		// 引数が0個の場合は0を返す
		if constexpr (sizeof...(next) == 0) {
			return 0.0;
		} else {
			// (args + ...) で全ての引数を合計し、個数で割る
			return static_cast<double>((next + ...)) / sizeof...(next);
		}
	}

	template<typename T>
	// 乗算関数
	inline T Pow(T a, int count)
	{
		T t = a;
		for (int i = 0; i < count - 1; i++)
		{
			t *= a;
		}
		return t;
	}

	// ベクトルのクランプ
	inline SimpleMath::Vector3 Vector3Clamp(SimpleMath::Vector3 vec, float max)
	{
		// 各ベクトルの長さ
		float vecLength = vec.Length();

		// 最大ベクトルより大きかったら
		if (vecLength > max)
		{
			// もともとのベクトルを正規化
			vec.Normalize();
			// 正規化させたものを最大ベクトルを返す
			return vec * max;
		}
		else
		{
			return vec;
		}
	}

	/// <summary>
	/// 操舵力を求める関数
	/// </summary>
	/// <param name="vec">速度</param>
	/// <param name="pos">オブジェクトの位置</param>
	/// <param name="target">目標とするターゲットの位置</param>
	/// <param name="maxSpeed">最大速度</param>
	/// <param name="maxForce">最大操舵力</param>
	/// <returns></returns>
	inline SimpleMath::Vector3 SetStreeringBehavier(SimpleMath::Vector3 vec, SimpleMath::Vector3 pos, SimpleMath::Vector3 target, float maxSpeed, float maxForce)
	{
		// ターゲットとアイテムの距離
		SimpleMath::Vector3 d = target - pos;
		// 正規化
		d.Normalize();
		// 最大速度になるように拡大
		SimpleMath::Vector3 desiredVelocity = d * maxSpeed;
		// 操舵力の計算
		SimpleMath::Vector3 steeringForve = desiredVelocity - vec;
		// 制限をかける
		steeringForve = MathLib::Vector3Clamp(steeringForve, maxForce);
		// 加速度の適用
		return MathLib::Vector3Clamp(vec + steeringForve, maxSpeed);
	}

	/// <summary>
	/// 二つのベクトルの角度を求める
	/// </summary>
	/// <param name="a">一つ目</param>
	/// <param name="b">二つ目</param>
	/// <returns>角度</returns>
	inline float BetweenAngle(SimpleMath::Vector3 a, SimpleMath::Vector3 b)
	{
		float lengthProd = a.Length() * b.Length();
		// どちらかがゼロベクトルなら、なす角は 0 とする（ゼロ除算回避）
		if (lengthProd < 1e-6f) return 0.0f;

		float cosTheta = a.Dot(b) / lengthProd;
		// 浮動小数点誤差による -1.0 ～ 1.0 の範囲外へのハミ出しをクランプ
		cosTheta = Clamp(cosTheta, -1.0f, 1.0f);

		return std::acos(cosTheta);
	}

	inline SimpleMath::Vector3 Sleap(SimpleMath::Vector3 p, SimpleMath::Vector3 p2, float t)
	{
		float angle = BetweenAngle(p, p2);

		// 角度がほぼ 0（またはベクトルの長さが 0）の場合は、通常の線形補間にフォールバック
		if (angle < 1e-6f)
		{
			return SimpleMath::Vector3::Lerp(p, p2, t);
		}

		return std::sin((1 - t) * angle) / std::sin(angle) * p + std::sin(t * angle) / std::sin(angle) * p2;
	}

	inline SimpleMath::Vector3 Sleap(SimpleMath::Vector3 start, SimpleMath::Vector3 end, float t, float max)
	{
		float angle = BetweenAngle(start, end);

		// 角度がほぼ 0（またはベクトルの長さが 0）の場合は、イージングにフォールバック
		if (angle < 1e-6f)
		{
			return Easing::EaseOut(t, start, end, max);
		}

		t /= max;
		t *= 1.3f;

		return std::sin((1 - t) * angle) / std::sin(angle) * start + std::sin(t * angle) / std::sin(angle) * end;
	}

	/// <summary>
	/// 点と線分の距離計算
	/// </summary>
	/// <param name="lineStart">線分の始点</param>
	/// <param name="lineDir">線分の方向ベクトル</param>
	/// <param name="point">任意の点</param>
	/// <param name="isSegment">無限直線か否か</param>
	/// <returns>距離</returns>
	inline XMVECTOR GetDistancePointToVectorDX(FXMVECTOR lineStart, FXMVECTOR lineDir, FXMVECTOR point, bool isSegment = true) {
    
		// 始点から点Pへのベクトル(AP)を計算
		XMVECTOR ap = XMVectorSubtract(point, lineStart);

		// 方向ベクトルの長さの2乗を計算 (SIMD)
		XMVECTOR dirLenSqVec = XMVector3LengthSq(lineDir);
		float dirLenSq = XMVectorGetX(dirLenSqVec); // floatとして取り出す

		if (dirLenSq < 0.00001f) {
			// ベクトルの長さがほぼ0の場合のフェールセーフ
			XMVECTOR distVec = XMVector3Length(ap);
			return distVec;
		}

		// 内積を計算し、比率 t を求める
		XMVECTOR dotVec = XMVector3Dot(ap, lineDir);
		float t = XMVectorGetX(dotVec) / dirLenSq;

		// 線分の場合はクランプ
		if (isSegment) {
			t = std::clamp(t, 0.0f, 1.0f);
		}

		// 最近接点 H の座標を計算 (H = lineDir * t + lineStart)
		// XMVectorMultiplyAdd を使うと「掛けて足す」を1つの命令で処理できて非常に高速です
		XMVECTOR tVec = XMVectorReplicate(t); // tを(t, t, t, t)のベクトルにする
		XMVECTOR h = XMVectorMultiplyAdd(lineDir, tVec, lineStart);

		// 任意の点P と 最近接点 H の間の距離を計算
		XMVECTOR distVec = XMVector3Length(XMVectorSubtract(point, h));
		return distVec;
	}

	inline float LenSegOnSeparateAxis(SimpleMath::Vector3* Sep, SimpleMath::Vector3* e1, SimpleMath::Vector3* e2, SimpleMath::Vector3* e3 = nullptr)
	{
		float r1 = std::abs(Sep->Dot(*e1));
		float r2 = std::abs(Sep->Dot(*e2));
		float r3 = e3 ? std::abs(Sep->Dot(*e3)) : 0;

		return r1 + r2 + r3;
	}

	// レイを使った交差計算
	inline bool UpdateRayRange(
		float start,
		float dir,
		float minBound,
		float maxBound,
		float& tMin,
		float& tMax)
	{
		if (std::abs(dir) < 0.000001f)
		{
			// その軸方向に進んでいない
			if (start < minBound || start > maxBound)
				return false;

			return true;
		}

		const float invDir = 1.0f / dir;

		float t1 = (minBound - start) * invDir;
		float t2 = (maxBound - start) * invDir;

		if (t1 > t2)
			std::swap(t1, t2);

		tMin = std::max(tMin, t1);
		tMax = std::min(tMax, t2);

		return tMin <= tMax;
	}

	// 等比級数の和
	inline float SumOfGeometricSeries(const float& x, const float& y)
	{
		return (Pow(x, y) - 1) / (x - 1);
	}

	// 小数点第２位まで丸め
	inline float RoundPosition(float value)
	{
		return std::round(value * 100.0f) / 100.0f;
	}

};
