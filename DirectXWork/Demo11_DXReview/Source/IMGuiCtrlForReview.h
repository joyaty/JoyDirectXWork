
#pragma once

class DXSampleForReview;

/// <summary>
/// DearIMGui调试面板
/// </summary>
class IMGuiCtrlForReview
{
public:
	bool InitializeIMGui(HWND hWndInstance);
	void TerminateIMGui();

	void DrawIMGUI();
};