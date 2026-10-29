#pragma once
#include <random>

class Random {
public:
    // 初期化（シード値の設定）
    static void Initialize() {
        std::random_device rd;
        GetEngine().seed(rd());
    }

    // 指定範囲の整数を返す [min, max]
    static int Range(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(GetEngine());
    }

    // 指定範囲の実数を返す [min, max]
    static float Range(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(GetEngine());
    }

    // 0.0f ～ 1.0f の値を返す
    static float Value() {
        return Range(0.0f, 1.0f);
    }

private:
    // メルセンヌ・ツイスタエンジンを唯一のインスタンスとして保持
    static std::mt19937& GetEngine() {
        static std::mt19937 engine;
        return engine;
    }
};