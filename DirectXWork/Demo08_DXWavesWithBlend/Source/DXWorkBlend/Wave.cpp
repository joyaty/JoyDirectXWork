/**
 * Wave.cpp
 * 波浪Mesh模拟
 **/

#include "stdafx.h"
#include "Wave.h"
#include "DirectXBaseWork/GeometryGenerator.h"
#include <ppl.h>

Wave::Wave(int row, int col, float inveral, float timeStep, float speed, float damping)
	: m_NumRows(row)
	, m_NumCols(col)
	, m_VertexCount(row * col) // 行数列数构成交叉点即为顶点总数
	, m_TriangleCount((row - 1) * (col - 1) * 2) // 行数和列数构成四边形数，一个四边形由两个三角形组成
{
	// 构建XZ平面网格
	float width = (col - 1) * inveral;
	float height = (row - 1) * inveral;
	GeometryGenerator::MeshData gridMesh = GeometryGenerator::CreateGrid(width, height, row, col);
	m_PrePositions.resize(gridMesh.Vertices.size());
	m_CurPositions.resize(gridMesh.Vertices.size());
	m_Normals.resize(gridMesh.Vertices.size());
	m_TangentXs.resize(gridMesh.Vertices.size());
	m_TexCoords.resize(gridMesh.Vertices.size());
	// 拷贝顶点数据
	for (size_t i = 0; i < gridMesh.Vertices.size(); ++i)
	{
		m_PrePositions[i] = gridMesh.Vertices[i].Position;
		m_CurPositions[i] = gridMesh.Vertices[i].Position;
		m_Normals[i] = gridMesh.Vertices[i].Normal;
		m_TangentXs[i] = gridMesh.Vertices[i].TangentU;
		m_TexCoords[i] = gridMesh.Vertices[i].TexCoord;
	}
	// 拷贝索引数据
	m_Indices.insert(m_Indices.cend(), gridMesh.GetIndices16().begin(), gridMesh.GetIndices16().end());

	m_TimeStep = timeStep;
	m_SpatialStep = width / (col - 1);
	float d = damping * timeStep + 2.f;
	float e = (speed * speed) * (timeStep * timeStep) / (inveral * inveral);
	m_K1 = (damping * timeStep - 2.f) / d;
	m_K2 = (4.f - 8.f * 2) / d;
	m_K3 = (2.f * e) / d;
}

Wave::~Wave()
{
}

void Wave::UpdateWave(float deltaTime)
{
	// 龙书波浪模拟算法
	static float fContinueTime = 0.f;
	fContinueTime += deltaTime;
	// 累计时间抵达更新步长才需要更新波浪网格
	if (fContinueTime >= m_TimeStep)
	{
		// Only update interior points; we use zero boundary conditions.
		concurrency::parallel_for(1, m_NumRows - 1, [this](int i)
			//for(int i = 1; i < mNumRows-1; ++i)
			{
				for (int j = 1; j < m_NumCols - 1; ++j)
				{
					// After this update we will be discarding the old previous
					// buffer, so overwrite that buffer with the new update.
					// Note how we can do this inplace (read/write to same element) 
					// because we won't need prev_ij again and the assignment happens last.

					// Note j indexes x and i indexes z: h(x_j, z_i, t_k)
					// Moreover, our +z axis goes "down"; this is just to 
					// keep consistent with our row indices going down.
					m_PrePositions[i * m_NumCols + j].y =
						m_K1 * m_PrePositions[i * m_NumCols + j].y +
						m_K2 * m_CurPositions[i * m_NumCols + j].y +
						m_K3 * (m_CurPositions[(i + 1) * m_NumCols + j].y +
							m_CurPositions[(i - 1) * m_NumCols + j].y +
							m_CurPositions[i * m_NumCols + j + 1].y +
							m_CurPositions[i * m_NumCols + j - 1].y);
				}
			});

		// We just overwrote the previous buffer with the new data, so
		// this data needs to become the current solution and the old
		// current solution becomes the new previous solution.
		std::swap(m_PrePositions, m_CurPositions);
		// reset time
		fContinueTime = 0.0f;
		// Compute normals using finite difference scheme.
		concurrency::parallel_for(1, m_NumRows - 1, [this](int i)
			//for(int i = 1; i < mNumRows - 1; ++i)
			{
				for (int j = 1; j < m_NumCols - 1; ++j)
				{
					float l = m_CurPositions[i * m_NumCols + j - 1].y;
					float r = m_CurPositions[i * m_NumCols + j + 1].y;
					float t = m_CurPositions[(i - 1) * m_NumCols + j].y;
					float b = m_CurPositions[(i + 1) * m_NumCols + j].y;
					m_Normals[i * m_NumCols + j].x = -r + l;
					m_Normals[i * m_NumCols + j].y = 2.0f * m_SpatialStep;
					m_Normals[i * m_NumCols + j].z = b - t;

					DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&m_Normals[i * m_NumCols + j]));
					DirectX::XMStoreFloat3(&m_Normals[i * m_NumCols + j], n);

					m_TangentXs[i * m_NumCols + j] = DirectX::XMFLOAT3(2.0f * m_SpatialStep, r - l, 0.0f);
					DirectX::XMVECTOR T = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&m_TangentXs[i * m_NumCols + j]));
					DirectX::XMStoreFloat3(&m_TangentXs[i * m_NumCols + j], T);
				}
			});
	}
}

void Wave::Disturb(int i, int j, float magnitude)
{
	// Don't disturb boundaries.
	assert(i > 1 && i < m_NumRows - 2);
	assert(j > 1 && j < m_NumCols - 2);

	float halfMag = 0.5f * magnitude;

	// Disturb the ijth vertex height and its neighbors.
	m_CurPositions[i * m_NumCols + j].y += magnitude;
	m_CurPositions[i * m_NumCols + j + 1].y += halfMag;
	m_CurPositions[i * m_NumCols + j - 1].y += halfMag;
	m_CurPositions[(i + 1) * m_NumCols + j].y += halfMag;
	m_CurPositions[(i - 1) * m_NumCols + j].y += halfMag;
}