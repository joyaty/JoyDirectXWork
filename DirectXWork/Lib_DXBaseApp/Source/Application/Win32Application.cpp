/**
 * Win32窗口代码封装源文件
 */

#include "stdafx.h"
#include "Win32Application.h"
#include <windowsx.h>
#include <tchar.h>
#include "imgui_impl_win32.h"
#include "DirectXBaseWork/DirectXBaseWork.h"
#include "DearIMGuiHelper/DearIMGuiBaseHelper.h"
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

Win32Application::Win32Application(HINSTANCE hInstance, DirectXBaseWork* pDirectXWork, DearIMGuiBaseHelper* pDearIMGuiHelper)
	: m_HInstance(hInstance)
	, m_DirectXWork(pDirectXWork)
	, m_DearIMGuiHelper(pDearIMGuiHelper)
{
	m_Width = pDirectXWork->GetWidth();
	m_Height = pDirectXWork->GetHeight();
}

Win32Application::~Win32Application()
{
}

bool Win32Application::InitializeMainWindow()
{
	// 定义一个WNDCLASSEX结构体，描述窗口的一些属性，后续根据描述的窗口属性创建窗口
	WNDCLASSEX wndClsEx = { 0 };
	wndClsEx.cbSize = sizeof(WNDCLASSEX);
	wndClsEx.style = CS_HREDRAW | CS_VREDRAW;		// 指定窗口类的样式，CS_HREDRAW | CS_VREDRAW，这两组标记表示单工作区宽高发生变化时，重绘窗口
	wndClsEx.lpfnWndProc = WindowProc;				// 窗口过程函数指针，接受Windows消息并处理
	wndClsEx.cbClsExtra = 0;						// 通过这两个字段指定应用分配额外的内存空间，目前不需要，指定为0
	wndClsEx.cbWndExtra = 0;						// 通过这两个字段指定应用分配额外的内存空间，目前不需要，指定为0
	wndClsEx.hInstance = m_HInstance;				// 当前应用实例句柄，通过WinMain函数入口传入
	wndClsEx.hIcon = LoadIcon(0, IDI_APPLICATION);	// 指定创建窗口的图标句柄
	wndClsEx.hCursor = LoadCursor(NULL, IDC_ARROW);	// 指定鼠标样式
	wndClsEx.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH); // 指定画刷句柄，以此指定窗口工作区背景颜色
	wndClsEx.lpszMenuName = nullptr;				// 指定窗口菜单，不需要，因此指定为null
	wndClsEx.lpszClassName = _T("JoyDirectX12Work");	// 指定所创建窗口类结构体名称，可以随意填写，后续可以在需要此窗口类结构体时方便的引用
	// 为WNDCLASSEX注册实例，接下来就可根据这个实例创建窗口
	RegisterClassEx(&wndClsEx);

	RECT wndRect = { 0, 0, static_cast<LONG>(m_Width), static_cast<LONG>(m_Height) };
	AdjustWindowRect(&wndRect, WS_OVERLAPPEDWINDOW, FALSE);

	// 创建窗口并且保存窗口Handle
	m_HWND = CreateWindow(
		wndClsEx.lpszClassName,			// 使用前面注册的实例创建窗口
		m_DirectXWork->GetTitle(),		// 窗口标题，显示在标题栏中
		WS_OVERLAPPEDWINDOW,			// 窗口样式，
		CW_USEDEFAULT,					// 左上角位置X
		CW_USEDEFAULT,					// 左上角位置Y
		wndRect.right - wndRect.left,	// 窗口宽度
		wndRect.bottom - wndRect.top,	// 窗口高度
		nullptr,						// 无父窗口
		nullptr,						// 不使用菜单
		wndClsEx.hInstance,				// 此窗口关联的应用句柄
		this);							// 指向用户定义数据的指针，可用作WM_CREATE消息的lpParam参数

	// 初始化DX渲染工作
	m_DirectXWork->Initialize(m_HWND);
	// 初始化DearIMGui
	m_DearIMGuiHelper->InitDearIMGui(m_HWND
		, m_DirectXWork->GetD3D12Device()
		, m_DirectXWork->GetBackBufferCount()
		, m_DirectXWork->GetBackBufferFormat()
		, m_DirectXWork->GetAdapterDesc());
	// 显示窗口
	ShowWindow(m_HWND, SW_SHOW);
	UpdateWindow(m_HWND);
	return true;
}

int Win32Application::Run()
{
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
		else
		{
			m_GameTimer.Tick();
			if (!m_AppPaused)
			{
				m_DearIMGuiHelper->DrawDearIMGuiWindow();
				m_DirectXWork->Update(m_GameTimer.DeltaTime());
				m_DirectXWork->Render();
			}
			else
			{
				Sleep(100);
			}
		}
	}
	// 销毁DearIMGui
	m_DearIMGuiHelper->TerminateIMGui();
	// 销毁DX渲染工作
	m_DirectXWork->Terminate();
	// 返回退出消息
	return static_cast<int>(msg.wParam);
}

void Win32Application::OnResize()
{
	if (m_DirectXWork != nullptr)
	{
		m_DirectXWork->OnResize(m_Width, m_Height);
	}
}

void Win32Application::OnKeyDown(UINT8 keyCode)
{
	if (m_DirectXWork != nullptr)
	{
		m_DirectXWork->OnKeyDown(keyCode);
	}
}

void Win32Application::OnKeyUp(UINT8 keyCode)
{
	if (m_DirectXWork != nullptr)
	{
		m_DirectXWork->OnKeyUp(keyCode);
	}
}

void Win32Application::OnMouseDown(UINT8 keyCode, int x, int y)
{
	if (m_DirectXWork != nullptr)
	{
		m_DirectXWork->OnMouseDown(keyCode, x, y);
	}
}

void Win32Application::OnMouseUp(UINT8 keyCode, int x, int y)
{
	if (m_DirectXWork != nullptr)
	{
		m_DirectXWork->OnMouseUp(keyCode, x, y);
	}
}

void Win32Application::OnMouseMove(UINT8 keyCode, int x, int y)
{
	if (m_DirectXWork != nullptr)
	{
		m_DirectXWork->OnMouseMove(keyCode, x, y);
	}
}

LRESULT CALLBACK Win32Application::WnMsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_AppPaused = true;
			m_GameTimer.Stop();
		}
		else
		{
			m_AppPaused = false;
			m_GameTimer.Start();
		}
		return 0;
	}
	case WM_SIZE:
	{
		m_Width = LOWORD(lParam);
		m_Height = HIWORD(lParam);
		if (wParam == SIZE_MINIMIZED)
		{
			m_AppPaused = true;
			m_AppMinimized = true;
			m_AppMaximized = false;
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			m_AppPaused = false;
			m_AppMinimized = false;
			m_AppMaximized = true;
			OnResize();
		}
		else if (wParam == SIZE_RESTORED)
		{
			if (m_AppMinimized || m_AppMaximized)
			{
				m_AppPaused = false;
				m_AppMinimized = false;
				m_AppMaximized = false;
				OnResize();
			}
			else if (!m_Resizing)
			{
				OnResize();
			}
		}
		return 0;
	}
	case WM_ENTERSIZEMOVE:
	{
		m_AppPaused = true;
		m_Resizing = true;
		m_GameTimer.Stop();
		return 0;
	}
	case WM_EXITSIZEMOVE:
	{
		m_AppPaused = false;
		m_Resizing = false;
		m_GameTimer.Start();
		OnResize();
		return 0;
	}
	case WM_GETMINMAXINFO:
	{
		// 处理此消息避免当前窗口尺寸过小
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	}
	case WM_KEYDOWN:
	{
		// 按下Esc按钮，关闭程序窗口
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hWnd);
		}
		else
		{
			OnKeyDown(static_cast<UINT8>(wParam));
		}
		return 0;
	}
	case WM_KEYUP:
	{
		OnKeyUp(static_cast<UINT8>(wParam));
		return 0;
	}
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	{
		OnMouseDown(static_cast<UINT8>(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	{
		OnMouseUp(static_cast<UINT8>(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		OnMouseMove(static_cast<UINT8>(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	// 其他类型事件交由系统默认窗口过程处理
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK Win32Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// 响应IMGui事件
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}

	switch (message)
	{
	case WM_CREATE:
	{
		// WM_CREATE的lParam为用户自定义数据，本例为Win32Application实例，设置到GWLP_USERDATA中，备后续使用
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
		return 0;
	}
	}
	// 获取Win32App，事件交由Win32App实例处理
	Win32Application* pWin32App = reinterpret_cast<Win32Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (pWin32App != nullptr)
	{
		return pWin32App->WnMsgProc(hWnd, message, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}