/**
 * IMGuiHelloTexture.cpp
 * HelloTexture示例程序调试面板代码文件
 **/

#include "stdafx.h"
#include "IMGuiHelloTexture.h"
#include "DXHelloTexture.h"
#include <imgui.h>

IMGuiHelloTexture* IMGuiHelloTexture::GetInstance()
{
	static IMGuiHelloTexture s_IMGuiHelloTexture{};
	return &s_IMGuiHelloTexture;
}

void IMGuiHelloTexture::OnDrawWindow()
{
	ImGui::Begin("Hello Lighting");
	DrawAdapterInfo();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	// TODO 绘制HelloTexture的调试面板

	ImGui::End();
}