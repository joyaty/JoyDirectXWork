/**
 * DXWavesWithBlend.h
 * Demo08_DXWavesWithBlend示例项目主实现头文件
 **/

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"
#include "WavesWithBlendRenderItem.h"
#include "WavesWithBlendFrameResource.h"
#include "Wave.h"

/// <summary>
/// Demo08_DXWavesWithBlend示例项目主实现
/// </summary>
class DXWavesWithBlend : public DirectXBaseWork
{
public:
	DXWavesWithBlend(std::wstring title, UINT width = 1280U, UINT height = 720U);
	virtual ~DXWavesWithBlend();

public:
	void SwitchFrogState(bool enableFrog) { BuildPSOs(enableFrog); };
	void SetBlendFactor(float r, float g, float b, float a)
	{
		m_BlendFactor[0] = r;
		m_BlendFactor[1] = g;
		m_BlendFactor[2] = b;
		m_BlendFactor[3] = a;
	}

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
	void BuildPSOs(bool enableFrog);
	/// <summary>
	/// 创建帧资源
	/// </summary>
	void BuildFrameResource();

	/// <summary>
	/// 更新对象常量
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdateObjectConstant(float deltaTime, float totalTime);
	/// <summary>
	/// 更新材质常量
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdateMaterialConstant(float deltaTime, float totalTime);
	/// <summary>
	/// 更新渲染过程长
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdatePassConstant(float deltaTime, float totalTime);
	/// <summary>
	/// 更新相机观察空间矩阵
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdateCamera(float deltaTime, float totalTime);
	/// <summary>
	/// 更新波浪动画顶点数据
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdateWave(float deltaTime, float totalTime);
	/// <summary>
	/// 水体流动
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void WaveFlow(float deltaTime, float totalTime);

	/// <summary>
	/// 记录渲染指令
	/// </summary>
	void PopulateCommandList();
	/// <summary>
	/// 绘制指定层级渲染项
	/// </summary>
	/// <param name="renderItems"></param>
	void DrawRenderItem(const std::vector<WavesWithBlendRenderItem*>& renderItems);

private:
	std::unique_ptr<Wave> m_Wave{ nullptr };
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
	/// 按RenderLayer归类渲染项
	/// </summary>
	std::vector<WavesWithBlendRenderItem*> m_RenderItemLayers[static_cast<int>(EnumRenderLayer::Count)];
	/// <summary>
	/// 渲染管线对象，opaque/transparent使用不同的渲染管线状态设置
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PSOs[static_cast<int>(EnumRenderLayer::Count)];

	/// <summary>
	/// 顶点着色器编译后字节码
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_VSByteCode{ nullptr };
	/// <summary>
	/// 标准像素着色器编译后字节码
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_StandardPSByteCode{ nullptr };
	/// <summary>
	/// 开启雾效的像素着色器编译后字节码
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_EnableFrogPSByteCode{ nullptr };
	/// <summary>
	/// 带有AlphaTest预编译宏的像素着色器编译后字节码
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_PSWithAlphaTestByteCode{ nullptr };
	/// <summary>
	/// 带有AlphaTest预编译宏的像素着色器编译后字节码
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_EnableFrogPSWithAlphaTestByteCode{ nullptr };

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
	std::array<CD3DX12_STATIC_SAMPLER_DESC, 6> m_StaticSamplerDescs{};
	/// <summary>
	/// 着色器资源描述符堆
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SrvDescriptorHeap{ nullptr };
	/// <summary>
	/// 根签名
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignture{ nullptr };

private:
	/// <summary>
	/// 观察矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_ViewMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 投影矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_ProjMatrix{ MathUtil::Identity4x4() };

	/// <summary>
	/// 相机观察距离
	/// </summary>
	float m_CameraDistance{ 150.f };
	/// <summary>
	/// 相机视角和Y轴的夹角Theta
	/// </summary>
	float m_CameraTheta{ DirectX::XM_PIDIV4 };
	/// <summary>
	/// 相机视角在XZ平面投影与X轴的夹角
	/// </summary>
	float m_CameraPhi{ 1.5f * DirectX::XM_PI };
	/// <summary>
	/// 相机位置
	/// </summary>
	DirectX::XMFLOAT3 m_CameraPos{};
	/// <summary>
	/// 近平面
	/// </summary>
	float m_NearZ{ 1.f };
	/// <summary>
	/// 远平面
	/// </summary>
	float m_FarZ{ 1000.f };

	/// <summary>
	/// 上次鼠标位置
	/// </summary>
	POINT m_LastMousePos{};

private:
	/// <summary>
	/// 环境光
	/// </summary>
	DirectX::XMFLOAT4 m_AmbientLight{ DirectX::XMFLOAT4(0.25f, 0.25f, 0.35f, 1.0f) };
	/// <summary>
	/// 直接光
	/// </summary>
	DirectX::XMFLOAT3 m_DirectLight{ DirectX::XMFLOAT3(0.7f, 0.7f, 0.7f) };
	/// <summary>
	/// 直接光与Y轴的夹角
	/// </summary>
	float m_LightTheta{ 0.3f * DirectX::XM_PI };
	/// <summary>
	/// 直接光在XZ平面投影与X轴的夹角
	/// </summary>
	float m_LightPhi{ DirectX::XM_PI };

private:
	/// <summary>
	/// 混合因子
	/// </summary>
	float m_BlendFactor[4];
};