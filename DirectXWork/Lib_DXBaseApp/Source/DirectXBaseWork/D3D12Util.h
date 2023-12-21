/*
 * Direct3D示例中的一些工具方法和通用结构体定义
 * D3D12Util.h
 */

#pragma once

#include <unordered_map>

/// <summary>
/// Direct3D示例程序的一些工具方法
/// </summary>
class D3D12Util
{
public:
	/// <summary>
	/// 在默认堆上创建缓冲区资源
	/// </summary>
	/// <param name="pDevice">D3D12逻辑设备</param>
	/// <param name="pCommandList">指令列表</param>
	/// <param name="pInitData">缓冲区要复制数据</param>
	/// <param name="bufferSize">缓冲区数据大小，缓冲区大小必须可以容纳下数据</param>
	/// <param name="pUploadBuffer">上传堆上的中介缓冲区</param>
	/// <returns></returns>
	static Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferInDefaultHeap(ID3D12Device* pDevice
		, ID3D12GraphicsCommandList* pCommandList
		, const void* pInitData, UINT bufferSize
		, Microsoft::WRL::ComPtr<ID3D12Resource>& pUploadBuffer
	);

	/// <summary>
	/// 将bytesize对齐到最小256的整数倍
	/// ConstantBuffer的大小要求为256B的整数倍
	/// </summary>
	/// <param name="byteSize">对齐的字节数</param>
	/// <returns>满足需求字节数的最小256整数倍</returns>
	static UINT ConstantBufferByteSizeAlignment(UINT byteSize)
	{
		return (byteSize + 255) & ~255;
	}
};

/// <summary>
/// 子Mesh结构体，几个子Mesh可以构成一个大几何体Mesh
/// </summary>
struct SubMeshGeometry
{
public:
	/// <summary>
	/// 子Mesh索引数量
	/// </summary>
	UINT m_IndexCount;
	/// <summary>
	/// 子Mesh索引的起始位置，在大几何体全局索引缓冲区的偏移
	/// </summary>
	UINT m_StartIndexLocation;
	/// <summary>
	/// 子Mesh的基准顶点位置，表示子Mesh第一个顶点在全局顶点缓冲区的位置
	/// 几个子Mesh的顶点缓冲区组合成一个大的全局顶点缓冲区
	/// 顶点在大几何体顶点缓冲区的索引 = 子Mesh的本地索引 + 第一个顶点在大几何体顶点缓冲区的位置
	/// </summary>
	UINT m_BaseVertexLocation;
};

/// <summary>
/// 几何体Mesh结构体
/// </summary>
struct MeshGeometry
{
public:
	/// <summary>
	/// 几何体名字
	/// </summary>
	std::string m_Name;

	/// <summary>
	/// CPU内存上的顶点数据，顶点数据可以是泛型，故使用Blob类型
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_VertexBufferCPU{ nullptr };
	/// <summary>
	/// CPU内存上的索引数据，索引数据可以是泛型，故使用Blob类型
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_IndexBufferCPU{ nullptr };

	/// <summary>
	/// 默认堆顶点缓冲区资源
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBufferGPU{ nullptr };
	/// <summary>
	/// 上传堆顶点缓冲区中介资源
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBufferUploader{ nullptr };

	/// <summary>
	/// 默认堆索引缓冲区资源
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBufferGPU{ nullptr };
	/// <summary>
	/// 上传堆索引缓冲区中介资源
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBufferUploader{ nullptr };

	/// <summary>
	/// 顶点数据元素(单个顶点数据结构)大小，
	/// </summary>
	UINT m_VertexStride;
	/// <summary>
	/// 顶点数据的总大小
	/// </summary>
	UINT m_VertexSize;
	/// <summary>
	/// 索引数据格式，一般为UINT16或UINT32
	/// </summary>
	DXGI_FORMAT m_IndexFormat;
	/// <summary>
	/// 顶点数据的总大小
	/// </summary>
	UINT m_IndexSize;

	/// <summary>
	/// 几何体包含的子Mesh，一个复杂几何体可能由几个简单的子几何体组成而成。
	/// </summary>
	std::unordered_map<std::string, SubMeshGeometry> m_SubMeshGeometrys;

	/// <summary>
	/// 获取顶点缓冲区视图
	/// </summary>
	/// <returns></returns>
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

	/// <summary>
	/// 获取索引缓冲区视图
	/// </summary>
	/// <returns></returns>
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;
};
