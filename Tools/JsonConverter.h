#pragma once
#include "nlohmann/json.hpp"
#include "SimpleMath.h"

namespace DirectX {
    namespace SimpleMath {
        inline void from_json(const nlohmann::json& j, SimpleMath::Vector3& v) {
            v.x = j.value("x", 0.0f);
            v.y = j.value("y", 0.0f);
            v.z = j.value("z", 0.0f);
        }
        inline void to_json(nlohmann::json& j, const SimpleMath::Vector3& v) {
            j = nlohmann::json{ {"x", v.x}, {"y", v.y}, {"z", v.z} };
        }

            inline void from_json(const nlohmann::json& j, SimpleMath::Quaternion& q) {
                q.x = j.value("x", 0.0f);
                q.y = j.value("y", 0.0f);
                q.z = j.value("z", 0.0f);
                q.w = j.value("w", 0.0f);
            }
            inline void to_json(nlohmann::json& j, const SimpleMath::Quaternion& q) {
                j = nlohmann::json{ {"x", q.x}, {"y", q.y}, {"z", q.z}, {"w", q.w} };
            }
    }
}
