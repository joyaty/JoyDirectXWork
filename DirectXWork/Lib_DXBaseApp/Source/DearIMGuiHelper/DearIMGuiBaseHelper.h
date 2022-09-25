/**
 * DearIMGui对接类头文件
 */

#pragma once

class DearIMGuiBaseHelper
{
public:
	/// <summary>
	/// 初始化DearIMGui
	/// </summary>
	/// <param name="hwnd">Windows窗口</param>
	/// <param name="pD3D12Device">DX显示适配设备</param>
	/// <param name="nRTVCount">渲染目标缓冲区数量</param>
	/// <param name="rtvFormat">RenderTargetView格式</param>
	/// <returns></returns>
	bool InitDearIMGui(HWND hwnd, ID3D12Device* pD3D12Device, int nRTVCount, DXGI_FORMAT rtvFormat);
	
	/// <summary>
	/// 清理DearIMGui
	/// </summary>
	void TerminateIMGui();

	/// <summary>
	/// 绘制DearIMGui窗口
	/// </summary>
	void DrawDearIMGuiWindow();

	/// <summary>
	/// 提交DearIMGui渲染指令
	/// </summary>
	/// <param name="pCommandList">DirectX渲染队列</param>
	void PopulateDearIMGuiCommand(ID3D12GraphicsCommandList* pCommandList);

protected:
	/// <summary>
	/// 子类绘制自定义窗口
	/// </summary>
	virtual void OnDrawWindow() = 0;

private:
	/// <summary>
	/// DearIMGui专用的SRV描述符堆
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SRVDescriptorHeap{};
};