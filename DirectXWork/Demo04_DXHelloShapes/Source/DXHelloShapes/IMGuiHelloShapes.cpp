/*
 * IMGuiHelloShapes.cpp
 * DearIMGui - 绘制多个几何体实例程序参数调试面板调试面板
 */

#include "stdafx.h"
#include "IMGuiHelloShapes.h"
#include "imgui.h"

IMGuiHelloShapes* IMGuiHelloShapes::s_Instance = nullptr;

void IMGuiHelloShapes::OnDrawWindow()
{
	ImGui::Begin("Hello Shapes!");
	DrawAdapterInfo();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	// TODO 绘制示例项目参数调试面板

	ImGui::End();
}