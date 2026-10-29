/// --------------------------
/// Croutineクラス
/// 設定されたら規定のタイミングまで実行し続ける関数
/// --------------------------

enum class IEmurator
{
	Null,	// 次のフレームまで待つ
	One,	// 一秒待つ
	End,	// 終了合図
};

#pragma once
class Croutine final
{
public:

	// コルーチンの設定
	// どんな引数の関数でも、ここでラムダ式で包んで保存する
	template<typename TFunction, typename... TArgs>
	void StartCroutine(TFunction&& func, TArgs&&... args)
	{
		// 引数をキャプチャして「引数なしの関数」に変換して保持する
		m_croutine = [func, args...]() mutable {
			return func(args...);
			};
		m_started = true;
		m_startTime = -1.0f;
		m_iEmurator = IEmurator::Null;
	}

	// コルーチンの削除
	void DeleteCroutine() { 
		m_croutine = nullptr; 
		m_started = false;
		m_iEmurator = IEmurator::End;
	}

	// コルーチンが実行中かどうか
	bool IsPlaying() { return m_iEmurator == IEmurator::End; }

	bool Update(float elapsedTime);

	static Croutine* GetInstace()
	{
		if (s_croutine == nullptr)
		{
			s_croutine.reset(new Croutine());
		}
		return s_croutine.get();
	}

	static Croutine& GetRefarence()
	{
		if (s_croutine == nullptr)
		{
			s_croutine.reset(new Croutine());
		}
		return *s_croutine.get();
	}

	static void Delete()
	{
		s_croutine.reset();
	}

	~Croutine() {};


private:

	// 実行対象の関数
	std::function<IEmurator()> m_croutine;

	// 実行していいか
	bool m_started;
	// 時間計測
	float m_elapsedTime;
	// 前フレーム実行した際の停止命令
	IEmurator m_iEmurator;
	// 計測開始時間
	float m_startTime = -1;

	// 自身のユニークポインタ
	static std::unique_ptr<Croutine> s_croutine;

	Croutine() 
		: m_croutine{}
		, m_started{}
		, m_iEmurator{}
		, m_startTime{}
		, m_elapsedTime{}
	{};

};

