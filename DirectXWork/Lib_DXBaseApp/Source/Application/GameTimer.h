/*
 * GameTimer.h ʱ�丨�������ඨ��
 */

#pragma once

class GameTimer
{
public:
	/// <summary>
	/// ���캯������ʼʱ��¼������ʾ�ļ��
	/// </summary>
	GameTimer();

	/// <summary>
	/// ��ȡ���е���ʱ����������ֹͣʱ��
	/// </summary>
	/// <returns></returns>
	float TotalTime() const;	// ��λ: ��

	/// <summary>
	/// ��ȡ֡�������λ��
	/// </summary>
	/// <returns></returns>
	float DeltaTime() const { return m_DeltaTime; };

	/// <summary>
	/// ���ü�����������ʱ����һ��
	/// </summary>
	void Reset();
	/// <summary>
	/// ��ֹͣʱ�ָ̻�
	/// </summary>
	void Start();
	/// <summary>
	/// ����ֹͣʱ�̵���
	/// </summary>
	void Stop();
	/// <summary>
	/// ����ʱÿ֡����
	/// </summary>
	void Tick();

private:
	/// <summary>
	/// ÿ��������ʱ����
	/// </summary>
	double m_SecondsPerCount;

	/// <summary>
	/// ֡���ʱ��
	/// </summary>
	double m_DeltaTime;

	/// <summary>
	/// ��Ϸ��ʱ��ֹͣ���
	/// </summary>
	bool m_Stoped;

	/// <summary>
	/// ֹͣ��ʱ���ۼ�
	/// </summary>
	__int64 m_PauseTime;
	/// <summary>
	/// ����ʱ�̣�������ʱ��¼һ��
	/// </summary>
	__int64 m_BaseTime;
	/// <summary>
	/// ֹͣʱ�̣�����ֹͣʱ��ʱ��¼һ�Σ��ظ�����ֹͣʱ�̲���¼
	/// </summary>
	__int64 m_StopTime;
	/// <summary>
	/// ǰһ֡ʱ��
	/// </summary>
	__int64 m_PrevTime;
	/// <summary>
	/// ��ǰ֡ʱ��
	/// </summary>
	__int64 m_CurrTime;
};