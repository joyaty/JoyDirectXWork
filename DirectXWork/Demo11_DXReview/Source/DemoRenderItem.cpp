
#include "stdafx.h"
#include "DemoRenderItem.h"


D3D12_VERTEX_BUFFER_VIEW MeshGeometry::GetVertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	vbView.BufferLocation = m_VertexBufferGPU->GetGPUVirtualAddress();
	vbView.SizeInBytes = m_VertexSize;
	vbView.StrideInBytes = m_VertexStride;
	return vbView;
}

D3D12_INDEX_BUFFER_VIEW MeshGeometry::GetIndexBufferView() const
{
	D3D12_INDEX_BUFFER_VIEW ibView{};
	ibView.BufferLocation = m_IndexBufferGPU->GetGPUVirtualAddress();
	ibView.SizeInBytes = m_IndexSize;
	ibView.Format = m_IndexFormat;
	return ibView;
}