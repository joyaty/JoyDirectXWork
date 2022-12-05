/*
 * D3D12HelloShapes.cpp
 * Demo04 - DirectX12渲染多个几何图形
 */

#include "stdafx.h"
#include "D3D12HelloShapes.h"
#include "FrameResource.h"
#include <DirectXColors.h>
#include "DirectXBaseWork/GeometryGenerator.h"
#include "DirectXBaseWork/D3D12Util.h"
#include "RenderItem.h"


 /// <summary>
 /// 帧资源的个数
 /// </summary>
const int kNumFrameResource = 3;

D3D12HelloShapes::D3D12HelloShapes(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
	m_AssetPath += L"\\DirectXWork\\Demo04_DXHelloShapes\\Source\\DXHelloShapes\\";
}

D3D12HelloShapes::~D3D12HelloShapes()
{
}

bool D3D12HelloShapes::OnInit()
{
	BuildShapeGeometry();
	BuildRenderItem();
	BuildFrameResource();
	return true;
}

void D3D12HelloShapes::OnUpdate(float deltaTime, float totalTime)
{
	// 循环往复获取帧资源循环数组中的帧资源
	m_CurrentFrameResourceIndex = (m_CurrentFrameResourceIndex + 1) % kNumFrameResource;
	m_CurrentFrameResource = m_FrameResources[m_CurrentFrameResourceIndex].get();
	// 当前完成的围栏值小于帧资源标记的围栏值，说明需要CPU需要等待GPU完成当前帧资源的绘制
	if (m_CurrentFrameResource->fenceValue != 0 && m_CurrentFrameResource->fenceValue > m_Fence->GetCompletedValue())
	{
		// FlushCommandQueue();
	}
}

void D3D12HelloShapes::OnRender()
{
}

void D3D12HelloShapes::OnDestroy()
{
}

void D3D12HelloShapes::UpdateObjectCBs(float deltaTime, float totalTime)
{
}

void D3D12HelloShapes::UpdatePassCBs(float deltaTime, float totalTime)
{
}

void D3D12HelloShapes::BuildShapeGeometry()
{
	// 构建Cube几何体
	GeometryGenerator::MeshData cube = GeometryGenerator::CreateCube(2.f, 2.f, 2.f, 0);
	// 顶点总数
	size_t totalVertexCount = cube.Vertices.size();
	// 构建所有Shape的顶点数据
	std::vector<ShapeVertex> vertices(totalVertexCount);
	for (size_t i = 0; i < cube.Vertices.size(); ++i)
	{
		ShapeVertex vert{};
		vert.position = cube.Vertices[i].Position;
		vert.color = DirectX::XMFLOAT4(DirectX::Colors::Gray);
		vertices[i] = vert;
	}
	// 构建所有Shape的索引数据
	std::vector<std::uint16_t> indices{};
	indices.assign(cube.GetIndices16().begin(), cube.GetIndices16().end());
	// 顶点缓冲区的大小
	UINT vertexBufferSize = sizeof(ShapeVertex) * (UINT)vertices.size();
	UINT indexBufferSize = sizeof(std::uint16_t) * (UINT)indices.size();
	// 构建需要绘制的整体网格数据
	std::unique_ptr<MeshGeometry> meshGeo = std::make_unique<MeshGeometry>();
	meshGeo->m_Name = "ShapeGeo";
	// 在GPU上构建顶点缓冲区资源，缓存单个顶点数据的步长和顶点缓冲区的字节大小
	meshGeo->m_VertexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), vertices.data(), vertexBufferSize, meshGeo->m_VertexBufferUploader);
	meshGeo->m_VertexStride = sizeof(ShapeVertex);
	meshGeo->m_VertexSize = vertexBufferSize;
	// 在GPU上构建索引缓冲区资源，缓存索引的数据类型和索引缓冲区的字节大小
	meshGeo->m_IndexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), indices.data(), indexBufferSize, meshGeo->m_IndexBufferUploader);
	meshGeo->m_IndexFormat = DXGI_FORMAT_R16_UINT;
	meshGeo->m_IndexSize = indexBufferSize;
	// 整体网格数据可用多个子Mesh组成，各个子Mesh的顶点和索引数据，组合成一个大的顶点和索引数据，子Mesh中记录自己Mesh的顶点和索引数据在整体上的偏移
	// 子Mesh - Cube
	SubMeshGeometry subMeshCube{};
	subMeshCube.m_IndexCount = (UINT)indices.size();
	subMeshCube.m_StartIndexLocation = 0U;
	subMeshCube.m_BaseVertexLocation = 0U;
	meshGeo->m_SubMeshGeometrys["Cube"] = subMeshCube;
	// 缓存需要绘制的网格数据
	m_GeoMeshes[meshGeo->m_Name] = std::move(meshGeo);
}

void D3D12HelloShapes::BuildRenderItem()
{
	std::unique_ptr<RenderItem> cubeRenderItem = std::make_unique<RenderItem>();
	cubeRenderItem->meshGeo = m_GeoMeshes["ShapeGeo"].get();
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(-2.0f, 2.0f, 0.0f);
	DirectX::XMStoreFloat4x4(&cubeRenderItem->worldMatrix, worldMatrix);
	cubeRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	cubeRenderItem->objectCBIndex = 0;
	cubeRenderItem->indexCount = cubeRenderItem->meshGeo->m_SubMeshGeometrys["Cube"].m_IndexCount;
	cubeRenderItem->startIndexLocation = cubeRenderItem->meshGeo->m_SubMeshGeometrys["Cube"].m_StartIndexLocation;
	cubeRenderItem->startVertexLocation = cubeRenderItem->meshGeo->m_SubMeshGeometrys["Cube"].m_BaseVertexLocation;
	// 保存到所有的渲染项向量中
	m_AllRenderItems.push_back(std::move(cubeRenderItem));
}

void D3D12HelloShapes::BuildFrameResource()
{
	for (int i = 0; i < kNumFrameResource; ++i)
	{
		m_FrameResources.push_back(std::make_unique<FrameResource>(m_Device.Get(), 1U, (UINT)m_AllRenderItems.size()));
	}
}

void D3D12HelloShapes::BuildCBVDescriptorHeaps()
{
	// 每个帧资源有自己独立的渲染过程常量缓冲区和物体常量缓冲区(由渲染项物体自己的常量缓冲区组合而成)
	// 故总共需要的常量缓冲区描述符个数为(1 + RenderItemCount) * FrameResourceCount
	UINT numDescriptor = (1U + (UINT)m_AllRenderItems.size()) * kNumFrameResource;
	// 构建描述符堆描述
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc{};
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.NodeMask = 0U;
	cbvHeapDesc.NumDescriptors = numDescriptor;
	// 构建常量缓冲区描述符堆
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(m_CBVDescriptorHeap.GetAddressOf())));
}

void D3D12HelloShapes::BuildConstantBufferView()
{
	// 渲染项的个数
	UINT objectCount = (UINT)m_AllRenderItems.size();
	// 遍历所有的帧资源，为每个帧资源和渲染先创建独立的物体常量缓冲视图
	for (size_t i = 0; i < m_FrameResources.size(); ++i)
	{
		// 获取物体常量缓冲区资源
		ID3D12Resource* pObjCBResource = m_FrameResources[i]->pObjectConstants->GetResource();
		// 获取常量缓冲区的大小(单个元素)
		UINT cbByteSize = m_FrameResources[i]->pObjectConstants->GetElementSize();
		// 遍历所有的渲染项，创建渲染项关联物体的常量缓冲区视图
		for (size_t j = 0; j < objectCount; ++j)
		{
			// 常量缓冲区资源的GPU内存地址
			D3D12_GPU_VIRTUAL_ADDRESS cbAddress = pObjCBResource->GetGPUVirtualAddress();
			// 根据单元素常量缓冲区字节大小，计算当前渲染项关联物体常量缓冲区的地址
			cbAddress += j * cbByteSize;
			// 创建常量缓冲区视图描述
			D3D12_CONSTANT_BUFFER_VIEW_DESC desc{};
			desc.BufferLocation = cbAddress;
			desc.SizeInBytes = cbByteSize;
			// 描述符堆有多个描述符，根据帧资源和渲染项的索引按需分配handle
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_CBVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			handle.Offset((UINT)(i * objectCount + j), m_CBVUAVDescriptorSize);
			// 创建常量缓冲区视图
			m_Device->CreateConstantBufferView(&desc, handle);
		}
	}
	// 创建渲染过程常量缓冲区视图
	for (size_t i = 0; i < m_FrameResources.size(); ++i)
	{
		// 获取渲染过程常量缓冲区资源
		ID3D12Resource* pPassCBResource = m_FrameResources[i]->pPassConstants->GetResource();
		// 获取渲染过程常量缓冲区的字节大小(单个元素)
		UINT cbByteSize = m_FrameResources[i]->pPassConstants->GetElementSize();
		// 获取渲染过程常量缓冲区资源的GPU内存地址，每个帧资源只需要一个渲染过程常量缓冲区，不需要偏移
		D3D12_GPU_VIRTUAL_ADDRESS address = pPassCBResource->GetGPUVirtualAddress();
		// 创建常量缓冲区视图描述
		D3D12_CONSTANT_BUFFER_VIEW_DESC desc{};
		desc.BufferLocation = address;
		desc.SizeInBytes = cbByteSize;
		// 从描述符堆中分配Handle
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_CBVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset((UINT)(kFrameBufferCount * objectCount + i), m_CBVUAVDescriptorSize);
		// 创建常量缓冲视图
		m_Device->CreateConstantBufferView(&desc, handle);
	}
}