/*
 * GameTimer.h 时间辅助工具类定义
 */

#pragma once

class GameTimer
{
public:
	/// <summary>
	/// 构造函数，初始时记录计数表示的间隔
	/// </summary>
	GameTimer();

	/// <summary>
	/// 获取运行的总时长，不包含停止时长
	/// </summary>
	/// <returns></returns>
	float TotalTime() const;	// 单位: 秒

	/// <summary>
	/// 获取帧间隔，单位秒
	/// </summary>
	/// <returns></returns>
	float DeltaTime() const { return m_DeltaTime; };

	/// <summary>
	/// 重置计数器，启动时调用一次
	/// </summary>
	void Reset();
	/// <summary>
	/// 从停止时刻恢复
	/// </summary>
	void Start();
	/// <summary>
	/// 进入停止时刻调用
	/// </summary>
	void Stop();
	/// <summary>
	/// 运行时每帧更新
	/// </summary>
	void Tick();

private:
	/// <summary>
	/// 每个计数的时间间隔
	/// </summary>
	double m_SecondsPerCount;

	/// <summary>
	/// 帧间隔时间
	/// </summary>
	double m_DeltaTime;

	/// <summary>
	/// 游戏计时器停止标记
	/// </summary>
	bool m_Stoped;

	/// <summary>
	/// 停止的时长累计
	/// </summary>
	__int64 m_PauseTime;
	/// <summary>
	/// 启动时刻，在启动时记录一次
	/// </summary>
	__int64 m_BaseTime;
	/// <summary>
	/// 停止时刻，进入停止时刻时记录一次，重复进入停止时刻不记录
	/// </summary>
	__int64 m_StopTime;
	/// <summary>
	/// 前一帧时刻
	/// </summary>
	__int64 m_PrevTime;
	/// <summary>
	/// 当前帧时刻
	/// </summary>
	__int64 m_CurrTime;
};