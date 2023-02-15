/**
 * DXWavesWithBlend.h
 * Demo08_DXWavesWithBlend示例项目主实现头文件
 **/

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"
#include "WavesWithBlendRenderItem.h"
#include "WavesWithBlendFrameResource.h"

/// <summary>
/// Demo08_DXWavesWithBlend示例项目主实现
/// </summary>
class DXWavesWithBlend : public DirectXBaseWork
{
public:
	DXWavesWithBlend(std::wstring title, UINT width = 1280U, UINT height = 720U);
	virtual ~DXWavesWithBlend();

public:
	void OnResize(UINT width, UINT height) override;
	void OnMouseDown(UINT8 keyCode, int x, int y) override;
	void OnMouseUp(UINT8 keyCode, int x, int y) override;
	void OnMouseMove(UINT8 keyCode, int x, int y) override;

protected:
	bool OnInit() override;
	void OnUpdate(float deltaTime, float totalTime) override;
	void OnRender() override;
	void OnDestroy() override;

private:
	/// <summary>
	/// 创建地形，构建山地的顶点缓冲区和索引缓冲区
	/// </summary>
	void BuildTerrainMesh();
	/// <summary>
	/// 创建水波网格数据
	/// </summary>
	void BuildWavesMesh();
	/// <summary>
	/// 创建围栏立方体网格数据
	/// </summary>
	void BuildFenceCubeMesh();
	/// <summary>
	/// 加载纹理
	/// </summary>
	void LoadTextures();
	/// <summary>
	/// 创建着色器资源描述符堆
	/// </summary>
	void BuildSrvHeap();
	/// <summary>
	/// 创建静态采样器
	/// </summary>
	void BuildStaticSampler();
	/// <summary>
	/// 创建纹理
	/// </summary>
	void BuildMaterials();
	/// <summary>
	/// 创建渲染项
	/// </summary>
	void BuildRenderItems();
	/// <summary>
	/// 获取Terrain地形偏移高度
	/// </summary>
	/// <param name="x"></param>
	/// <param name="z"></param>
	/// <returns></returns>
	float GetHillsHeight(float x, float z) const;
	/// <summary>
	/// 获取Terrain地形偏移后的法向量，微积分计算
	/// </summary>
	/// <param name="x"></param>
	/// <param name="z"></param>
	/// <returns></returns>
	DirectX::XMFLOAT3 GetHillsNormal(float x, float z) const;

	/// <summary>
	/// 创建顶点输入布局描述
	/// </summary>
	void BuildInputLayout();
	/// <summary>
	/// 编译Shader
	/// </summary>
	void CompileShaderFiles();
	/// <summary>
	/// 创建根签名
	/// </summary>
	void BuildRootSignature();
	/// <summary>
	/// 创建渲染管线状态对象
	/// </summary>
	void BuildPSOs();
	/// <summary>
	/// 创建帧资源
	/// </summary>
	void BuildFrameResource();

private:
	/// <summary>
	/// 场景上所有的对象
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_SceneObjects{};
	/// <summary>
	/// 场景中所有使用纹理
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<Texture>> m_AllTextures{};
	/// <summary>
	/// 场景中所有使用的材质
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<WavesWithBlendMaterial>> m_AllMaterials{};
	/// <summary>
	/// 全部的渲染项
	/// </summary>
	std::vector<std::unique_ptr<WavesWithBlendRenderItem>> m_AllRenderItems{};
	/// <summary>
	/// 不透明渲染项
	/// </summary>
	std::vector<WavesWithBlendRenderItem*> m_OpaqueRenderItems{};
	/// <summary>
	/// 透明渲染项
	/// </summary>
	std::vector<WavesWithBlendRenderItem*> m_TransparentItems{};
	/// <summary>
	/// 输入布局元素描述
	/// </summary>
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputElementDescs{};

	/// <summary>
	/// 帧资源
	/// </summary>
	std::vector<std::unique_ptr<WavesWithBlendFrameResource>> m_FrameResources;
	/// <summary>
	/// 当前使用的帧资源
	/// </summary>
	WavesWithBlendFrameResource* m_CurrentFrameResource;
	/// <summary>
	/// 当前的帧资源索引
	/// </summary>
	int m_CurrentFrameResourceIndex;

	/// <summary>
	/// 静态采样器描述
	/// </summary>
	std::array<CD3DX12_STATIC_SAMPLER_DESC, 6> m_StaticSamplerDesc{};
	/// <summary>
	/// 着色器资源描述符堆
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SrvDescriptorHeap{ nullptr };

};