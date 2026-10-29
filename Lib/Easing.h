#pragma once

// イージング関数用名前空間
namespace Easing
{
	/**
	*@brief リニア関数
	*
	* @param now	     経過時間
	* @param destination 開始地点
	* @param distance    目的地までの距離
	* @param max	     最大時間
	*
	*/
	inline float Linear(float now, float destination, float  distance, float max)
	{
		return distance * now / max + destination;
	}


	/**
	*@brief イーズイン関数
	* 
	* @param now	     経過時間
	* @param destination 開始地点
	* @param distance    目的地までの距離
	* @param max	     最大時間
	* 
	*/
	template<typename T>
	inline T EaseIn(float now, T destination, T distance, float max)
	{
		now /= max;
		return distance * now * now + destination;
	}


	/**
	* @brief イーズアウト関数
	* 
	* @param now	     経過時間
	* @param destination 開始地点
	* @param distance    目的地までの距離
	* @param max	     最大時間
	* 
	*/
	template<typename T>
	inline T EaseOut(float now, T destination, T  distance, float max)
	{
		return distance * (-(pow(2.0f, (-20.0f * now / max))) + 1.0f) + destination;
	}
}

