/**
 * Win32窗口代码封装源文件
 */

#include "stdafx.h"
#include "Win32Application.h"
#include <tchar.h>
#include "DXWork/DXBaseWork.h"
#include "DearIMGuiHelper/DearIMGuiBaseHelper.h"
#include "imgui_impl_win32.h"
#include "GameTimer.h"

/// <summary>
/// 外部声明IMGui事件响应处理
/// </summary>
/// <param name="hwnd"></param>
/// <param name="msg"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
/// <returns></returns>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND Win32Application::m_HWND = nullptr;
GameTimer Win32Application::m_GameTimer;

int Win32Application::Run(DXBaseWork* pDXWork, HINSTANCE hInstance, int nCmdShow, DearIMGuiBaseHelper* pDearIMGuiHelper)
{
	// 定义一个WNDCLASSEX结构体，描述窗口的一些属性，后续根据描述的窗口属性创建窗口
	WNDCLASSEX wndClsEx = { 0 };
	wndClsEx.cbSize = sizeof(WNDCLASSEX);
	wndClsEx.style = CS_HREDRAW | CS_VREDRAW;		// 指定窗口类的样式，CS_HREDRAW | CS_VREDRAW，这两组标记表示单工作区宽高发生变化时，重绘窗口
	wndClsEx.lpfnWndProc = WindowProc;				// 窗口过程函数指针，接受Windows消息并处理
	wndClsEx.cbClsExtra = 0;						// 通过这两个字段指定应用分配额外的内存空间，目前不需要，指定为0
	wndClsEx.cbWndExtra = 0;						// 通过这两个字段指定应用分配额外的内存空间，目前不需要，指定为0
	wndClsEx.hInstance = hInstance;					// 当前应用实例句柄，通过WinMain函数入口传入
	wndClsEx.hIcon = LoadIcon(0, IDI_APPLICATION);	// 指定创建窗口的图标句柄
	wndClsEx.hCursor = LoadCursor(NULL, IDC_ARROW);	// 指定鼠标样式
	wndClsEx.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH); // 指定画刷句柄，以此指定窗口工作区背景颜色
	wndClsEx.lpszMenuName = nullptr;				// 指定窗口菜单，不需要，因此指定为null
	wndClsEx.lpszClassName = _T("JoyDirectX12Work");	// 指定所创建窗口类结构体名称，可以随意填写，后续可以在需要此窗口类结构体时方便的引用
	// 为WNDCLASSEX注册实例，接下来就可根据这个实例创建窗口
	RegisterClassEx(&wndClsEx);

	RECT wndRect = { 0, 0, 1280, 720 };
	AdjustWindowRect(&wndRect, WS_OVERLAPPEDWINDOW, FALSE);

	// 创建窗口并且保存窗口Handle
	m_HWND = CreateWindow(
		wndClsEx.lpszClassName,			// 使用前面注册的实例创建窗口
		pDXWork->GetTitle(),			// 窗口标题，显示在标题栏中
		WS_OVERLAPPEDWINDOW,			// 窗口样式，
		CW_USEDEFAULT,					// 左上角位置X
		CW_USEDEFAULT,					// 左上角位置Y
		wndRect.right - wndRect.left,	// 窗口宽度
		wndRect.bottom - wndRect.top,	// 窗口高度
		nullptr,						// 无父窗口
		nullptr,						// 不使用菜单
		wndClsEx.hInstance,				// 此窗口关联的应用句柄
		pDXWork);						// 指向用户定义数据的指针，可用作WM_CREATE消息的lpParam参数	

	// 初始化DX渲染工作
	pDXWork->OnInit();
	// 初始化DearIMGui
	pDearIMGuiHelper->InitDearIMGui(m_HWND, pDXWork->GetD3D12Device(), pDXWork->GetFrameCount(), DXGI_FORMAT_R8G8B8A8_UNORM, pDXWork->GetD3D12DescriptorHeap());
	// 显示窗口
	ShowWindow(m_HWND, nCmdShow);
	UpdateWindow(m_HWND);
	// 初始化计时器
	m_GameTimer.Reset();
	// 窗口主循环
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		// 处理消息队列
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// TODO 执行程序逻辑
		m_GameTimer.Tick();
	}
	// 销毁DearIMGui
	pDearIMGuiHelper->TerminateIMGui();
	// 销毁DX渲染工作
	pDXWork->OnDestroy();
	// 返回退出消息
	return static_cast<char>(msg.wParam);
}

LRESULT CALLBACK Win32Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// 响应IMGui事件
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
	 	return true;
	}
	// 响应IMGui外事件
	DXBaseWork* pDXWork = reinterpret_cast<DXBaseWork*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	switch (message)
	{
	case WM_CREATE:
		{
		//
			LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
		}
		return 0;
	case WM_ACTIVATE:
	{
		if (LOWORD(lParam) == WA_INACTIVE)
		{
			m_GameTimer.Stop();
		}
		else
		{
			m_GameTimer.Start();
		}
	}
	return 0;
	case WM_SIZE:
		// TODO something
		return 0;
	case WM_KEYDOWN:
		// 按下Esc按钮，关闭程序窗口
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hWnd);
		}
		else if (pDXWork != nullptr)
		{
			pDXWork->OnKeyDown(static_cast<UINT8>(wParam));
		}
		return 0;
	case WM_KEYUP:
		if (pDXWork != nullptr)
		{
			pDXWork->OnKeyUp(static_cast<UINT8>(wParam));
		}
		return 0;
	case WM_PAINT:
		if (pDXWork != nullptr)
		{
			pDXWork->OnUpdate();
			pDXWork->OnRender();
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	// 其他类型事件交由系统默认窗口过程处理
	return DefWindowProc(hWnd, message, wParam, lParam);
}