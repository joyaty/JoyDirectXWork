/*
 * Direct3D示例中的一些工具方法和通用结构体定义
 * D3D12Util.cpp
 */

#include "stdafx.h"
#include "D3D12Util.h"
#include "DXWorkHelper.h"

Microsoft::WRL::ComPtr<ID3D12Resource> D3D12Util::CreateBufferInDefaultHeap(ID3D12Device* pDevice
	, ID3D12GraphicsCommandList* pCommandList
	, const void* pInitData, UINT bufferSize
	, Microsoft::WRL::ComPtr<ID3D12Resource>& pUploadBuffer
)
{
	// 在默认堆上创建缓冲区资源
	Microsoft::WRL::ComPtr<ID3D12Resource> pDefaultBuffer{};
	ThrowIfFailed(pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(pDefaultBuffer.GetAddressOf())
	));

	// 在上传堆上创建中介缓冲区资源，CPU数据写入到上传堆的中介缓冲区后，再复制到默认堆上真正的默认堆缓冲区中
	ThrowIfFailed(pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,	// 读取状态位的Or组合，上传堆所需初始状态
		nullptr,
		IID_PPV_ARGS(pUploadBuffer.GetAddressOf())
	));
	// 希望复制到默认堆缓冲区的数据
	D3D12_SUBRESOURCE_DATA subResourceData{};
	subResourceData.pData = pInitData;		// 指向缓冲区要复制的内存数据地址
	subResourceData.RowPitch = bufferSize;  // 对于缓冲区资源而言，该字段表示要复制的数据字节数
	subResourceData.SlicePitch = subResourceData.RowPitch; // 对于缓冲区资源而言，该字段表示要复制的数据字节数
	// 默认堆缓冲区资源转换到拷贝目标状态
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		pDefaultBuffer.Get()
		, D3D12_RESOURCE_STATE_COMMON
		, D3D12_RESOURCE_STATE_COPY_DEST));
	// UpdateSubresources辅助函数会先将数据从CPU端内存复制到中介位置的上传堆，接着调用ID3D12CommandList::CopySubresourceRegion函数，将上传堆的数据复制到默认堆上
	UpdateSubresources<1>(pCommandList, pDefaultBuffer.Get(), pUploadBuffer.Get(), 0, 0, 1, &subResourceData);
	// 数据拷贝完成，将默认堆缓冲区资源转换为读取状态
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		pDefaultBuffer.Get()
		, D3D12_RESOURCE_STATE_COPY_DEST
		, D3D12_RESOURCE_STATE_GENERIC_READ));

	// 注意，工具函数没有清理UploadBuffer，因为这里只是填充指令列表，而不是立即执行，需要等到复制指令确实完成后，再销毁UploadBuffer
	return pDefaultBuffer;
}

D3D12_VERTEX_BUFFER_VIEW MeshGeometry::GetVertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW vbv{};
	vbv.BufferLocation = m_VertexBufferGPU->GetGPUVirtualAddress();
	vbv.SizeInBytes = m_VertexSize;
	vbv.StrideInBytes = m_VertexStride;

	return vbv;
}

D3D12_INDEX_BUFFER_VIEW MeshGeometry::GetIndexBufferView() const
{
	D3D12_INDEX_BUFFER_VIEW ibv{};
	ibv.BufferLocation = m_IndexBufferGPU->GetGPUVirtualAddress();
	ibv.Format = m_IndexFormat;
	ibv.SizeInBytes = m_IndexSize;

	return ibv;
}