#include "pch.h"
#include "Croutine.h"

std::unique_ptr<Croutine> Croutine::s_croutine = nullptr;

bool Croutine::Update(float elapsedTime)
{
    m_elapsedTime += elapsedTime;

    if (!m_started || !m_croutine) return true;

    // 待ち時間の判定 logic
    if (m_iEmurator == IEmurator::One)
    {
        if (m_startTime < 0.0f)
        {
            m_startTime = m_elapsedTime; // 待機開始時間を記録
            return true;
        }

        // 1秒経っていなければ次のフレームへ
        if (m_elapsedTime - m_startTime < 1.0f)
        {
            return true;
        }

        // 1秒経ったのでリセットして実行へ進む
        m_startTime = -1.0f;
    }
    // 終わっていたら関数を削除
    else if (m_iEmurator == IEmurator::End && m_croutine != nullptr)
    {
        DeleteCroutine();
    }

    // コルーチン関数の実行（引数はすでにバインドされているので呼ぶだけでOK）
    if(m_croutine)
        m_iEmurator = m_croutine();
}
