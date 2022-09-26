/**
 * HelloWindow范例的DearIMGui面板CPP文件
 */

#include "stdafx.h"
#include "IMGuiHelloWindow.h"

IMGuiHelloWindow* IMGuiHelloWindow::s_IMGUIWindowInstance = nullptr;

void IMGuiHelloWindow::OnDrawWindow()
{
    ImGui::Begin("Hello Window!");                                      // Create a window called "Hello, world!" and append into it.
    DrawAdapterInfo();
    ImGui::ColorEdit3("clear color", (float*)&m_ClearColor);            // Edit 3 floats representing a color
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}