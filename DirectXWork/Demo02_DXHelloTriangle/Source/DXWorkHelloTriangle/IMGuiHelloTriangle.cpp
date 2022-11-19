/*
 * IMGuiHelloTriangle.cpp
 * DearIMGui - 绘制三角形实例程序参数调试面板调试面板
 */

#include "stdafx.h"
#include "DXWorkHelloTriangle/IMGuiHelloTriangle.h"
#include "imgui.h"

IMGuiHelloTriangle* IMGuiHelloTriangle::s_Instance = nullptr;

void IMGuiHelloTriangle::OnDrawWindow()
{
	ImGui::Begin("Hello HelloTriangle!");
	DrawAdapterInfo();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}