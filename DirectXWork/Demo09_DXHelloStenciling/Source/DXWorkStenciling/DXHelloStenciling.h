/**
 * DXHelloStenciling.h
 * Demo09_DXHelloStenciling练习项目，模板知识练习项目头文件
 **/

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"
#include "HelloStencilingRenderItem.h"
#include "HelloStencilingFrameResource.h"

 /// <summary>
 /// 模板练习项目
 /// </summary>
class DXHelloStenciling : public DirectXBaseWork
{
public:
	DXHelloStenciling(std::wstring title, UINT width = 1280U, UINT height = 720U);
	virtual ~DXHelloStenciling();

public:
	void SetEnableFog(bool enableFog) { BuildPSOs(enableFog, m_FillMode); }
	void SetFillMode(D3D12_FILL_MODE fillMode) { BuildPSOs(m_EnableFog, fillMode); }

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
	/// 构造房间网格几何体
	/// </summary>
	void BuildRoomGeoMesh();
	/// <summary>
	/// 创建骷髅网格集合体
	/// </summary>
	void BuildSkullGeoMesh();
	/// <summary>
	/// 加载纹理文件
	/// </summary>
	void LoadTexture();
	/// <summary>
	/// 创建静态采样器
	/// </summary>
	void BuildStaticSampler();
	/// <summary>
	/// 创建材质
	/// </summary>
	void BuildMaterial();
	/// <summary>
	/// 创建渲染项
	/// </summary>
	void BuildRenderItem();
	/// <summary>
	/// 构建帧资源
	/// </summary>
	void BuildFrameResource();
	/// <summary>
	/// 创建根签名
	/// </summary>
	void BuildRootSignature();
	/// <summary>
	/// 创建输入布局描述
	/// </summary>
	void BuildInputLayout();
	/// <summary>
	/// 编译Shader
	/// </summary>
	void CompileShaderFiles();
	/// <summary>
	/// 创建渲染管线状态对象
	/// </summary>
	/// <param name="enableFog"></param>
	/// <param name="fillMode"></param>
	void BuildPSOs(bool enableFog, D3D12_FILL_MODE fillMode);

	/// <summary>
	/// 更新相机观察矩阵
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdateCamera(float deltaTime, float totalTime);
	/// <summary>
	/// 更新物体常量缓冲区
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdateObjectCB(float deltaTime, float totalTime);
	/// <summary>
	/// 更新材质常量缓冲区
	/// </summary>
	/// <param name="delteTime"></param>
	/// <param name="totalTime"></param>
	void UpdateMaterialCB(float delteTime, float totalTime);
	/// <summary>
	/// 更新渲染过程常量缓冲区
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdatePassCB(float deltaTime, float totalTime);

	/// <summary>
	/// 记录渲染指令
	/// </summary>
	void PopulateCommandList();
	/// <summary>
	/// 绘制渲染项
	/// </summary>
	/// <param name="renderItems"></param>
	void DrawRenderItem(const std::vector<HelloStencilingRenderItem*>& renderItems);

private:
	/// <summary>
	/// 场景中所有的物体
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_SceneObjects{};
	/// <summary>
	/// 场景中所用的全部材质
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<HelloStencilingMaterial>> m_AllMaterials{};
	/// <summary>
	/// 场景中所用的全部纹理
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<Texture>> m_AllTextures{};

	/// <summary>
	/// 场景中所有的渲染项
	/// </summary>
	std::vector<std::unique_ptr<HelloStencilingRenderItem>> m_AllRenderItem{};
	/// <summary>
	/// 按渲染层级归类的渲染项
	/// </summary>
	std::vector<HelloStencilingRenderItem*> m_RenderItemLayouts[static_cast<int>(EnumRenderLayer::Count)];

	/// <summary>
	/// 帧资源
	/// </summary>
	std::vector<std::unique_ptr<HelloStencilingFrameResource>> m_AllFrameResources{};
	/// <summary>
	/// 当前激活的帧资源
	/// </summary>
	HelloStencilingFrameResource* m_ActiveFrameResource{ nullptr };
	/// <summary>
	/// 当前激活的帧资源索引
	/// </summary>
	int m_ActiveFrameResourceIndex{ 0 };

	/// <summary>
	/// 地面宽度
	/// </summary>
	float m_FloorWidh{ 30.f };

private:
	/// <summary>
	/// 着色器资源描述符堆
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SRVDescriptorHeap{ nullptr };
	/// <summary>
	/// 标准顶点着色器
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_StandardVSByteCode{ nullptr };
	/// <summary>
	/// 标准像素着色器
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_StandardPSByteCode{ nullptr };
	/// <summary>
	/// 启用雾效的像素着色器
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_EnableFogPSByteCode{ nullptr };
	/// <summary>
	/// 根签名
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature{ nullptr };
	/// <summary>
	/// 输入布局元素描述
	/// </summary>
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputElementDesces{};
	/// <summary>
	/// 静态采样器
	/// </summary>
	std::array<CD3DX12_STATIC_SAMPLER_DESC, 6U> m_StaticSamplerDescs{};
	/// <summary>
	/// 渲染管线状态对象
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PSOs[static_cast<int>(EnumRenderLayer::Count)]{};

private:
	/// <summary>
	/// 观察变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_ViewMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 投影变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_ProjMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 相机距离观察点距离
	/// </summary>
	float m_CameraDistance{ 50.f };
	/// <summary>
	/// 相机视角与Y轴夹角
	/// </summary>
	float m_CameraTheta{ DirectX::XM_PIDIV4 };
	/// <summary>
	/// 相机视角在XZ平面投影与Z轴夹角
	/// </summary>
	float m_CameraPhi{ -0.75f * DirectX::XM_PI };
	/// <summary>
	/// 相机世界空间位置
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
	/// FOV角，默认45度
	/// </summary>
	float m_FOV{ DirectX::XM_PIDIV4 };

	/// <summary>
	/// 上次点击位置
	/// </summary>
	POINT m_LastMousePos{ 0, 0 };

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
	/// 直接光在XZ平面投影与Z轴的夹角
	/// </summary>
	float m_LightPhi{ 0.75f * DirectX::XM_PI };

private:
	bool m_EnableFog{ false };
	D3D12_FILL_MODE m_FillMode{ D3D12_FILL_MODE_SOLID };
};