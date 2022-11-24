/*
 * D3D12HelloCube.h 
 * Demo02 - DirectX12渲染立方体
 */

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include "DirectXBaseWork/MathUtil.h"
#include "DirectXBaseWork/UploadBuffer.h"

struct MeshGeometry;

/// <summary>
/// 顶点数据结构体
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

/// <summary>
/// 常量缓冲区数据结构
/// </summary>
struct ObjectConstants
{
public:
	/// <summary>
	/// 世界到裁剪空间的变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 worldViewProjMatrix = MathUtil::Identity4x4();
};

/// <summary>
/// Direct3D绘制一个立方体
/// </summary>
class D3D12HelloCube : public DirectXBaseWork
{
public:
	D3D12HelloCube(std::wstring title, UINT width = 1280U, UINT height = 720U);
	virtual ~D3D12HelloCube();

public:
	void OnMouseDown(UINT8 keyCode, int x, int y) override;
	void OnMouseUp(UINT8 keyCode, int x, int y) override;
	void OnMouseMove(UINT8 keyCode, int x, int y) override;

protected:
	bool OnInit() override;
	void OnUpdate() override;
	void OnRender() override;
	void OnDestroy() override;

private:
	/// <summary>
	/// 创建输入布局描述
	/// </summary>
	void CreateInputLayout();

	/// <summary>
	/// 创建常量缓冲区
	/// </summary>
	void CreateConstantBuffer();

	/// <summary>
	/// 创建根签名
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// 创建立方体几何体
	/// </summary>
	void CreateCubeGeometry();

	/// <summary>
	/// 编译Shader
	/// </summary>
	void CompileShaderFile();

	/// <summary>
	/// 构建渲染管线状态
	/// </summary>
	void BuildGraphicsPiplineState();

	/// <summary>
	/// 记录渲染指令
	/// </summary>
	void PopulateCommandList();

private:
	/// <summary>
	/// 顶点输入布局描述
	/// </summary>
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputElementDescs;

	/// <summary>
	/// 立方体Mesh数据结构
	/// </summary>
	std::unique_ptr<MeshGeometry> m_pMeshGeometry{ nullptr };

	/// <summary>
	/// 常量缓冲区
	/// </summary>
	std::unique_ptr<UploadBuffer<ObjectConstants>> m_pConstantBuffer{ nullptr };

	/// <summary>
	/// 常量缓冲区描述符堆
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CBVHeap{ nullptr };

	/// <summary>
	/// 根签名
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature{ nullptr };

	/// <summary>
	/// 顶点着色器字节码
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_VertexShaderByteCode{ nullptr };
	/// <summary>
	/// 像素着色器字节码
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_PixelShaderByteCode{ nullptr };

	/// <summary>
	/// 渲染管线状态对象
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PSO{ nullptr };

private:
	/// <summary>
	/// 上次鼠标位置
	/// </summary>
	POINT m_LastMousePos{};

	/// <summary>
	/// 球坐标系
	/// </summary>
	float m_Theta{ 1.5f * XM_PI };
	/// <summary>
	/// 球坐标系
	/// </summary>
	float m_Phi{ XM_PIDIV4 };
	/// <summary>
	/// 球坐标半径
	/// </summary>
	float m_Radius{ 5.0f };
};
