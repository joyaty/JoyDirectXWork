/**
 * HelloWindow范例的DearIMGui面板CPP文件
 */

#include "stdafx.h"
#include "IMGuiHelloWindow.h"

IMGuiHelloWindow* IMGuiHelloWindow::s_IMGUIWindowInstance = nullptr;

void IMGuiHelloWindow::OnDrawWindow()
{
    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

    ImGui::SliderFloat("float", &m_FloatValue, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&m_ClearColor); // Edit 3 floats representing a color

    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        ++m_Counter;
    ImGui::SameLine();
    ImGui::Text("counter = %d", m_Counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}