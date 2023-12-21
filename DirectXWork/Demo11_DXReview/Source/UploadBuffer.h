
#pragma once

#include "Utils.h"

/// <summary>
/// 指向GPU上传堆中一块内存，以及绑定的数据的封装
/// </summary>
/// <typeparam name="T">绑定数据的类型</typeparam>
template <typename T> class UploadBuffer
{
public:
	/// <summary>
	/// 构造上传缓冲区
	/// </summary>
	/// <param name="pDevice"></param>
	/// <param name="elementCount"></param>
	/// <param name="isConstantBuffer"></param>
	UploadBuffer(ID3D12Device* pDevice, uint32_t elementCount, bool isConstantBuffer)
		: m_IsConstantBuffer(isConstantBuffer)
	{
		m_ElementByteSize = sizeof(T);
		if (m_IsConstantBuffer)
		{
			m_ElementByteSize = D3DHelper::ConstantBufferByteSizeAligment(m_ElementByteSize);
		}
		// 申请一块位于上传堆的内存资源
		ThrowIfFailed(pDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD)
			, D3D12_HEAP_FLAG_NONE
			, &CD3DX12_RESOURCE_DESC::Buffer(m_ElementByteSize * elementCount)
			, D3D12_RESOURCE_STATE_GENERIC_READ
			, nullptr
			, IID_PPV_ARGS(m_BufferResource.GetAddressOf())));
		// 获取指向该内存资源的CPU指针，后续可以通过该指针写入数据
		m_BufferResource->Map(0, nullptr, reinterpret_cast<void**>(&m_MappingData));
	}

	UploadBuffer(const UploadBuffer& rhs) = delete;
	//UploadBuffer(UploadBuffer&& rhs) = delete;
	UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
	//UploadBuffer& operator=(UploadBuffer&& rhs) = delete;

	virtual ~UploadBuffer()
	{
		if (m_BufferResource != nullptr)
		{
			// 移除CPU指针
			m_BufferResource->Unmap(0, nullptr);
		}
		m_MappingData = nullptr;
	}

	void CopyData(int elementIndex, const T& data)
	{
		memcpy(&m_MappingData[elementIndex * m_ElementByteSize], &data, sizeof(T));
	}

	ID3D12Resource* GetResource() const
	{
		return m_BufferResource.Get();
	}

	uint32_t GetElementSize() const
	{
		return m_ElementByteSize;
	}

private:
	/// <summary>
	/// 上传堆中的一块内存资源
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> m_BufferResource{ nullptr };

	/// <summary>
	/// 缓冲区的字节大小
	/// </summary>
	uint32_t m_ElementByteSize{ 0 };

	/// <summary>
	/// 是否用作常量缓冲区
	/// </summary>
	bool m_IsConstantBuffer{false};

	/// <summary>
	/// 指向所申请的GPU资源的CPU指针
	/// </summary>
	uint8_t* m_MappingData{ nullptr };
};