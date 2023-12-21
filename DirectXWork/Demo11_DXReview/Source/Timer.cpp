
#include "stdafx.h"
#include "Timer.h"

Timer::Timer()
	: m_SecondsPerCount(0)
	, m_BaseTime(0)
	, m_PrevTime(0)
	, m_CurrentTime(0)
	, m_DeltaTime(0.0)
	, m_PauseTime(0)
	, m_StopTime(0)
	, m_IsStop(false)
{
	int64_t countsPerSec{ 0 };
	// 获取时钟频率
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	// 计算每个时钟周期的间隔
	m_SecondsPerCount = 1.0 / static_cast<double>(countsPerSec);
}

float Timer::TotalTime() const
{
	if (m_IsStop)
	{
		return static_cast<float>((m_StopTime - m_BaseTime - m_PauseTime) * m_SecondsPerCount);
	}
	else
	{
		return static_cast<float>((m_CurrentTime - m_BaseTime - m_BaseTime) * m_SecondsPerCount);
	}
}

void Timer::Tick()
{
	if (m_IsStop)
	{
		m_DeltaTime = 0.0;
		return;
	}
	int64_t currentCounter{ 0 };
	// 获取当前时钟周期
	QueryPerformanceCounter((LARGE_INTEGER*)(&currentCounter));
	m_CurrentTime = currentCounter;
	// 计算时钟周期间隔
	m_DeltaTime = (m_CurrentTime - m_PrevTime) * m_SecondsPerCount;
	m_PrevTime = m_CurrentTime;
	// 阻止帧间隔为负值(如果处理器处于节能模式，或是两帧间切换到了另外的处理器上，即两次QueryPerformanceCounter调用在不同处理器上，则m_DeltaTime可能为负值)
	if (m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}
}

void Timer::Start()
{
	if (!m_IsStop) { return; }
	int64_t	currentCounter{ 0 };
	QueryPerformanceCounter((LARGE_INTEGER*)&currentCounter);
	m_PauseTime += (currentCounter - m_StopTime);
	m_PrevTime = currentCounter;
	m_StopTime = 0;
	m_IsStop = false;
}

void Timer::Stop()
{
	if (m_IsStop) { return; }
	int64_t	currentCounter{ 0 };
	QueryPerformanceCounter((LARGE_INTEGER*)&currentCounter);
	m_StopTime = currentCounter;
	m_IsStop = true;
}

void Timer::Reset()
{
	int64_t currentCounter{ 0 };
	QueryPerformanceCounter((LARGE_INTEGER*)&currentCounter);
	m_BaseTime = currentCounter;
	m_PrevTime = currentCounter;
	m_StopTime = 0;
	m_IsStop = false;
}