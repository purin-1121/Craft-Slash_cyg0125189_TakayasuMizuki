//====================================================//
// ファイル名   : UIHelper.h
// 作成者       : 高安瑞貴
//
// 概要 : UIを使う際のヘルパー関数クラス
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include <string>
#include "Lib/Camera.h"
#include "Tools/GameContext.h"
#include "Common/UI/Canvas.h"

//====================================================//
// 宣言
//====================================================//


using SimpleMath::Vector3;

// UIが呼び出す
namespace UIAnimation
{
	// 指定したキャンバスを、目標座標に向かって滑らかに近づける関数
    inline void InsertPos(Canvas& canvas, SimpleMath::Vector3& target, float speed, float max)
    {
       SimpleMath::Vector3 current = canvas.GetPosition();
        // 簡易的な補間計算
       SimpleMath::Vector3 next = Easing::EaseIn(speed, current, target - current, max);
       canvas.SetPosition(next);
    }
    // 指定したキャンバスを、目標座標に向かって滑らかに近づける関数
    inline void InsertPos(Canvas& canvas, SimpleMath::Vector3& target, float speed)
    {
       SimpleMath::Vector3 current = canvas.GetPosition();
        // 簡易的な補間計算
       SimpleMath::Vector3 next = SimpleMath::Vector3::Lerp(current, target, speed);
       canvas.SetPosition(next);
    }
};

