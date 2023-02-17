/**
 * Wave.h
 * 波浪Mesh模拟
 **/

#pragma once

/// <summary>
/// 模拟波浪的顶点数据变化
/// </summary>
class Wave
{
public:
	Wave(int row, int col, float inveral, float timeStep, float speed, float damping);
	Wave(const Wave& rhs) = delete;
	Wave operator= (const Wave& rhs) = delete;
	~Wave();

public:
	/// <summary>
	/// 波浪动画模拟
	/// </summary>
	/// <param name="deltaTime"></param>
	void UpdateWave(float deltaTime);

	void Disturb(int i, int j, float magnitude);

public:
	const std::vector<std::uint16_t>& GetIndices() const { return m_Indices; }
	int GetRowNum() const { return m_NumRows; }
	int GetColNum() const { return m_NumCols; }
	int GetVertexCount() const { return m_VertexCount; }
	int GetTriangleCount() const { return m_TriangleCount; }
	float GetWidth() const { return m_SpatialStep * m_NumCols; }
	float GetHeight() const { return m_SpatialStep * m_NumRows; }
	const DirectX::XMFLOAT3& GetPosition(int index) const { return m_CurPositions[index]; }

private:
	/// <summary>
	/// 网格行数
	/// </summary>
	int m_NumRows{ 0 };
	/// <summary>
	/// 网格列数
	/// </summary>
	int m_NumCols{ 0 };
	/// <summary>
	/// 顶点总是
	/// </summary>
	int m_VertexCount{ 0 };
	/// <summary>
	/// 三角面总数
	/// </summary>
	int m_TriangleCount{ 0 };

	/// <summary>
	/// 上次顶点位置
	/// </summary>
	std::vector<DirectX::XMFLOAT3> m_PrePositions{};
	/// <summary>
	/// 现在顶点位置
	/// </summary>
	std::vector<DirectX::XMFLOAT3> m_CurPositions{};
	/// <summary>
	/// 顶点法线
	/// </summary>
	std::vector<DirectX::XMFLOAT3> m_Normals{};
	/// <summary>
	/// 顶点切线
	/// </summary>
	std::vector<DirectX::XMFLOAT3> m_TangentXs{};
	/// <summary>
	/// 纹理坐标
	/// </summary>
	std::vector<DirectX::XMFLOAT2> m_TexCoords{};
	/// <summary>
	/// 索引
	/// </summary>
	std::vector<std::uint16_t> m_Indices{};

	/// <summary>
	/// 时间轴步长
	/// </summary>
	float m_TimeStep{ 0.3f };
	/// <summary>
	/// 空间步长
	/// </summary>
	float m_SpatialStep{ 1.f };

	float m_K1{ 0.f }, m_K2{ 0.f }, m_K3{ 0.f };
};