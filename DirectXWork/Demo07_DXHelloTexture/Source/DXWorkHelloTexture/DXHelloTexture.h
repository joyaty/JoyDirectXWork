/**
 * DXHelloTexture.h
 * 纹理贴图实例程序头文件
 **/

#pragma once

#include <DirectXBaseWork/DirectXBaseWork.h>
#include <DirectXBaseWork/D3D12Util.h>
#include "HelloTextureRenderItem.h"
#include "HelloTextureFrameResource.h"

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
	/// 加载纹理
	/// </summary>
	void LoadTexture();
	/// <summary>
	/// 初始化采样器
	/// </summary>
	void BuildSampler();
	/// <summary>
	/// 初始化材质
	/// </summary>
	void BuildMaterial();
	/// <summary>
	/// 构建渲染项
	/// </summary>
	void BuildRenderItem();
	/// <summary>
	/// 初始化帧资源
	/// </summary>
	void BuildFrameResource();

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
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdateMatCB(float deltaTime, float totalTime);
	/// <summary>
	/// 更新渲染过程常量缓冲区
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdatePassCB(float deltaTime, float totalTime);
	/// <summary>
	/// 键盘输入时间，控制光照方向
	/// </summary>
	void OnKeyboardInput(float deltaTime, float totalTime);

	/// <summary>
	/// 记录渲染名字
	/// </summary>
	void PopulateCommandList();
	/// <summary>
	/// 绘制渲染项
	/// </summary>
	void DrawRenderItem();

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
	/// 着色器资源描述符堆
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SRVDescriptorHeap{ nullptr };
	/// <summary>
	/// 采样器描述符堆
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SamplerDescriptorHeap{ nullptr };

	/// <summary>
	/// 场景中所有的物体
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_Geometrys{};
	/// <summary>
	/// 场景中所用的纹理
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<Texture>> m_Textures{};
	/// <summary>
	/// 所有的材质
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<HelloTextureMaterial>> m_Materials{};
	/// <summary>
	/// 所有的渲染项
	/// </summary>
	std::vector<std::unique_ptr<HelloTextureRenderItem>> m_AllRenderItems{};

	/// <summary>
	/// 所有的帧资源
	/// </summary>
	std::vector<std::unique_ptr<HelloTextureFrameResource>> m_FrameResourses{};
	/// <summary>
	/// 当前使用的帧资源索引
	/// </summary>
	int m_CurrentFrameResourceIndex{ 0 };
	/// <summary>
	/// 当前使用的帧资源
	/// </summary>
	HelloTextureFrameResource* m_pCurrentFrameResource{ nullptr };

	/// <summary>
	/// 投影矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_ProjMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 观察矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_ViewMatrix{ MathUtil::Identity4x4() };

	/// <summary>
	/// 环境光
	/// </summary>
	DirectX::XMFLOAT4 m_AmbientLight{ 0.25f, 0.25f, 0.35f, 1.0f };
	/// <summary>
	/// 直接光
	/// </summary>
	DirectX::XMFLOAT3 m_DirectLight{ 0.8f, 0.7f, 0.7f };

private:
	/// <summary>
	/// 球坐标系半径
	/// </summary>
	float m_Radius{ 10.f };
	/// <summary>
	/// 位矢与Y轴的夹角
	/// </summary>
	float m_Theta{ DirectX::XM_PIDIV4 };
	/// <summary>
	/// 位矢在XZ平面的投影和X轴的夹角
	/// </summary>
	float m_Phi{ 1.5f * DirectX::XM_PI };
	/// <summary>
	/// 观察者(相机)位置信息
	/// </summary>
	DirectX::XMFLOAT3 m_EyesPos{};
	/// <summary>
	/// 上次鼠标位置
	/// </summary>
	POINT m_LastMousePos{};

	/// <summary>
	/// 方向光位矢与Y轴的夹角
	/// </summary>
	float m_SunTheta{ 0.25f * DirectX::XM_PI };
	/// <summary>
	/// 方向光位矢在XZ平面的投影和X轴的夹角
	/// </summary>
	float m_SunPhi{ -0.25f * DirectX::XM_PI };
};