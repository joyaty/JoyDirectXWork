/**
 * DXWavesWithBlend.h
 * Demo08_DXWavesWithBlend示例项目主实现头文件
 **/

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"
#include "DirectXBaseWork/D3D12Util.h"
#include "DirectXBaseWork/RenderConstantsStruct.h"

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
	void BuildTerrain();
	/// <summary>
	/// 创建水波
	/// </summary>
	void BuildWaves();
	/// <summary>
	/// 创建箱子
	/// </summary>
	void BuildBox();
	/// <summary>
	/// 加载纹理
	/// </summary>
	void LoadTextures();
	/// <summary>
	/// 创建纹理
	/// </summary>
	void BuildMaterials();
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
	std::unordered_map<std::string, std::unique_ptr<Material>> m_AllMaterials{};
};