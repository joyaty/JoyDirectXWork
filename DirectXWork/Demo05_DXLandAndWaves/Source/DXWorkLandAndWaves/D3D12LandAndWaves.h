/**
 * D3D12LandAndWaves.h
 * 陆地与波浪示例程序Direct3D代码头文件声明
 */

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"
#include "DirectXBaseWork/MathUtil.h"

// 前置声明 - Mesh数据
struct MeshGeometry;
// 前置声明 - 渲染项
struct LandAndWavesRenderItem;
// 前置声明 - 帧资源
struct LandAndWavesFrameResource;
// 前置声明 - 波浪网格
class Wave;

/// <summary>
/// 陆地与波浪示例程序Direct3D代码
/// </summary>
class D3D12LandAndWaves : public DirectXBaseWork
{
public:
	D3D12LandAndWaves(std::wstring title, UINT width = 1280U, UINT height = 720U);
	virtual ~D3D12LandAndWaves();

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
	/// 更新物体的常量缓冲区数据
	/// </summary>
	void UpdateObjectCBs(float deltaTime, float totalTime);
	/// <summary>
	/// 更新渲染过程的常量缓冲区数据
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdatePassCBs(float deltaTime, float totalTime);
	/// <summary>
	/// 更新相机(观察者)的变换矩阵
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdateCamera(float deltaTime, float totalTime);
	/// <summary>
	/// 更新波浪顶点
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdateWave(float deltaTime, float totalTime);
	/// <summary>
	/// 构建输入布局
	/// </summary>
	void BuildInputLayout();
	/// <summary>
	/// 编译Shader
	/// </summary>
	void CompileShaderFile();
	/// <summary>
	/// 构建地表网格
	/// </summary>
	void BuildGrid();
	/// <summary>
	/// 构建波浪网格
	/// </summary>
	void BuildWave();
	/// <summary>
	/// 地形高度计算，随机生成地形
	/// </summary>
	/// <param name="x">XZ平面位置X</param>
	/// <param name="z">XZ平面位置Z</param>
	float GetHillsHeight(float x, float z);
	/// <summary>
	/// 构建渲染项
	/// </summary>
	void BuildRenderItem();
	/// <summary>
	/// 构建帧资源
	/// </summary>
	void BuildFrameResource();
	/// <summary>
	/// 构建根签名
	/// </summary>
	void BuildRootSignature();
	/// <summary>
	/// 构建渲染管线状态对象
	/// </summary>
	void BuildPipelineState();
	/// <summary>
	/// 绘制渲染项
	/// </summary>
	void DrawRenderItem();
	/// <summary>
	/// 记录渲染指令
	/// </summary>
	void PopulateCommandList();

private:
	/// <summary>
	/// 顶点输入布局
	/// </summary>
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayouts{};
	/// <summary>
	/// 顶点着色器程序编译后字节码
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_VSByteCode{ nullptr };
	/// <summary>
	/// 像素着色器程序编译后字节码
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

private:
	/// <summary>
	/// 物体映射缓存表
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_Meshes{};

	/// <summary>
	/// 波浪网格数据
	/// </summary>
	std::unique_ptr<Wave> m_pWave{ nullptr };

	/// <summary>
	/// 所有的渲染项集合
	/// </summary>
	std::vector<std::unique_ptr<LandAndWavesRenderItem>> m_AllRenderItems{};

	/// <summary>
	/// 帧资源集合
	/// </summary>
	std::vector<std::unique_ptr<LandAndWavesFrameResource>> m_AllFrameResources{};
	/// <summary>
	/// 当前使用帧资源索引
	/// </summary>
	int m_CurrentFrameResourceIndex{ 0 };
	/// <summary>
	/// 当前使用的帧资源
	/// </summary>
	LandAndWavesFrameResource* m_pCurrentFrameResource{ nullptr };

	/// <summary>
	/// 本地世界变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_WorldMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 观察变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_ViewMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 投影变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_ProjMatrix{ MathUtil::Identity4x4() };

	/// <summary>
	/// 球坐标系半径
	/// </summary>
	float m_Radius{ 150.f };
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
};