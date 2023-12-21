
#include "stdafx.h"
#include "Utils.h"

Microsoft::WRL::ComPtr<ID3D12Resource> D3DHelper::CreateBufferInDefaultHeap(ID3D12Device* pDevice
	, ID3D12GraphicsCommandList* pCommandList
	, const void* pData, uint32_t dataSize
	, Microsoft::WRL::ComPtr<ID3D12Resource>& pUploaderBuffer)
{
	// 在默认堆上创建缓冲区资源
	Microsoft::WRL::ComPtr<ID3D12Resource> pDefaultBuffer{};
	ThrowIfFailed(pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
		, D3D12_HEAP_FLAG_NONE
		, &CD3DX12_RESOURCE_DESC::Buffer(dataSize)
		, D3D12_RESOURCE_STATE_COMMON
		, nullptr
		, IID_PPV_ARGS(pDefaultBuffer.GetAddressOf())));
	// 上传堆上创建中介缓冲区，用于CPU写入数据
	ThrowIfFailed(pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD)
		, D3D12_HEAP_FLAG_NONE
		, &CD3DX12_RESOURCE_DESC::Buffer(dataSize)
		, D3D12_RESOURCE_STATE_GENERIC_READ
		, nullptr
		, IID_PPV_ARGS(pUploaderBuffer.GetAddressOf())));

	D3D12_SUBRESOURCE_DATA subResourceData{};
	subResourceData.pData = pData;
	subResourceData.RowPitch = dataSize;	// 对于缓冲区资源而言，该字段表示要复制的数据字节数
	subResourceData.SlicePitch = subResourceData.RowPitch; // 对于缓冲区资源而言，该字段表示要复制的数据字节数
	// 默认堆缓冲区资源状态转换为拷贝目标状态
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pDefaultBuffer.Get()
		, D3D12_RESOURCE_STATE_COMMON
		, D3D12_RESOURCE_STATE_COPY_DEST));
	// UpdateSubresources辅助函数会先将数据从CPU端内存复制到中介位置的上传堆，接着调用ID3D12CommandList::CopySubresourceRegion函数，将上传堆的数据复制到默认堆上
	UpdateSubresources<1>(pCommandList, pDefaultBuffer.Get(), pUploaderBuffer.Get(), 0, 0, 1, &subResourceData);
	// 数据拷贝完成，默认堆缓冲区资源状态转换为读取状态
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pDefaultBuffer.Get()
		, D3D12_RESOURCE_STATE_COPY_DEST
		, D3D12_RESOURCE_STATE_GENERIC_READ));

	return pDefaultBuffer;
}

const float MathUtil::Infinity = FLT_MAX;
const float MathUtil::Pi = 3.1415926535f;