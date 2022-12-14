/*
 * D3D12HelloWindow.cpp
 * Demo01 - DirectX绘制窗口颜色
 */

#include "stdafx.h"
#include "D3D12HelloWindow.h"
#include "IMGuiHelloWindow.h"
#include "DirectXBaseWork/DXWorkHelper.h"
#include "Application/Win32Application.h"
#include <array>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include "DirectXBaseWork/D3D12Util.h"

using namespace DirectX;

D3D12HelloWindow::D3D12HelloWindow(std::wstring name, UINT width, UINT height)
	: DirectXBaseWork(name, width, height)
{
}

bool D3D12HelloWindow::OnInit()
{
	return true;
}

void D3D12HelloWindow::OnUpdate(float deltaTime, float totalTime)
{
}

void D3D12HelloWindow::OnRender()
{
	// 记录所有的渲染指令
	PopulateCommandList();
	// 提交指令列表
	ID3D12CommandList* ppCommandList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
	// 交换缓冲区
	ThrowIfFailed(m_SwapChain->Present(1, 0));
	// 刷新命令队列
	FlushCommandQueue();
}

void D3D12HelloWindow::OnDestroy()
{
}

void D3D12HelloWindow::PopulateCommandList()
{
	// Command list allocators can only be reset when the associated command lists have finished execution on th GPU.
	// apps should use fence to determine GPU excution progress.
	ThrowIfFailed(m_CommandAllocator->Reset());
	// However, when ExecuteCommandList() is called on a particular command list, that command list can the be reset at any time and must be before re-recording.
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));
	// 当前后台缓冲区切换到渲染目标状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_CurrentBackBufferIndex].Get()
		, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentBackBufferIndex, m_RTVDescriptorSize);
	// Record commands.
	ImVec4 backgroudColor = IMGuiHelloWindow::GetInstance()->GetBackColor();
	const float clearColor[] = { backgroudColor.x, backgroudColor.y, backgroudColor.z, backgroudColor.w };
	m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	// 提交DearIMGui的渲染命令
	if (IMGuiHelloWindow::GetInstance() != nullptr)
	{
		IMGuiHelloWindow::GetInstance()->PopulateDearIMGuiCommand(m_CommandList.Get());
	}
	// 当前后台缓冲区切换到显示状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_CurrentBackBufferIndex].Get()
		, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_CommandList->Close());
}