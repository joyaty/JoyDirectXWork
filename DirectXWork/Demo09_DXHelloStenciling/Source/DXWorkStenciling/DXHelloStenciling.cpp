/**
 * DXHelloStenciling.cpp
 * Demo09_DXHelloStenciling练习项目，模板知识练习项目实现文件
 **/

#include "stdafx.h"
#include "DXHelloStenciling.h"
#include "IMGuiHelloStenciling.h"
#include "DirectXBaseWork/GeometryGenerator.h"

/// <summary>
/// 帧资源个数
/// </summary>
const int kNumFrameResource = 3;

DXHelloStenciling::DXHelloStenciling(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
	m_AssetPath += L"\\DirectXWork\\Demo09_DXHelloStenciling\\Source\\DXWorkStenciling\\";
}

DXHelloStenciling::~DXHelloStenciling()
{
}

void DXHelloStenciling::OnMouseDown(UINT8 keyCode, int x, int y)
{
}

void DXHelloStenciling::OnMouseUp(UINT8 keyCode, int x, int y)
{
}

void DXHelloStenciling::OnMouseMove(UINT8 keyCode, int x, int y)
{
}

void DXHelloStenciling::OnResize(UINT width, UINT height)
{
}

bool DXHelloStenciling::OnInit()
{
	// 等级管线初始化指令执行完毕
	FlushCommandQueue();
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));
	// 初始化HelloStenciling
	BuildGeometry();
	LoadTexture();
	BuildMaterial();
	BuildRenderItem();
	BuildFrameResource();
	BuildInputLayout();
	CompileShaderFiles();
	BuildRootSignature();
	BuildPSOs();
	// 执行HelloStenciling相关的初始化指令
	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(1, cmdList);
	FlushCommandQueue();

	return true;
}

void DXHelloStenciling::OnUpdate(float deltaTime, float totalTime)
{
	// 切换
	UpdateCamera(deltaTime, totalTime);
	UpdateObjectCB(deltaTime, totalTime);
	UpdateMaterialCB(deltaTime, totalTime);
	UpdatePassCB(deltaTime, totalTime);
}

void DXHelloStenciling::OnRender()
{
	PopulateCommandList();
}

void DXHelloStenciling::OnDestroy()
{
}

void DXHelloStenciling::BuildGeometry()
{
}

void DXHelloStenciling::BuildMaterial()
{
}

void DXHelloStenciling::LoadTexture()
{
}

void DXHelloStenciling::BuildRenderItem()
{
}

void DXHelloStenciling::BuildFrameResource()
{
}

void DXHelloStenciling::BuildRootSignature()
{
}

void DXHelloStenciling::BuildInputLayout()
{
}

void DXHelloStenciling::CompileShaderFiles()
{
}

void DXHelloStenciling::BuildPSOs()
{
}

void DXHelloStenciling::UpdateCamera(float deltaTime, float totalTime)
{
}

void DXHelloStenciling::UpdateObjectCB(float deltaTime, float totalTime)
{
}

void DXHelloStenciling::UpdateMaterialCB(float delteTime, float totalTime)
{
}

void DXHelloStenciling::UpdatePassCB(float deltaTime, float totalTime)
{
}

void DXHelloStenciling::PopulateCommandList()
{
}

void DXHelloStenciling::DrawRenderItem()
{
}