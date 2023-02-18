/**
 * HelloLighting.hlsl
 * HelloLighting工程使用的主HLSL文件，包含顶点着色器和像素着色器入口
 **/

// 定义方向光源数量
#ifndef NUM_DIR_LIGHTS
	#define NUM_DIR_LIGHTS 1
#endif
// 定义点光源数量
#ifndef NUM_POINT_LIGHTS
	#define NUM_POINT_LIGHTS 0
#endif
// 定义聚光灯光源数量
#ifndef NUM_SPOT_LIGHTS
	#define NUM_SPOT_LIGHTS 0
#endif

// 包含光照模型辅助HLSL文件
#include "LightingUtil.hlsl"

// 物体常量缓冲区
cbuffer cbPerObject : register(b0)
{
	// 本地到世界坐标系的变换矩阵
	float4x4 gWorldMatrix;
};

// 材质常量缓冲区
cbuffer cbPerMaterial : register(b1)
{
	// 材质漫反射反照率
	float4 gDiffuseAlbedo;
	// FresnelR0属性
	float3 gFresnelR0;
	// 粗糙度
	float gRoughness;
}

// 渲染过程常量缓冲区
cbuffer cbPass : register(b2)
{
	// 观察变换矩阵
	float4x4 gViewMatrix;
	// 观察变换矩阵逆矩阵
	float4x4 gInvViewMatrix;
	// 投影变换矩阵
	float4x4 gProjMatrix;
	// 投影变换矩阵逆矩阵
	float4x4 gInvProjMatrix;
	// 观察投影变换矩阵
	float4x4 gViewProjMatrix;
	// 观察投影变换矩阵的逆矩阵
	float4x4 gInvViewProjMatrix;
	// 观察者世界空间位置
	float3 gEyeWorldPos;
	// 渲染目标尺寸宽高
	float2 gRenderTargetSize;
	// 渲染目标尺寸宽高倒数
	float2 gInvRenderTargetSize;
	// 近平面Z
	float gNearZ;
	// 远平面Z
	float gFarZ;
	// 总时间
	float gTotalTime;
	// 帧间隔时间
	float gDeltaTime;
	// 雾的颜色
	float4 gFogColor;
	// 雾开始的距离
	float gFogStart;
	// 雾的范围
	float gFogRange;
	// 占位，与gFrogStart,gFrogRange构成4D向量
	float2 __padding1;
	// 环境光
	float4 gAmbientLight;
	// 所有光源数据
	Light gLights[MAX_LIGHTS];
};

// 顶点着色器输入
struct VertexIn
{
	// 本地坐标系顶点位置
	float3 PosL : POSITION;
	// 顶点颜色
	float4 Color : COLOR;
	// 局部空间顶点法线
	float3 NormalL : NORMAL;
};

// 顶点着色器输出
struct VertexOut
{
	// 裁剪空间的顶点坐标
	float4 PosH : SV_POSITION;
	// 世界空间位置
	float3 PosW : POSITION;
	// 顶点颜色
	float4 Color : COLOR;
	// 世界空间顶点法线
	float3 NormalW : NORMAL;
};

// 顶点着色器入口
VertexOut VSMain(VertexIn vIn)
{
	VertexOut vOut;
	// 转换为齐次坐标，本地坐标系变换到世界空间坐标系下
	float4 posW = mul(float4(vIn.PosL, 1.0f), gWorldMatrix);
	vOut.PosW = posW.xyz;
	// 世界空间坐标系变换到裁剪空间
	vOut.PosH = mul(posW, gViewProjMatrix);
	// 原样传递顶点颜色
	vOut.Color = vIn.Color;
	// 法线转换到世界空间下，这里假设进行的是等比缩放变换
	vOut.NormalW = mul(vIn.NormalL, (float3x3)gWorldMatrix);
	return vOut;
}

// 像素着色器入口
float4 PSMain(VertexOut pIn) : SV_TARGET
{
	// 法线变换可能导致非规范放，重新规范化法线
	pIn.NormalW = normalize(pIn.NormalW);
	// 光线反射表面点到观察点的方向向量
	float3 toEye = normalize(gEyeWorldPos - pIn.PosW);

	// 构建材质数据
	Material mat;
	mat.DiffuseAlbedo = gDiffuseAlbedo;
	mat.FresnelR0 = gFresnelR0;
	mat.Shininess = 1.0f - gRoughness;
	// 直接光照反射光
	float4 directLight = ComputeLight(gLights, mat, pIn.PosW, pIn.NormalW, toEye);
	// 间接光照
	float4 ambient = gAmbientLight * gDiffuseAlbedo;
	// 表面光 = 间接反射光 + 直接反射光
	float4 litColor = ambient + directLight;
	// 从漫反射材质中获取alpha值是常见手段
	litColor.a = gDiffuseAlbedo.a;
	return litColor;
}