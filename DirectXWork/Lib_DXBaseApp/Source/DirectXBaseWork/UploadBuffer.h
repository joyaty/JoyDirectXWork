/*
 * 上传堆中创建资源的模板类
 * UploadBuffer.h
 */

#pragma once

#include "D3D12Util.h"
#include "DXWorkHelper.h"

 /// <summary>
 /// T类型资源的上传缓冲区封装
 /// </summary>
 /// <typeparam name="T"></typeparam>
template<typename T> class UploadBuffer
{
public:
	/// <summary>
	/// 构造上传缓冲区
	/// </summary>
	/// <param name="pDevice">GPU逻辑设备</param>
	/// <param name="elementCount">元素个数(多个相同数据结构的常量缓冲区)</param>
	/// <param name="isConstantBuffer">是否是常量缓冲区</param>
	UploadBuffer(ID3D12Device* pDevice, UINT elementCount, bool isConstantBuffer)
		: m_IsConstanctBuffer(isConstantBuffer)
	{
		// 获取上传缓冲区中数据元素的字节大小
		m_ElementByteSize = sizeof(T);
		// 如果上传缓冲区是常量缓冲区，元素大小需要对齐为256的倍数
		if (m_IsConstanctBuffer)
		{
			m_ElementByteSize = D3D12Util::ConstantBufferByteSizeAlignment(m_ElementByteSize);
		}
		// 创建上传缓冲区资源
		ThrowIfFailed(pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(m_ElementByteSize * elementCount),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(m_UploadBuffer.GetAddressOf())));

		// 获取待映射资源数据的目标内存块地址
		m_UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_MappingData));
	}

	// 禁用拷贝和复制
	UploadBuffer(const UploadBuffer& rhs) = delete;
	UploadBuffer& operator=(const UploadBuffer& rhs) = delete;

	virtual ~UploadBuffer()
	{
		if (m_UploadBuffer != nullptr)
		{
			m_UploadBuffer->Unmap(0, nullptr);
		}
		m_MappingData = nullptr;
	}

	/// <summary>
	/// 更新上传缓冲区的数据
	/// </summary>
	/// <param name="elementIndex"></param>
	/// <param name="data"></param>
	void CopyData(int elementIndex, const T& data)
	{
		// 拷贝数据到目标内存块
		memcpy(&m_MappingData[elementIndex * m_ElementByteSize], &data, sizeof(T));
	}

	/// <summary>
	/// 获取上传缓冲区资源
	/// </summary>
	/// <returns></returns>
	ID3D12Resource* GetResource() const
	{
		return m_UploadBuffer.Get();
	}

	/// <summary>
	/// 上传缓冲区资源元素大小
	/// </summary>
	/// <returns></returns>
	UINT GetElementSize() const
	{
		return m_ElementByteSize;
	}

private:
	/// <summary>
	/// 上传堆中的缓冲区资源
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> m_UploadBuffer{ nullptr };

	/// <summary>
	/// 缓冲区字节大小
	/// </summary>
	UINT m_ElementByteSize = 0;

	/// <summary>
	/// 是否是常量缓冲区资源
	/// </summary>
	bool m_IsConstanctBuffer = false;

	/// <summary>
	/// 待映射资源数据的目标内存块
	/// </summary>
	BYTE* m_MappingData{ nullptr };
};