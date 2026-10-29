#pragma once

#include "pch.h"
#include "NarrowPhase.h"
#include "Game/GamePlay/GameObject/GameObject.h"
#include <tchar.h>

CollisionInfo NarrowPhase::IsColliding(Collider* a, Collider* b)
{
    Type aT = a->GetType();
    Type bT = b->GetType();

    // Box同士の判定
    if (aT == Type::Box && bT == Type::Box)
    {
        BoxCollider* boxA = static_cast<BoxCollider*>(a);
        BoxCollider* boxB = static_cast<BoxCollider*>(b);
        return IsColliding(*boxA, *boxB);
    }
    // Box と Sphere の判定
    else if (aT == Type::Box && bT == Type::Sphere)
    {
        BoxCollider* boxA = static_cast<BoxCollider*>(a);
        SphereCollider* sphereB = static_cast<SphereCollider*>(b);
        return IsColliding(*boxA, *sphereB);
    }
    // SphereとBoxの判定
    else if (aT == Type::Sphere && bT == Type::Box)
    {
        SphereCollider* sphereA = static_cast<SphereCollider*>(a);
        BoxCollider* boxB = static_cast<BoxCollider*>(b);
        return IsColliding(*boxB, *sphereA);
    }
    // SphereとSphereの判定
    else if (aT == Type::Sphere && bT == Type::Sphere)
    {
        SphereCollider* sphereA = static_cast<SphereCollider*>(a);
        SphereCollider* sphereB = static_cast<SphereCollider*>(b);
        return IsColliding(*sphereA, *sphereB);
    }
    // CapsuleとBoxの判定
    else if (aT == Type::Capsule && bT == Type::Box)
    {
        CapsuleCollider* CapsuleA = static_cast<CapsuleCollider*>(a);
        BoxCollider* boxB = static_cast<BoxCollider*>(b);
        return IsColliding(*CapsuleA, *boxB);
    }
    // BoxとCapsuleの判定
    else if (aT == Type::Box && bT == Type::Capsule)
    {
        CapsuleCollider* CapsuleB = static_cast<CapsuleCollider*>(b);
        BoxCollider* boxA = static_cast<BoxCollider*>(a);
        return IsColliding(*CapsuleB, *boxA);
    }
    // CapsuleとSphereの判定
    else if (aT == Type::Capsule && bT == Type::Sphere)
    {
        CapsuleCollider* capsuleA = static_cast<CapsuleCollider*>(a);
        SphereCollider* sphereB = static_cast<SphereCollider*>(b);
        return IsColliding(*capsuleA, *sphereB);
    }
    // SphereとCapsuleの判定
    else if (aT == Type::Sphere && bT == Type::Capsule)
    {
        CapsuleCollider* capsuleB = static_cast<CapsuleCollider*>(b);
        SphereCollider* sphereA = static_cast<SphereCollider*>(a);
        return IsColliding(*capsuleB, *sphereA);
    }
    // CylinderとBoxの判定
    else if (aT == Type::Cylinder && bT == Type::Box)
    {
        CylinderCollider* CapsuleA = static_cast<CylinderCollider*>(a);
        BoxCollider* boxB = static_cast<BoxCollider*>(b);
        return IsColliding(*CapsuleA, *boxB);
    }
    // CylinderとBoxの判定
    else if (aT == Type::Box && bT == Type::Cylinder)
    {
        CylinderCollider* CapsuleB = static_cast<CylinderCollider*>(b);
        BoxCollider* boxA = static_cast<BoxCollider*>(a);
        return IsColliding(*CapsuleB, *boxA);
    }
    else if (aT == Type::OBB && bT == Type::OBB)
    {
        OBBCollider* obbA = static_cast<OBBCollider*>(a);
        OBBCollider* obbB = static_cast<OBBCollider*>(b);
        return IsColliding(*obbA, *obbB);
    }
    else if (aT == Type::OBB && bT == Type::Box)
    {
        OBBCollider* obb = static_cast<OBBCollider*>(a);
        BoxCollider* box = static_cast<BoxCollider*>(b);
        return IsColliding(*obb, *box);
    }
    else if (aT == Type::Box && bT == Type::OBB)
    {
        BoxCollider* box = static_cast<BoxCollider*>(a);
        OBBCollider* obb = static_cast<OBBCollider*>(b);
        return ReverseCollisionInfo(IsColliding(*obb, *box));
    }
    else if (aT == Type::OBB && bT == Type::Sphere)
    {
        OBBCollider* obb = static_cast<OBBCollider*>(a);
        SphereCollider* Sphere = static_cast<SphereCollider*>(b);
        return IsColliding(*obb, *Sphere);
    }
    else if (aT == Type::Sphere && bT == Type::OBB)
    {
        SphereCollider* Sphere = static_cast<SphereCollider*>(a);
        OBBCollider* obb = static_cast<OBBCollider*>(b);
        return ReverseCollisionInfo(IsColliding(*obb, *Sphere));
    }
    else if (aT == Type::OBB && bT == Type::Cylinder)
    {
        OBBCollider* obb = static_cast<OBBCollider*>(a);
        CylinderCollider* Cylinder = static_cast<CylinderCollider*>(b);
        return IsColliding(*obb, *Cylinder);
    }
    else if (aT == Type::Cylinder && bT == Type::OBB)
    {
        CylinderCollider* Cylinder = static_cast<CylinderCollider*>(a);
        OBBCollider* obb = static_cast<OBBCollider*>(b);
        return ReverseCollisionInfo(IsColliding(*obb, *Cylinder));
    }

    return CollisionInfo{};
}

CollisionInfo NarrowPhase::IsColliding(const SimpleMath::Vector2& point, const PlaneCollider& plane)
{
    auto planePos = plane.GetPos();
    auto planeSize = plane.GetSize();

    CollisionInfo info{};
    info.is = point.x >= planePos.x &&
        point.x <= planePos.x + planeSize.x &&
        point.y >= planePos.y &&
        point.y <= planePos.y + planeSize.y;

    return  info;
}

CollisionInfo NarrowPhase::IsColliding(PlaneCollider& planeA, PlaneCollider& planeB)
{
    auto aPos = planeA.GetPos();
    auto aOffset = planeA.GetPosOffset();
    auto aSize = planeA.GetSize();
    auto bPos = planeB.GetPos();
    auto bOffset = planeB.GetPosOffset();
    auto bSize = planeB.GetSize();
    
    CollisionInfo info{};
    info.is = aPos.x + aOffset.x + aSize.x >= bPos.x + bOffset.x &&
        aPos.x + aOffset.x <= bPos.x + bSize.x + bOffset.x &&
        aPos.y + aOffset.y + aSize.y >= bPos.y + bOffset.y &&
        aPos.y + aOffset.y <= bPos.y + bSize.y + bOffset.y;

    if (!info.is)return info;

    float overlapX = (planeA.GetSize().x + planeB.GetSize().x) + std::abs(planeB.GetCenter().x - planeA.GetCenter().x);
    float overlapY = (planeA.GetSize().y + planeB.GetSize().y) + std::abs(planeB.GetCenter().y - planeA.GetCenter().y);
    float penetration = std::min(overlapX, overlapY);

    info.normal = planeB.GetCenter() - planeA.GetCenter();
    info.normal.Normalize();
    info.push = info.normal * penetration;

    return info;
}

CollisionInfo NarrowPhase::IsColliding(SimpleMath::Vector3& point, BoxCollider& box)
{
    auto boxPos = box.GetPos(); 
    auto boxOffset = box.GetPosOffset();
    auto boxSize = box.GetSize();

    return CollisionInfo{
        point.x >= boxPos.x + boxOffset.x &&
        point.x <= boxPos.x + boxOffset.x + boxSize.x &&
        point.y >= boxPos.y + boxOffset.y &&
        point.y <= boxPos.y + boxOffset.y + boxSize.y &&
        point.z >= boxPos.z + boxOffset.z &&
        point.z <= boxPos.z + boxOffset.z + boxSize.z, SimpleMath::Vector3::Zero, SimpleMath::Vector3::Zero };
}

CollisionInfo NarrowPhase::IsColliding(BoxCollider& boxA, BoxCollider& boxB)
{
    const auto aPos = boxA.GetPos();
    const auto aOffset = boxA.GetPosOffset();
    const auto aSize = boxA.GetSize();

    const auto bPos = boxB.GetPos();
    const auto bOffset = boxB.GetPosOffset();
    const auto bSize = boxB.GetSize();

    CollisionInfo info{};

    // ワールド座標上の最小・最大
    const SimpleMath::Vector3 aMin = aPos + aOffset;
    const SimpleMath::Vector3 aMax = aMin + aSize;

    const SimpleMath::Vector3 bMin = bPos + bOffset;
    const SimpleMath::Vector3 bMax = bMin + bSize;

    // AABB衝突判定
    info.is =
        aMin.x <= bMax.x && aMax.x >= bMin.x &&
        aMin.y <= bMax.y && aMax.y >= bMin.y &&
        aMin.z <= bMax.z && aMax.z >= bMin.z;

    if (!info.is)
        return info;

    // 中心
    const SimpleMath::Vector3 aCenter = (aMin + aMax) * 0.5f;
    const SimpleMath::Vector3 bCenter = (bMin + bMax) * 0.5f;

    // 各軸の重なり量
    const float ox =
        std::min(aMax.x, bMax.x) -
        std::max(aMin.x, bMin.x);

    const float oy =
        std::min(aMax.y, bMax.y) -
        std::max(aMin.y, bMin.y);

    const float oz =
        std::min(aMax.z, bMax.z) -
        std::max(aMin.z, bMin.z);

    float penetration = 0.0f;

    if (ox < oy && ox < oz)
    {
        penetration = ox;

        info.normal =
            (aCenter.x > bCenter.x)
            ? SimpleMath::Vector3(1, 0, 0)
            : SimpleMath::Vector3(-1, 0, 0);
    }
    else if (oy < oz)
    {
        penetration = oy;

        info.normal =
            (aCenter.y > bCenter.y)
            ? SimpleMath::Vector3(0, 1, 0)
            : SimpleMath::Vector3(0, -1, 0);
    }
    else
    {
        penetration = oz;

        info.normal =
            (aCenter.z > bCenter.z)
            ? SimpleMath::Vector3(0, 0, 1)
            : SimpleMath::Vector3(0, 0, -1);
    }

    constexpr float percent = 0.8f;
    constexpr float slop = 0.005f;

    info.push =
        std::max(penetration - slop, 0.0f)
        * percent
        * info.normal;

    return info;
}
CollisionInfo NarrowPhase::IsColliding(BoxCollider& box, SphereCollider& sphere)
{
    // 球の中心座標
    SimpleMath::Vector3 centerBox = box.GetCenter() + box.GetPosOffset();
    SimpleMath::Vector3 centerSphere = sphere.GetCenter() + sphere.GetPosOffset();

    // ボックスの半径
    SimpleMath::Vector3 half = box.GetSize() / 2;

    // 球の中心座標に対する最短座標を計算
    SimpleMath::Vector3 nearPos =
    {
        MathLib::Clamp(centerSphere.x, -half.x + centerBox.x,half.x + centerBox.x),
        MathLib::Clamp(centerSphere.y, -half.y + centerBox.y,half.y + centerBox.y),
        MathLib::Clamp(centerSphere.z, -half.z + centerBox.z,half.z + centerBox.z)
    };

    SimpleMath::Vector3 dir = centerSphere - nearPos;
    float dist = dir.Length();
    float radius = sphere.GetRadius();

    CollisionInfo info{};
    info.is = dist <= radius * radius;

    if (!info.is) return info;

    // 距離があったら
    if (dist > 0.0001f)
    {
        // 球からボックスへの向き
        SimpleMath::Vector3 normal = dir / dist;
        // 距離
        float penetration = sphere.GetRadius() - dist;

        if (penetration > 0.0f)
        {
            info.normal = normal;
            info.push = normal * penetration;
        }
    }
    // 内部に入り込んだときの処理
    else
    {
        // オブジェクトの中心同士のベクトル
        SimpleMath::Vector3 dirFromCenter = centerBox - centerSphere;
        // 距離が小さかったら架空のベクトルを設定
        if (dirFromCenter.LengthSquared() < 0.0001f)
        {
            dirFromCenter = SimpleMath::Vector3::UnitY;
        }
        // 正規化
        dirFromCenter.Normalize();

        info.normal = dirFromCenter;
        info.push = dirFromCenter * sphere.GetRadius();
    }

    return info;
}

CollisionInfo NarrowPhase::IsColliding(SphereCollider& a, SphereCollider& b)
{
    SimpleMath::Vector3 centerA = a.GetCenter() + a.GetPosOffset();
    SimpleMath::Vector3 centerB = b.GetCenter() + b.GetPosOffset();

    SimpleMath::Vector3 dir = centerA - centerB;
    float dist = dir.Length();
    float rSum = a.GetRadius() + b.GetRadius();

    CollisionInfo info{};
    info.is = dist <= rSum;
    if (!info.is) return info;

    if (dist > 0.0001f)
    {
        info.normal = dir / dist;
    }
    else
    {
        info.normal = SimpleMath::Vector3::UnitY;
    }

    info.push = info.normal * (rSum - dist);
    return info;
}

CollisionInfo NarrowPhase::IsColliding(CapsuleCollider& capsuleA, CapsuleCollider& capsuleB)
{
    // それぞれの始点と終点との差ベクトル
    SimpleMath::Vector3 a_d = capsuleA.GetSegmentEnd() - capsuleA.GetSegmentStart();
    SimpleMath::Vector3 b_d = capsuleB.GetSegmentEnd() - capsuleB.GetSegmentStart();
    // 二つのカプセルの半径の長さ
    SimpleMath::Vector3 r = capsuleA.GetSegmentStart() - capsuleB.GetSegmentStart();

    // それぞれの距離の二乗
    float a_to_b = a_d.Dot(a_d);
    float b_to_a = b_d.Dot(b_d);
    // (何これ？)
    float f = b_d.Dot(r);

    // 誤差対策用の定数
    static constexpr float EPS = 0.001f;

    float s = 0;
    float t = 0;

    // 両方とも距離が誤差対策以下だったら球扱い
    if (a_to_b <= EPS && b_to_a <= EPS)
    {
        s = 0;
        t = 0;
    }
    // 対象だけ誤差以下だったら
    else if (a_to_b <= EPS)
    {
        s = 0;
        t = MathLib::Clamp(f / b_to_a, 0.0f, 1.0f);
    }
    else
    {
        // (これは何？)
        float c = a_d.Dot(r);
        if (b_to_a <= EPS)
        {
            t = 0;
            // (なんで-c?)
            s = MathLib::Clamp(-c / a_to_b, 0.0f, 1.0f);
        }
        else
        {
            float d = a_d.Dot(b_d);
            float denom = a_to_b * b_to_a - d * d;
            if (denom != 0)
            {
                s = MathLib::Clamp((d * f - c * b_to_a) / denom, 0.0f, 1.0f);
            }
            else
            {
                s = 0;  // 平行な場合
            }
            t = (d * s + f) / b_to_a;  // ← ifの外に出す(両方のケースで必要)

            if (t < 0)
            {
                t = 0;
                s = MathLib::Clamp(-c / a_to_b, 0.0f, 1.0f);
            }
            else if (t > 1)
            {
                t = 1;
                s = MathLib::Clamp((d - c) / a_to_b, 0.0f, 1.0f);
            }

        }
    }

    SimpleMath::Vector3 closestA = capsuleA.GetSegmentStart() + s * a_d;
    SimpleMath::Vector3 closestB = capsuleB.GetSegmentStart() + t * b_d;
    float distance = (closestA - closestB).Length();

    CollisionInfo info{};
    info.is = distance <= (capsuleA.GetRadius() + capsuleB.GetRadius());

    if (!info.is) return info;

    float r_sum = capsuleA.GetRadius() + capsuleB.GetRadius();

    float penetration = r_sum - distance;
    info.normal = (closestA - closestB);
    info.normal.Normalize();

    info.push = info.normal * penetration;

    return info;
}

CollisionInfo NarrowPhase::IsColliding(CapsuleCollider& capsule, SphereCollider& sphere)
{
    SimpleMath::Vector3 segStart = capsule.GetSegmentStart();
    SimpleMath::Vector3 segEnd = capsule.GetSegmentEnd();
    SimpleMath::Vector3 sphereCenter = sphere.GetCenter();

    SimpleMath::Vector3 d = segEnd - segStart;
    float lenSq = d.Dot(d);

    static constexpr float EPS = 0.0001f;
    float t = 0.0f;

    // 芯の長さがほぼ0(球扱い)でなければ、射影パラメータtを計算
    if (lenSq > EPS)
    {
        t = (sphereCenter - segStart).Dot(d) / lenSq;
        t = MathLib::Clamp(t, 0.0f, 1.0f);
    }

    // 球の中心に一番近い、カプセルの芯上の点
    SimpleMath::Vector3 closestPoint = segStart + t * d;

    float distance = (closestPoint - sphereCenter).Length();

    CollisionInfo info{};
    info.is = distance <= (capsule.GetRadius() + sphere.GetRadius());

    if (!info.is) return info;

    float r_sum = capsule.GetRadius() + sphere.GetRadius();
    float penetration = r_sum - distance;

    info.normal = d / distance;
    info.push = info.normal * penetration;

    return info;
}

CollisionInfo NarrowPhase::IsColliding(CapsuleCollider& capsule, BoxCollider& box)
{
    SimpleMath::Vector3 segStart = capsule.GetSegmentStart();
    SimpleMath::Vector3 segEnd = capsule.GetSegmentEnd();

    // ボックスの範囲(最小座標・最大座標)
    SimpleMath::Vector3 boxMin = box.GetPos();               // 角(コーナー)
    SimpleMath::Vector3 boxMax = boxMin + box.GetSize();     // 角 + サイズ

    SimpleMath::Vector3 d = segEnd - segStart;
    float lenSq = d.Dot(d);
    static constexpr float EPS = 0.0001f;

    SimpleMath::Vector3 p = segStart; // 線分側の暫定最近接点

    // 「ボックス側にクランプ」→「線分側にクランプし直す」を数回繰り返して収束させる
    for (int i = 0; i < 4; ++i)
    {
        // pに一番近い、ボックス内の点(はみ出た分だけ押し戻す)
        SimpleMath::Vector3 q
        {
            MathLib::Clamp(p.x, boxMin.x, boxMax.x),
            MathLib::Clamp(p.y, boxMin.y, boxMax.y),
            MathLib::Clamp(p.z, boxMin.z, boxMax.z)
        };

        // qに一番近い、線分上の点
        float t = 0.0f;
        if (lenSq > EPS)
        {
            t = (q - segStart).Dot(d) / lenSq;
            t = MathLib::Clamp(t, 0.0f, 1.0f);
        }
        p = segStart + t * d;
    }

    // 最終的なpに対するボックス側の最近接点をもう一度求める
    SimpleMath::Vector3 closestOnBox
    {
        MathLib::Clamp(p.x, boxMin.x, boxMax.x),
        MathLib::Clamp(p.y, boxMin.y, boxMax.y),
        MathLib::Clamp(p.z, boxMin.z, boxMax.z)
    };

    float distance = (p - closestOnBox).Length();

    CollisionInfo info{};
    info.is = distance <= capsule.GetRadius();
    if (!info.is) return info;

    float penetration = capsule.GetRadius() - distance;
    info.normal = p / distance;
    info.push = info.normal * penetration;

    return info;
}

CollisionInfo NarrowPhase::IsColliding(CylinderCollider& cylinder, BoxCollider& box)
{
    SimpleMath::Vector3 bottom = cylinder.GetBottom();
    SimpleMath::Vector3 top = cylinder.GetTop();
    SimpleMath::Vector3 axis = cylinder.GetAxis();
    float height = cylinder.GetHeight();
    float radius = cylinder.GetRadius();

    SimpleMath::Vector3 boxMin = box.GetPos();
    SimpleMath::Vector3 boxMax = boxMin + box.GetSize();

    SimpleMath::Vector3 d = top - bottom;
    float lenSq = d.Dot(d);
    static constexpr float EPS = 0.0001f;

    SimpleMath::Vector3 p = bottom; // 軸側の暫定最近接点

    // 「ボックス側にクランプ」→「軸側にクランプし直す」を繰り返して収束させる
    for (int i = 0; i < 4; ++i)
    {
        SimpleMath::Vector3 q
        {
            MathLib::Clamp(p.x, boxMin.x, boxMax.x),
            MathLib::Clamp(p.y, boxMin.y, boxMax.y),
            MathLib::Clamp(p.z, boxMin.z, boxMax.z)
        };

        float t = 0.0f;
        if (lenSq > EPS)
        {
            t = (q - bottom).Dot(d) / lenSq;
            t = MathLib::Clamp(t, 0.0f, 1.0f); // 円柱は端点クランプでOK(蓋が平らなので)
        }
        p = bottom + t * d;
    }

    SimpleMath::Vector3 closestOnBox
    {
        MathLib::Clamp(p.x, boxMin.x, boxMax.x),
        MathLib::Clamp(p.y, boxMin.y, boxMax.y),
        MathLib::Clamp(p.z, boxMin.z, boxMax.z)
    };

    float distance = (p - closestOnBox).Length();

    CollisionInfo info;
    info.is = distance <= radius;

    return info;
}

CollisionInfo NarrowPhase::IsColliding(OBBCollider& obbA, OBBCollider& obbB)
{
    // OBBの中心間ベクトル
    SimpleMath::Vector3 interval = obbA.GetPos() - obbB.GetPos();

    // 回転行列
    SimpleMath::Matrix rotA = obbA.CreateRotMatrix();
    SimpleMath::Matrix rotB = obbB.CreateRotMatrix();

    // OBBの各ローカル軸をワールド空間へ変換
    SimpleMath::Vector3 NAx = SimpleMath::Vector3::Transform(obbA.GetAxisX(), rotA);
    SimpleMath::Vector3 NAy = SimpleMath::Vector3::Transform(obbA.GetAxisY(), rotA);
    SimpleMath::Vector3 NAz = SimpleMath::Vector3::Transform(obbA.GetAxisZ(), rotA);
    SimpleMath::Vector3 NBx = SimpleMath::Vector3::Transform(obbB.GetAxisX(), rotB);
    SimpleMath::Vector3 NBy = SimpleMath::Vector3::Transform(obbB.GetAxisY(), rotB);
    SimpleMath::Vector3 NBz = SimpleMath::Vector3::Transform(obbB.GetAxisZ(), rotB);

    // 各軸方向の半径ベクトル
    SimpleMath::Vector3 Ax = NAx * obbA.GetHalfExtents().x;
    SimpleMath::Vector3 Ay = NAy * obbA.GetHalfExtents().y;
    SimpleMath::Vector3 Az = NAz * obbA.GetHalfExtents().z;
    SimpleMath::Vector3 Bx = NBx * obbB.GetHalfExtents().x;
    SimpleMath::Vector3 By = NBy * obbB.GetHalfExtents().y;
    SimpleMath::Vector3 Bz = NBz * obbB.GetHalfExtents().z;

    // 最小めり込み量と、そのときの軸
    SimpleMath::Vector3 bestAxis{};
    float minPenetration = FLT_MAX;

    // めり込み量を更新するラムダ
    auto judge =
        [&minPenetration, &bestAxis]
        (float penetration, SimpleMath::Vector3 axis)
        {
            if (penetration < minPenetration)
            {
                minPenetration = penetration;
                bestAxis = axis;
            }
        };

    CollisionInfo info{};
    info.is = false;

    // 通常のSAT軸を判定
    auto judgeAxis =
        [&](SimpleMath::Vector3 axis,
            float rA,
            float rB)
        {
            float distance =  std::abs(interval.Dot(axis));

            // 分離している
            if (distance > rA + rB)
                return false;

            // めり込み量
            judge(rA + rB - distance,axis);

            return true;
        };

    // ======================================
    // Cross軸用の判定
    // ======================================
    constexpr float EPSILON = 1e-6f;

    auto judgeCrossAxis =
        [&](SimpleMath::Vector3 axis,
            SimpleMath::Vector3 a0,
            SimpleMath::Vector3 a1,
            SimpleMath::Vector3 b0,
            SimpleMath::Vector3 b1)
        {
            // Cross軸がほぼゼロなら無視
            float lengthSq = axis.LengthSquared();

            if (lengthSq < EPSILON)
            {
                return true;
            }

            // Cross軸を正規化
            axis /= std::sqrt(lengthSq);

            // A側の射影半径
            float rA = MathLib::LenSegOnSeparateAxis(&axis,&a0,&a1);

            // B側の射影半径
            float rB = MathLib::LenSegOnSeparateAxis(&axis,&b0,&b1);

            // 中心間距離を軸へ射影
            float distance = std::abs(interval.Dot(axis));

            // 分離していたら衝突していない
            if (distance > rA + rB)
                return false;

            // めり込み量を保存
            judge(rA + rB - distance,axis);

            return true;
        };

    // AのX軸
    if (!judgeAxis(NAx,obbA.GetHalfExtents().x,MathLib::LenSegOnSeparateAxis(&NAx,&Bx,&By,&Bz)))
        return info;

    // AのY軸
    if (!judgeAxis(NAy,obbA.GetHalfExtents().y,MathLib::LenSegOnSeparateAxis(&NAy,&Bx,&By,&Bz)))
        return info;
    
    // AのZ軸
    if (!judgeAxis(NAz,obbA.GetHalfExtents().z,MathLib::LenSegOnSeparateAxis(&NAz,&Bx,&By,&Bz)))
        return info;

    // BのX軸
    if (!judgeAxis(NBx,MathLib::LenSegOnSeparateAxis(&NBx,&Ax,&Ay,&Az),obbB.GetHalfExtents().x))
        return info;

    // BのY軸
    if (!judgeAxis(NBy,MathLib::LenSegOnSeparateAxis(&NBy, &Ax, &Ay, &Az), obbB.GetHalfExtents().y))
        return info;

    // BのZ軸
    if (!judgeAxis(NBz,MathLib::LenSegOnSeparateAxis(&NBz,&Ax,&Ay,&Az),obbB.GetHalfExtents().z))
        return info;

    // Ax × Bx
    SimpleMath::Vector3 cross{};
    cross.Cross(NAx, NBx);
    if (!judgeCrossAxis(cross,Ay, Az,By, Bz))
        return info;

    // Ax × By
    cross.Cross(NAx, NBy);
    if (!judgeCrossAxis(cross,Ay, Az,Bx, Bz))
        return info;

    // Ax × Bz
    cross.Cross(NAx, NBz);
    if (!judgeCrossAxis(cross,Ay, Az,Bx, By))
        return info;

    // Ay × Bx
    cross.Cross(NAy, NBx);
    if (!judgeCrossAxis(cross,Ax, Az,By, Bz))
        return info;

    // Ay × By
    cross.Cross(NAy, NBy);
    if (!judgeCrossAxis(cross,Ax, Az,Bx, Bz))
        return info;

    // Ay × Bz
    cross.Cross(NAy, NBz);
    if (!judgeCrossAxis(cross,Ax, Az,Bx, By))
        return info;

    // Az × Bx
    cross.Cross(NAz, NBx);
    if (!judgeCrossAxis(cross,Ax, Ay,By, Bz))
        return info;
    
    // Az × By
    cross.Cross(NAz, NBy);
    if (!judgeCrossAxis(cross,Ax, Ay,Bx, Bz))
        return info;

    // ⑮ Az × Bz
    cross.Cross(NAz, NBz);
    if (!judgeCrossAxis(cross,Ax, Ay,Bx, By))
        return info;

    info.is = true;

    // 法線の向きを B → A に統一
    info.normal = bestAxis;
    if (info.normal.Dot(interval) < 0.0f)
        info.normal = -info.normal;

    // 押し戻し量
    info.push = info.normal * minPenetration;

    return info;
}

CollisionInfo NarrowPhase::IsColliding(OBBCollider& obb, BoxCollider& box)
{
    // 中心間ベクトル
    // B(AABB) → A(OBB)
    SimpleMath::Vector3 interval = obb.GetPos() - box.GetCenter();

    // OBBの回転行列
    SimpleMath::Matrix rot = obb.CreateRotMatrix();

    // OBBのワールド軸
    SimpleMath::Vector3 NAx = SimpleMath::Vector3::Transform(obb.GetAxisX(), rot);
    SimpleMath::Vector3 NAy = SimpleMath::Vector3::Transform(obb.GetAxisY(), rot);
    SimpleMath::Vector3 NAz = SimpleMath::Vector3::Transform(obb.GetAxisZ(), rot);

    // OBBの半径ベクトル
    SimpleMath::Vector3 Ax = NAx * obb.GetHalfExtents().x;
    SimpleMath::Vector3 Ay = NAy * obb.GetHalfExtents().y;
    SimpleMath::Vector3 Az = NAz * obb.GetHalfExtents().z;

    // AABBの半サイズ
    SimpleMath::Vector3 boxHalf = box.GetSize() / 2.0f;

    // AABBの半径ベクトル
    SimpleMath::Vector3 Bx(1.0f, 0.0f, 0.0f);
    SimpleMath::Vector3 By(0.0f, 1.0f, 0.0f);
    SimpleMath::Vector3 Bz(0.0f, 0.0f, 1.0f);

    // AABBの正規化された軸
    SimpleMath::Vector3 NBx(1.0f, 0.0f, 0.0f);
    SimpleMath::Vector3 NBy(0.0f, 1.0f, 0.0f);
    SimpleMath::Vector3 NBz(0.0f, 0.0f, 1.0f);

    // 最小めり込み量
    float minPenetration = FLT_MAX;
    SimpleMath::Vector3 bestAxis{};

    // めり込み量と押し出し方向を更新
    auto judge = [&](float penetration, SimpleMath::Vector3 axis)
        {
            if (penetration < minPenetration)
            {
                minPenetration = penetration;
                bestAxis = axis;
            }
        };

    CollisionInfo info{};
    info.is = false;

    // 通常のSAT軸
    auto judgeAxis = [&](SimpleMath::Vector3 axis, float rA, float rB)
        {
            float centerDistance = interval.Dot(axis);
            float distance = std::abs(centerDistance);

            // 分離している
            if (distance > rA + rB)
                return false;

            // この軸方向で分離するために必要な最小移動量
            float penetration = rA + rB - distance;

            // AをBから離す方向にする
            //
            // centerDistance > 0
            // → Aはaxis方向にある
            //
            // centerDistance < 0
            // → Aは-axis方向にある
            //
            // centerDistance == 0
            // → 中心が完全一致しているため、
            //    この軸の向きをそのまま使用する
            if (centerDistance < 0.0f)
                axis = -axis;

            judge(penetration, axis);
            return true;
        };

    // Cross軸
    constexpr float EPSILON = 1e-6f;

    auto judgeCrossAxis = [&](SimpleMath::Vector3 axis, SimpleMath::Vector3 a0, SimpleMath::Vector3 a1, SimpleMath::Vector3 b0, SimpleMath::Vector3 b1)
        {
            // Cross軸がほぼゼロなら、この軸は不要
            float lengthSq = axis.LengthSquared();

            if (lengthSq < EPSILON)
                return true;

            // Cross軸を正規化
            axis /= std::sqrt(lengthSq);

            // OBB側の射影半径
            float rA = MathLib::LenSegOnSeparateAxis(&axis, &a0, &a1);

            // AABB側の射影半径
            float rB = MathLib::LenSegOnSeparateAxis(&axis, &b0, &b1);

            float centerDistance = interval.Dot(axis);
            float distance = std::abs(centerDistance);

            // 分離している
            if (distance > rA + rB)
                return false;

            // この軸方向で分離するために必要な最小移動量
            float penetration = rA + rB - distance;

            // AをBから離す方向にする
            if (centerDistance < 0.0f)
                axis = -axis;

            judge(penetration, axis);
            return true;
        };

    // OBB X軸
    if (!judgeAxis(NAx, obb.GetHalfExtents().x, MathLib::LenSegOnSeparateAxis(&NAx, &Bx, &By, &Bz)))
        return info;

    // OBB Y軸
    if (!judgeAxis(NAy, obb.GetHalfExtents().y, MathLib::LenSegOnSeparateAxis(&NAy, &Bx, &By, &Bz)))
        return info;

    // OBB Z軸
    if (!judgeAxis(NAz, obb.GetHalfExtents().z, MathLib::LenSegOnSeparateAxis(&NAz, &Bx, &By, &Bz)))
        return info;

    // AABB X軸
    if (!judgeAxis(NBx, MathLib::LenSegOnSeparateAxis(&NBx, &Ax, &Ay, &Az), boxHalf.x))
        return info;

    // AABB Y軸
    if (!judgeAxis(NBy, MathLib::LenSegOnSeparateAxis(&NBy, &Ax, &Ay, &Az), boxHalf.y))
        return info;

    // AABB Z軸
    if (!judgeAxis(NBz, MathLib::LenSegOnSeparateAxis(&NBz, &Ax, &Ay, &Az), boxHalf.z))
        return info;

    SimpleMath::Vector3 cross{};

    // Ax × Bx
    cross.Cross(NAx, NBx);
    if (!judgeCrossAxis(cross, Ay, Az, By, Bz))
        return info;

    // Ax × By
    cross.Cross(NAx, NBy);
    if (!judgeCrossAxis(cross, Ay, Az, Bx, Bz))
        return info;

    // Ax × Bz
    cross.Cross(NAx, NBz);
    if (!judgeCrossAxis(cross, Ay, Az, Bx, By))
        return info;

    // Ay × Bx
    cross.Cross(NAy, NBx);
    if (!judgeCrossAxis(cross, Ax, Az, By, Bz))
        return info;

    // Ay × By
    cross.Cross(NAy, NBy);
    if (!judgeCrossAxis(cross, Ax, Az, Bx, Bz))
        return info;

    // Ay × Bz
    cross.Cross(NAy, NBz);
    if (!judgeCrossAxis(cross, Ax, Az, Bx, By))
        return info;

    // Az × Bx
    cross.Cross(NAz, NBx);
    if (!judgeCrossAxis(cross, Ax, Ay, By, Bz))
        return info;

    // Az × By
    cross.Cross(NAz, NBy);
    if (!judgeCrossAxis(cross, Ax, Ay, Bx, Bz))
        return info;

    // Az × Bz
    cross.Cross(NAz, NBz);
    if (!judgeCrossAxis(cross, Ax, Ay, Bx, By))
        return info;

    // 15軸すべてで分離しなかった
    info.is = true;

    // SATで求めた最小押し出し方向
    info.normal = bestAxis;

    // 押し戻し量
    info.push = info.normal * minPenetration;

    OutputDebugStringA(
        ("interval = " +
            std::to_string(interval.x) + ", " +
            std::to_string(interval.y) + ", " +
            std::to_string(interval.z) + "\n" +
            "normal = " +
            std::to_string(info.normal.x) + ", " +
            std::to_string(info.normal.y) + ", " +
            std::to_string(info.normal.z) + "\n" +
            "penetration = " +
            std::to_string(minPenetration) + "\n").c_str()
    );

    return info;
}

CollisionInfo NarrowPhase::IsColliding(OBBCollider& obb, SphereCollider& sphere)
{
    using namespace DirectX::SimpleMath;

    // ボックスと球の中心
    Vector3 centerBox = obb.GetCenter() + obb.GetPosOffset();
    Vector3 centerSphere = sphere.GetCenter() + sphere.GetPosOffset();
    // ボックスと円の半径
    Vector3 half = obb.GetSize() / 2;
    float   radius = sphere.GetRadius();

    // OBB のローカル軸(列ベクトル。行/列の並びは環境に合わせる)
    auto euler = obb.GetRotation();
    Matrix rot = Matrix::CreateFromYawPitchRoll(euler.y, euler.x, euler.z);
    Vector3 axis[3] = { rot.Right(), rot.Up(), rot.Forward() };

    // 球の中心をボックスのローカル空間へ
    Vector3 d = centerSphere - centerBox;
    Vector3 local = { d.Dot(axis[0]), d.Dot(axis[1]), d.Dot(axis[2]) };

    // ローカル空間で最近接点
    Vector3 nearLocal =
    {
        MathLib::Clamp(local.x, -half.x, half.x),
        MathLib::Clamp(local.y, -half.y, half.y),
        MathLib::Clamp(local.z, -half.z, half.z)
    };

    Vector3 diffLocal = local - nearLocal;
    float distSq = diffLocal.LengthSquared();

    CollisionInfo info{};
    info.is = distSq <= radius * radius;
    if (!info.is) return info;

    Vector3 normalLocal;
    float penetration;

    if (distSq > 0.0001f * 0.0001f)
    {
        // 球の中心がボックスの外側
        float dist = std::sqrt(distSq);
        normalLocal = diffLocal / dist;
        penetration = radius - dist;
    }
    else
    {
        // 球の中心がボックスの内側:最も浅い面から押し出す
        float dx = half.x - std::abs(local.x);
        float dy = half.y - std::abs(local.y);
        float dz = half.z - std::abs(local.z);

        if (dx <= dy && dx <= dz)
        {
            normalLocal = { local.x >= 0 ? 1.0f : -1.0f, 0, 0 };
            penetration = dx + radius;
        }
        else if (dy <= dz)
        {
            normalLocal = { 0, local.y >= 0 ? 1.0f : -1.0f, 0 };
            penetration = dy + radius;
        }
        else
        {
            normalLocal = { 0, 0, local.z >= 0 ? 1.0f : -1.0f };
            penetration = dz + radius;
        }
    }

    // ワールドへ戻す(ボックス→球の向き)
    Vector3 normal = axis[0] * normalLocal.x
        + axis[1] * normalLocal.y
        + axis[2] * normalLocal.z;

    info.normal = normal;
    info.push = normal * penetration;   // 球を押し出すベクトル
    return info;
}

CollisionInfo NarrowPhase::IsColliding(OBBCollider& obb, CylinderCollider& cylinder)
{
    using namespace DirectX::SimpleMath;
    static constexpr float EPS = 0.0001f;

    // ---------- 円柱 ----------
    Vector3 bottom = cylinder.GetBottom();
    Vector3 top = cylinder.GetTop();
    Vector3 cylCenter = (bottom + top) * 0.5f;
    Vector3 cylAxis = top - bottom;
    float height = cylAxis.Length();
    if (height > EPS) cylAxis /= height; else cylAxis = Vector3::UnitY;
    float halfH = height * 0.5f;
    float R = cylinder.GetRadius();

    // ---------- OBB ----------
    Vector3 boxCenter = obb.GetCenter() + obb.GetPosOffset();
    Vector3 half = obb.GetSize() * 0.5f;

    Vector3 euler = obb.GetRotation();   // ラジアン前提(度ならここで変換)
    Matrix rot = Matrix::CreateFromYawPitchRoll(euler.y, euler.x, euler.z);
    Vector3 boxAxis[3] =
    {
        Vector3::TransformNormal(Vector3::UnitX, rot),
        Vector3::TransformNormal(Vector3::UnitY, rot),
        Vector3::TransformNormal(Vector3::UnitZ, rot),
    };

    auto toLocal = [&](const Vector3& w)
        {
            Vector3 v = w - boxCenter;
            return Vector3(v.Dot(boxAxis[0]), v.Dot(boxAxis[1]), v.Dot(boxAxis[2]));
        };
    auto clampBox = [&](const Vector3& v)
        {
            return Vector3(MathLib::Clamp(v.x, -half.x, half.x),
                MathLib::Clamp(v.y, -half.y, half.y),
                MathLib::Clamp(v.z, -half.z, half.z));
        };

    // ---------- 縁用の軸:軸線分とOBBの最近接点(ボックスのローカル空間で反復) ----------
    Vector3 bL = toLocal(bottom);
    Vector3 tL = toLocal(top);
    Vector3 dL = tL - bL;
    float lenSq = dL.Dot(dL);

    Vector3 pL = bL;
    for (int i = 0; i < 8; ++i)
    {
        Vector3 qL = clampBox(pL);
        float t = (lenSq > EPS) ? MathLib::Clamp((qL - bL).Dot(dL) / lenSq, 0.0f, 1.0f) : 0.0f;
        pL = bL + t * dL;
    }
    Vector3 qL = clampBox(pL);
    Vector3 rl = qL - pL;
    // ローカル → ワールドの向きへ
    Vector3 rimAxis = boxAxis[0] * rl.x + boxAxis[1] * rl.y + boxAxis[2] * rl.z;

    // ---------- 候補軸 ----------
    Vector3 axes[8];
    int count = 0;
    for (int i = 0; i < 3; ++i) axes[count++] = boxAxis[i];
    axes[count++] = cylAxis;
    for (int i = 0; i < 3; ++i) axes[count++] = cylAxis.Cross(boxAxis[i]);
    if (rimAxis.LengthSquared() > EPS * EPS) axes[count++] = rimAxis;

    // ---------- SAT ----------
    CollisionInfo info{};
    float minOverlap = FLT_MAX;
    Vector3 minAxis = Vector3::UnitY;

    for (int i = 0; i < count; ++i)
    {
        float l2 = axes[i].LengthSquared();
        if (l2 < 1e-6f) continue;                  // 平行で潰れた軸は飛ばす
        Vector3 n = axes[i] / std::sqrt(l2);

        // 円柱を n に射影した半径
        float ca = cylAxis.Dot(n);
        float rCyl = halfH * std::abs(ca) + R * std::sqrt(std::max(0.0f, 1.0f - ca * ca));

        // OBB を n に射影した半径
        float rBox = half.x * std::abs(boxAxis[0].Dot(n))
            + half.y * std::abs(boxAxis[1].Dot(n))
            + half.z * std::abs(boxAxis[2].Dot(n));

        float dist = (cylCenter - boxCenter).Dot(n);
        float overlap = rCyl + rBox - std::abs(dist);

        if (overlap < 0.0f) return info;           // 分離軸あり → 非衝突

        if (overlap < minOverlap)
        {
            minOverlap = overlap;
            minAxis = (dist >= 0.0f) ? n : -n;     // ボックス→円柱の向き
        }
    }

    info.is = true;
    info.normal = minAxis;
    info.push = minAxis * minOverlap;              // 円柱を押し出す量
    return info;
}
CollisionInfo NarrowPhase::ReverseCollisionInfo(CollisionInfo info)
{
    info.normal = -info.normal;
    info.push = -info.push;
    return info;
}


