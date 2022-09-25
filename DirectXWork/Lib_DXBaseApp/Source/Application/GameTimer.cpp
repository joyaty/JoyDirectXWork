/*
 * GameTimer.cpp
 * 时间辅助工具类实现
 */

#include "stdafx.h"
#include "GameTimer.h"

GameTimer::GameTimer()
	: m_SecondsPerCount(0.0)
	, m_DeltaTime(-1.0)
	, m_Stoped(false)
	, m_BaseTime(0)
	, m_PauseTime(0)
	, m_StopTime(0)
	, m_PrevTime(0)
	, m_CurrTime(0)
{
	
	__int64 countsPerSec = 0;
	// 获取性能计时器的频率(单位: 个数/秒)
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	// 求倒数，得到每个计数代表的秒数
	m_SecondsPerCount = 1.0 / (double)countsPerSec;
}

float GameTimer::TotalTime() const
{
	if (m_Stoped)
	{
		// 暂停状态，本次暂停的时刻 - 启动时刻 - 之前累计的暂停时刻数 = 运行总时刻数
		return (float)((m_StopTime - m_BaseTime - m_PauseTime) * m_SecondsPerCount);
	}
	else
	{
		// 运行状态，当前时刻 - 启动时刻 - 累计的暂停时刻数 = 运行总时刻数
		return (float)((m_CurrTime - m_BaseTime - m_PauseTime) * m_SecondsPerCount);
	}
}

void GameTimer::Reset()
{
	__int64 currTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_BaseTime = currTime;
	m_PrevTime = currTime;
	m_StopTime = 0;
	m_Stoped = false;
}

void GameTimer::Start()
{
	if (!m_Stoped) { return; }
	__int64 currTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	// 累加停止的时刻计数
	m_PauseTime += (currTime - m_StopTime);
	// 当前时间作为上一帧时间
	m_PrevTime = currTime;
	// 清理停止时刻和停止标记
	m_StopTime = 0;
	m_Stoped = false;
}

void GameTimer::Stop()
{
	if (m_Stoped) { return; }
	// 查询当前时刻，并保存为停止时刻，标记计时器已停止
	__int64 currTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_StopTime = currTime;
	m_Stoped = true;
}

void GameTimer::Tick()
{
	if (m_Stoped)
	{
		m_DeltaTime = 0.0;
		return;
	}
	// 查询当前的性能计数器计数
	__int64 currTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_CurrTime = currTime;
	// 计算本帧与上一帧的时间间隔
	m_DeltaTime = (m_CurrTime - m_PrevTime) * m_SecondsPerCount;
	// 更新
	m_PrevTime = m_CurrTime;
	// 阻止帧间隔为负值(如果处理器处于节能模式，或是两帧间切换到了另外的处理器上，即两次QueryPerformanceCounter调用在不同处理器上，则m_DeltaTime可能为负值)
	if (m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}
}
