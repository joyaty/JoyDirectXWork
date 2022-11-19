/*
 * D3D12HelloTriangle.h
 * Demo02 - DirectX12渲染三角形
 */

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"

namespace HelloTriangle
{
	/// <summary>
	/// HelloTriangle范例的顶点数据结构
	/// </summary>
	struct Vertex
	{
	public:
		/// <summary>
		/// 顶点坐标
		/// </summary>
		DirectX::XMFLOAT3 position;

		/// <summary>
		/// 顶点颜色
		/// </summary>
		DirectX::XMFLOAT4 color;
	};
};

struct MeshGeometry;

/// <summary>
/// DX12范例 - HelloTriangle
/// </summary>
class D3D12HelloTriangle : public DirectXBaseWork
{
public:
	D3D12HelloTriangle(std::wstring title, UINT width = 1280U, UINT height = 720U);
	virtual ~D3D12HelloTriangle();

protected:
	virtual bool OnInit() override;
	virtual void OnUpdate() override;
	virtual void OnRender() override;
	virtual void OnDestroy() override;

private:
	/// <summary>
	/// 初始化输入布局
	/// </summary>
	void CreateInputLayout();

	/// <summary>
	/// 创建三角形Mesh数据
	/// </summary>
	void CreateTriangleMesh();

	/// <summary>
	/// 编译Shader
	/// </summary>
	void CompileShaderFile();

	/// <summary>
	/// 创建根签名
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// 创建渲染管线状态对象
	/// </summary>
	void CreatePipelineState();

	/// <summary>
	/// 每帧的渲染指令
	/// </summary>
	void PopulateCommandList();

private:
	/// <summary>
	/// 顶点输入布局描述
	/// </summary>
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputElementDescs{};

	/// <summary>
	/// 三角形数据
	/// </summary>
	std::unique_ptr<MeshGeometry> m_TriangleMesh{ nullptr };

	/// <summary>
	/// 三角形顶点缓冲区
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBuffer{ nullptr };
	/// <summary>
	/// 三角形顶点缓冲区视图
	/// </summary>
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView{};

	/// <summary>
	/// 顶点着色器编译后字节码
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_VertexShaderByteCode{ nullptr };

	/// <summary>
	/// 像素着色器编译后字节码
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_PixelShaderByteCode{ nullptr };

	/// <summary>
	/// 根签名
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature{ nullptr };

	/// <summary>
	/// 渲染管线状态对象
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PSO{ nullptr };
};