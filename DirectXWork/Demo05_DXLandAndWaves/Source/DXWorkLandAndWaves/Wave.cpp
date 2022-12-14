/**
 * Wave.cpp
 * 模拟波浪的Mesh
 **/

#include "stdafx.h"
#include "Wave.h"
#include "DirectXBaseWork/GeometryGenerator.h"
#include <algorithm>
#include <ppl.h>
#include <cassert>

Wave::Wave(float dx, int row, int col, float dt, float speed, float damping)
{
	m_NumRows = row;
	m_NumCols = col;
	// 行数和列数构成交叉点个数即为顶点数
	m_VertexCount = row * col;
	// 行数和列数，组成四边形个数，每个四边网格由两个三角网格构成
	m_TriangeCount = (row - 1) * (col - 1) * 2; 
	// 构建地表网格
	float width = dx * (col - 1);
	float depth = dx * (row - 1);
	GeometryGenerator::MeshData gridMesh = GeometryGenerator::CreateGrid(width, depth, row, col);
	// 拷贝顶点数据，在网格顶点上做波浪模拟
	m_PrevSolution.resize(m_VertexCount);
	m_CurrSolution.resize(m_VertexCount);
	m_Normals.resize(m_VertexCount);
	m_TangentX.resize(m_VertexCount);
	for (size_t i = 0; i < gridMesh.Vertices.size(); ++i)
	{
		m_PrevSolution[i] = gridMesh.Vertices[i].Position;
		m_CurrSolution[i] = gridMesh.Vertices[i].Position;
		m_Normals[i] = gridMesh.Vertices[i].Normal;
		m_TangentX[i] = gridMesh.Vertices[i].TangentU;
	}
	// 拷贝索引数据
	// m_Indices.resize(3 * m_TriangeCount);
	m_Indices.insert(m_Indices.end(), gridMesh.GetIndices16().begin(), gridMesh.GetIndices16().end());

	m_TimeStep = dt;
	m_SpatialStep = width / (col - 1);

	float d = damping * dt + 2.f;
	float e = (speed * speed) * (dt * dt) / (dx * dx);
	m_K1 = (damping * dt - 2.f) / d;
	m_K2 = (4.f - 8.f * e) / d;
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
					m_PrevSolution[i * m_NumCols + j].y =
						m_K1 * m_PrevSolution[i * m_NumCols + j].y +
						m_K2 * m_CurrSolution[i * m_NumCols + j].y +
						m_K3 * (m_CurrSolution[(i + 1) * m_NumCols + j].y +
							m_CurrSolution[(i - 1) * m_NumCols + j].y +
							m_CurrSolution[i * m_NumCols + j + 1].y +
							m_CurrSolution[i * m_NumCols + j - 1].y);
				}
			});

		// We just overwrote the previous buffer with the new data, so
		// this data needs to become the current solution and the old
		// current solution becomes the new previous solution.
		std::swap(m_PrevSolution, m_CurrSolution);
		// reset time
		fContinueTime = 0.0f; 
		// Compute normals using finite difference scheme.
		concurrency::parallel_for(1, m_NumRows - 1, [this](int i)
			//for(int i = 1; i < mNumRows - 1; ++i)
			{
				for (int j = 1; j < m_NumCols - 1; ++j)
				{
					float l = m_CurrSolution[i * m_NumCols + j - 1].y;
					float r = m_CurrSolution[i * m_NumCols + j + 1].y;
					float t = m_CurrSolution[(i - 1) * m_NumCols + j].y;
					float b = m_CurrSolution[(i + 1) * m_NumCols + j].y;
					m_Normals[i * m_NumCols + j].x = -r + l;
					m_Normals[i * m_NumCols + j].y = 2.0f * m_SpatialStep;
					m_Normals[i * m_NumCols + j].z = b - t;

					DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&m_Normals[i * m_NumCols + j]));
					DirectX::XMStoreFloat3(&m_Normals[i * m_NumCols + j], n);

					m_TangentX[i * m_NumCols + j] = DirectX::XMFLOAT3(2.0f * m_SpatialStep, r - l, 0.0f);
					DirectX::XMVECTOR T = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&m_TangentX[i * m_NumCols + j]));
					DirectX::XMStoreFloat3(&m_TangentX[i * m_NumCols + j], T);
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
	m_CurrSolution[i * m_NumCols + j].y += magnitude;
	m_CurrSolution[i * m_NumCols + j + 1].y += halfMag;
	m_CurrSolution[i * m_NumCols + j - 1].y += halfMag;
	m_CurrSolution[(i + 1) * m_NumCols + j].y += halfMag;
	m_CurrSolution[(i - 1) * m_NumCols + j].y += halfMag;
}