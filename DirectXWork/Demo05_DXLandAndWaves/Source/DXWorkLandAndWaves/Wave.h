/**
 * Wave.h
 * 模拟波浪的Mesh
 **/

#pragma once

/// <summary>
/// 模拟波浪
/// </summary>
class Wave
{
public:
	/// <summary>
	/// 波浪构造函数
	/// </summary>
	/// <param name="dx">网格间隔</param>
	/// <param name="row">行数</param>
	/// <param name="col">列数</param>
	/// <param name="dt">时间步长</param>
	/// <param name="speed"></param>
	/// <param name="damping"></param>
	Wave(float dx, int row, int col, float dt, float speed, float damping);
	Wave(const Wave& rhs) = delete;
	Wave& operator=(const Wave& rhs) = delete;
	~Wave();

	/// <summary>
	/// 更新波浪的Mesh
	/// </summary>
	/// <param name="deltaTime"></param>
	void UpdateWave(float deltaTime);

	/// <summary>
	/// 指定(i,j)位置，产生一个高度为magnitude的涟漪
	/// </summary>
	/// <param name="i"></param>
	/// <param name="j"></param>
	/// <param name="magnitude"></param>
	void Disturb(int i, int j, float magnitude);

	int RowCount() const { return m_NumRows; }
	int ColumnCount() const { return m_NumCols; }
	int VertexCount() const { return m_VertexCount; }
	int TriangleCount() const { return m_TriangeCount; }
	float Width()const { return m_SpatialStep * m_NumCols; }
	float Depth()const { return m_SpatialStep * m_NumRows; }
	const std::vector<std::uint16_t>& GetIndices() const { return m_Indices; }

	const DirectX::XMFLOAT3& GetPosition(int i) const { return m_CurrSolution[i]; }

private:
	/// <summary>
	/// 前波浪顶点
	/// </summary>
	std::vector<DirectX::XMFLOAT3> m_PrevSolution{};
	/// <summary>
	/// 波浪顶点数据
	/// </summary>
	std::vector<DirectX::XMFLOAT3> m_CurrSolution{};
	/// <summary>
	/// 波浪的法线数据
	/// </summary>
	std::vector<DirectX::XMFLOAT3> m_Normals{};
	/// <summary>
	/// 波浪的切线数据
	/// </summary>
	std::vector<DirectX::XMFLOAT3> m_TangentX{};

	/// <summary>
	/// 索引数据
	/// </summary>
	std::vector<std::uint16_t> m_Indices{};

	/// <summary>
	/// 顶点数量
	/// </summary>
	int m_VertexCount{ 0 };
	/// <summary>
	/// 三角面数量
	/// </summary>
	int m_TriangeCount{ 0 };
	/// <summary>
	/// 地表网格行数
	/// </summary>
	int m_NumRows{ 0 };
	/// <summary>
	/// 地表网格列数
	/// </summary>
	int m_NumCols{ 0 };

	/// <summary>
	/// 更新时间步长
	/// </summary>
	float m_TimeStep{ 0.03f };
	/// <summary>
	/// 更新空间步长
	/// </summary>
	float m_SpatialStep{ 1.f };

	float m_K1{ 0.f };
	float m_K2{ 0.f };
	float m_K3{ 0.f };
};