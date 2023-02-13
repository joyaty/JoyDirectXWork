/**
 * DXWavesWithBlend.cpp
 * Demo08_DXWavesWithBlend示例项目主实现文件
 **/

#include "stdafx.h"
#include "DXWavesWithBlend.h"
#include "WavesWithBlendRenderItem.h"
#include "DirectXBaseWork/GeometryGenerator.h"


DXWavesWithBlend::DXWavesWithBlend(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
	m_AssetPath += L"\\DirectXWork\\Demo08_DXWavesWithBlend\\Source\\DXWorkBlend\\";
}

DXWavesWithBlend::~DXWavesWithBlend()
{
}

void DXWavesWithBlend::OnResize(UINT width, UINT height)
{
	DirectXBaseWork::OnResize(width, height);
}

void DXWavesWithBlend::OnMouseDown(UINT8 keyCode, int x, int y)
{
}

void DXWavesWithBlend::OnMouseUp(UINT8 keyCode, int x, int y)
{
}

void DXWavesWithBlend::OnMouseMove(UINT8 keyCode, int x, int y)
{
}

bool DXWavesWithBlend::OnInit()
{
	BuildTerrain();
	return true;
}

void DXWavesWithBlend::OnUpdate(float deltaTime, float totalTime)
{
}

void DXWavesWithBlend::OnRender()
{
}

void DXWavesWithBlend::OnDestroy()
{
}

void DXWavesWithBlend::BuildTerrain()
{
	GeometryGenerator::MeshData terrainMeshData = GeometryGenerator::CreateGrid(160.f, 160.f, 50, 50);
	UINT totalVerticeSize = (UINT)terrainMeshData.Vertices.size();
	// 拷贝顶点数据
	std::vector<Vertex> terrainVertices(totalVerticeSize);
	for (UINT i = 0; i < totalVerticeSize; ++i)
	{
		DirectX::XMFLOAT3 position = terrainMeshData.Vertices[i].Position;
		terrainVertices[i].position = position;
		terrainVertices[i].position.y = GetHillsHeight(position.x, position.z);
		terrainVertices[i].normal = GetHillsNormal(position.x, position.z);
		// 直接获取纹理坐标
		terrainVertices[i].texCoord = terrainMeshData.Vertices[i].TexCoord;
	}
	UINT totalIndicesSize = (UINT)terrainMeshData.GetIndices16().size();
	// 拷贝索引数据
	std::vector<std::uint16_t> terrainIndices{};
	terrainIndices.assign(terrainMeshData.GetIndices16().cbegin(), terrainMeshData.GetIndices16().cend());
	// 顶点缓冲区和索引缓冲区的大小
	UINT vertexBufferSize = sizeof(Vertex) * totalVerticeSize;
	UINT indexBufferSize = sizeof(std::uint16_t) * totalIndicesSize;
	// 构建几何体
	std::unique_ptr<MeshGeometry> terrainMesh = std::make_unique<MeshGeometry>();
	terrainMesh->m_Name = "HillTerrain";
	terrainMesh->m_VertexBufferCPU = nullptr;   // 不做碰撞检测等，暂时不需要用到CPU上的顶点缓冲区
	terrainMesh->m_VertexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), terrainVertices.data(), vertexBufferSize, terrainMesh->m_VertexBufferUploader);
	terrainMesh->m_VertexSize = vertexBufferSize;
	terrainMesh->m_VertexStride = sizeof(Vertex);
	terrainMesh->m_IndexBufferCPU = nullptr;	// 不做碰撞检测等，暂时不需要用到CPU上的索引缓冲区
	terrainMesh->m_IndexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), terrainIndices.data(), indexBufferSize, terrainMesh->m_IndexBufferUploader);
	terrainMesh->m_IndexSize = indexBufferSize;
	terrainMesh->m_IndexFormat = DXGI_FORMAT_R16_UINT;
	SubMeshGeometry subMesh{};
	subMesh.m_IndexCount = totalIndicesSize;
	subMesh.m_StartIndexLocation = 0U;
	subMesh.m_BaseVertexLocation = 0U;
	terrainMesh->m_SubMeshGeometrys["Terrain"] = subMesh;
	// 添加到场景对象集合中
	m_SceneObjects[terrainMesh->m_Name] = std::move(terrainMesh);
}

float DXWavesWithBlend::GetHillsHeight(float x, float z) const
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

DirectX::XMFLOAT3 DXWavesWithBlend::GetHillsNormal(float x, float z)const
{
	DirectX::XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

	DirectX::XMVECTOR unitNormal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}

void DXWavesWithBlend::BuildWaves()
{
}

void DXWavesWithBlend::BuildBox()
{
}

void DXWavesWithBlend::LoadTextures()
{
}

void DXWavesWithBlend::BuildMaterials()
{
}