#pragma once
#include "Collider.h"

// RayCaster.h
class RayCaster
{
public:
    // 基本形:形状ごとのレイキャスト
    static bool RayCast(LineCollider& ray, const PlaneCollider& plane);
    static bool RayCast(LineCollider& ray, const BoxCollider& box);
    static bool RayCast(LineCollider& ray, const SphereCollider& sphere);
    static bool RayCast(LineCollider& ray, const TriangleCollider& triangle);
    static bool RayCast(LineCollider& ray, const OBBCollider& obb); // 追加予定

    // 汎用ディスパッチ(Colliderの実型を見て振り分け)
    static bool RayCast(LineCollider& ray, Collider* target);

    // 発展形:全コライダーから一番近いヒットを取る、という使い方もここに追加しやすい
    struct HitResult
    {
        bool hit = false;
        Collider* collider = nullptr;
        SimpleMath::Vector3 point;
        float distance = 0.0f;
    };

    static HitResult RayCastAll(LineCollider& ray, const std::vector<Collider*>& colliders);
};

