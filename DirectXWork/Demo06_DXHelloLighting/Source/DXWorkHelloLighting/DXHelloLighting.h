/**
 * DXHelloLighting.h
 * 光照模型实例程序头文件
 */

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"

/// <summary>
/// Direct3D构建光照模型
/// </summary>
class DXHelloLighting : public DirectXBaseWork
{
public:
	DXHelloLighting(std::wstring title, UINT width = 1280U, UINT height = 720U);
	virtual ~DXHelloLighting();

protected:
	bool OnInit() override;
	void OnUpdate(float deltaTime, float totalTime) override;
	void OnRender() override;
	void OnDestroy() override;

private:

	/// <summary>
	/// 初始化渲染项
	/// </summary>
	void BuildRenderItem();

	/// <summary>
	/// 初始化顶点输入布局
	/// </summary>
	void InitInputLayout();

	/// <summary>
	/// 编译Shader
	/// </summary>
	void CompileShaderFile();
	
	/// <summary>
	/// 构建根签名
	/// </summary>
	void BuildRootSignature();

	/// <summary>
	/// 构建渲染管线状态对象
	/// </summary>
	void BuildPSO();

private:
	/// <summary>
	/// 输入布局元素描述
	/// </summary>
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputElementDesc{};
	/// <summary>
	/// 编译后的顶点着色器字节码
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_VSByteCode{ nullptr };
	/// <summary>
	/// 编译后的像素着色器字节码
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_PSByteCode{ nullptr };
	/// <summary>
	/// 根签名
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature{ nullptr };
	/// <summary>
	/// 渲染管线状态对象
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PSO{ nullptr };
};