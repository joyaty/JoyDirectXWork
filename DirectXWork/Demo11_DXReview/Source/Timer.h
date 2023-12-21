
#pragma once

/// <summary>
/// 时钟计时器
/// </summary>
class Timer
{
public:
	Timer();
	/// <summary>
	/// 获取总运行时间
	/// </summary>
	/// <returns></returns>
	float TotalTime() const;
	/// <summary>
	/// 获取去计时器时钟间隔
	/// </summary>
	/// <returns></returns>
	float DeltaTime() const { return static_cast<float>(m_DeltaTime); }

	/// <summary>
	/// 更新计时器周期
	/// </summary>
	void Tick();
	/// <summary>
	/// 恢复计时器
	/// </summary>
	void Start();
	/// <summary>
	/// 暂停计时器
	/// </summary>
	void Stop();
	/// <summary>
	/// 重置计时器，启动时会调用一次
	/// </summary>
	void Reset();

private:
	/// <summary>
	/// 每个时钟周期间隔，性能计数器的频率在系统启动时固定，并且在所有处理器中保持一致，因此可以缓存
	/// </summary>
	double m_SecondsPerCount;

	/// <summary>
	/// 当前计时器是否暂停
	/// </summary>
	bool m_IsStop;
	/// <summary>
	/// 记录累计的暂停总时钟周期
	/// </summary>
	int64_t m_PauseTime;
	/// <summary>
	/// 记录停止时刻的时钟周期
	/// </summary>
	int64_t m_StopTime;

	/// <summary>
	/// 记录上次的时钟周期
	/// </summary>
	int64_t m_PrevTime;
	/// <summary>
	/// 记录当前的时钟周期
	/// </summary>
	int64_t m_CurrentTime;
	/// <summary>
	/// 记录启动时的时钟周期
	/// </summary>
	int64_t m_BaseTime;

	/// <summary>
	/// 两次Tick时间间隔
	/// </summary>
	double m_DeltaTime;
};