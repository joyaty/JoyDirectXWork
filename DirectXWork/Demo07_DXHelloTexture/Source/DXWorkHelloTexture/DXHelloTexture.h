/**
 * DXHelloTexture.h
 * 纹理贴图实例程序头文件
 **/

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"
#include <DirectXBaseWork/D3D12Util.h>

/// <summary>
/// 纹理贴图示例程序
/// </summary>
class DXHelloTexture : public DirectXBaseWork
{
public:
	DXHelloTexture(std::wstring title, UINT width, UINT height);
	virtual ~DXHelloTexture();

public:
	void OnMouseDown(UINT8 keyCode, int x, int y) override;
	void OnMouseUp(UINT8 keyCode, int x, int y) override;
	void OnMouseMove(UINT8 keyCode, int x, int y) override;
	void OnResize(UINT width, UINT height) override;

protected:
	bool OnInit() override;
	void OnUpdate(float deltaTime, float totalTime) override;
	void OnRender() override;
	void OnDestroy() override;

private:
	/// <summary>
	/// 初始化顶点输入布局
	/// </summary>
	void BuildInputLayout();
	/// <summary>
	/// 编译Shader文件
	/// </summary>
	void CompileShaderFile();
	/// <summary>
	/// 初始化根签名
	/// </summary>
	void BuildRootSignature();
	/// <summary>
	/// 初始化渲染管线状态对象
	/// </summary>
	void BuildPSO();
	/// <summary>
	/// 初始化渲染几何体
	/// </summary>
	void BuildGeometry();
	/// <summary>
	/// 初始化材质
	/// </summary>
	void BuildMaterial();
	/// <summary>
	/// 初始化帧资源
	/// </summary>
	void BuildFrameResource();

	/// <summary>
	/// 更新物体常量缓冲区
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdateObjectCB(float deltaTime, float totalTime);
	/// <summary>
	/// 更新材质常量缓冲区
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdateMatCB(float deltaTime, float totalTime);
	/// <summary>
	/// 更新渲染过程常量缓冲区
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdatePassCB(float deltaTime, float totalTime);

private:
	/// <summary>
	/// 输入布局描述
	/// </summary>
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputElements{};
	/// <summary>
	/// 顶点着色器编译后字节码
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_VSByteCode{ nullptr };
	/// <summary>
	/// 像素着色器编译后字节码
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

	/// <summary>
	/// 场景中所有的物体
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_Geometrys{};
};