/*
 * GameTimer.cpp ʱ�丨��������ʵ��
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
	// ��ȡ���ܼ�ʱ����Ƶ��(��λ: ����/��)
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	// �������õ�ÿ���������������
	m_SecondsPerCount = 1.0 / (double)countsPerSec;
}

float GameTimer::TotalTime() const
{
	if (m_Stoped)
	{
		// ��ͣ״̬��������ͣ��ʱ�� - ����ʱ�� - ֮ǰ�ۼƵ���ͣʱ���� = ������ʱ����
		return (float)((m_StopTime - m_BaseTime - m_PauseTime) * m_SecondsPerCount);
	}
	else
	{
		// ����״̬����ǰʱ�� - ����ʱ�� - �ۼƵ���ͣʱ���� = ������ʱ����
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
	// �ۼ�ֹͣ��ʱ�̼���
	m_PauseTime += (currTime - m_StopTime);
	// ��ǰʱ����Ϊ��һ֡ʱ��
	m_PrevTime = currTime;
	// ����ֹͣʱ�̺�ֹͣ���
	m_StopTime = 0;
	m_Stoped = false;
}

void GameTimer::Stop()
{
	if (m_Stoped) { return; }
	// ��ѯ��ǰʱ�̣�������Ϊֹͣʱ�̣���Ǽ�ʱ����ֹͣ
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
	// ��ѯ��ǰ�����ܼ���������
	__int64 currTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_CurrTime = currTime;
	// ���㱾֡����һ֡��ʱ����
	m_DeltaTime = (m_CurrTime - m_PrevTime) * m_SecondsPerCount;
	// ����
	m_PrevTime = m_CurrTime;
	// ��ֹ֡���Ϊ��ֵ(������������ڽ���ģʽ��������֡���л���������Ĵ������ϣ�������QueryPerformanceCounter�����ڲ�ͬ�������ϣ���m_DeltaTime����Ϊ��ֵ)
	if (m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}
}
