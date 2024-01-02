
// 定义 方向光/点光/聚光 光源数量
#ifndef NUM_DIR_LIGHTS
	#define NUM_DIR_LIGHTS 1
#endif
#ifndef NUM_POINT_LIGHTS
	#define NUM_POINT_LIGHTS 0
#endif
#ifndef NUM_SPOT_LIGHTS
	#define NUM_SPOT_LIGHTS 0
#endif

#include "Lighting.hlsl"

// 物体层级常量缓冲区
cbuffer PerObjectConstants : register(b0)
{
	// 本地到世界变换矩阵
	float4x4 gWorldMatrix;
	// 纹理UV变换矩阵
    float4x4 gTexMatrix;
};

// 材质层级常量缓冲区
cbuffer PerMaterialConstants : register(b1)
{
	// 满反射照率
    float4 gDiffuseAlbedo;
	// 菲涅尔R0属性
    float3 gFresnelR0;
	// 粗糙度
    float gRoughness;
	// 材质UV动画
    float4x4 gUVMatrix;
}

// 渲染过程常量缓冲区
cbuffer PerPassConstants : register(b2)
{
	// 观察矩阵
	float4x4 gViewMatrix;
	// 观察矩阵逆矩阵
	float4x4 gInvViewMatrix;
	// 投影矩阵
	float4x4 gProjMatrix;
	// 投影矩阵逆矩阵
	float4x4 gInvProjMatrix;
	// 观察投影矩阵
	float4x4 gViewProjMatrix;
	// 观察投影矩阵逆矩阵
	float4x4 gInvViewProjMatrix;
	// 观察者位置
	float3 gEyePos;
	// 占位0
	float __padding0;
	// 渲染目标宽高
	float2 gRenderTargetSize;
	// 渲染对象的宽高倒数
	float2 gInvRenderTargetSize;
	// 近平面
	float gNearZ;
	// 远平面
	float gFarZ;
	// 帧间隔时间
	float gDeltaTime;
	// 总时间
	float gTotalTime;
	// 雾颜色
    float4 gFogColor;
	// 雾开始位置
    float gFogStart;
	// 雾不可见位置
    float gFogEnd;
	// 占位1
    float2 __padding1;
	// 环境光
    float4 gAmbientLight;
	// 全部的直接光信息
    Light gLights[MAX_LIGHTS];
};

// 纹理资源 - 漫反射照率贴图
Texture2D gDiffuseMap : register(t0);

// 采样器动分配的采样器，三线性过滤 + 循环寻址
SamplerState gLinearWrapSampler : register(s0);

// 静态采样器 - 点过滤 + 循环寻址
SamplerState gPointWrapStaticSampler : register(s1);
// 静态采样器 - 点过滤 + CLAMP寻址
SamplerState gPointClampStaticSampler : register(s2);
// 静态采样器 - 三线性过滤 + 循环寻址
SamplerState gLinearWrapStaticSampler : register(s3);
// 静态采样器 - 三线性过滤 + CLAMP寻址
SamplerState gLinearClampStaticSampler : register(s4);
// 静态采样器 - 各向异性过滤 + 循环寻址
SamplerState gAnisotropicWrapStaticSampler : register(s5);
// 静态采样器 - 各向异性过滤 + CLAMP寻址
SamplerState gAnisotropicClampStaticSampler : register(s6);

// 顶点缓冲区输入
struct VertexIn
{
    float3 PosL : POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float3 TangentU : TANGENT;
	float2 TexCoord : TEXCOORD;
};

// 顶点缓冲区输出
struct VertexOut
{
	float4 PosH : SV_POSITION;
	float4 PosW : POSITION;
	float4 Color : COLOR;
	float3 NormalW : NORMAL;
	float2 TexCoord : TEXCOORD;
};

VertexOut VSMain(VertexIn vIn)
{
	VertexOut vOut;
	// 世界坐标
	vOut.PosW = mul(float4(vIn.PosL, 1.0f), gWorldMatrix);
	// 投影空间齐次坐标
    vOut.PosH = mul(vOut.PosW, gViewProjMatrix);
	// 必须使用等比缩放，否则法线变换会变形，不保证与表面正交
    vOut.NormalW = mul(vIn.Normal, (float3x3)gWorldMatrix);
	// 原样传递顶点色和纹理坐标
	vOut.Color = vIn.Color;
	// UV动画变换，包含物体UV变换和材质UV变换
    float4 uv = mul(float4(vIn.TexCoord, 1.0f, 1.0f), gTexMatrix);
    uv = mul(uv, gUVMatrix);
    vOut.TexCoord = uv.xy;
	return vOut;
}

float4 PSMain(VertexOut pIn) : SV_Target
{
	// 法线变换，可能导致非规范化，这里重新规范化法线，确保单位向量
    float3 normal = normalize(pIn.NormalW);
	// 当前位置到观察者的方向向量，规范化
    float3 pos = (float3) pIn.PosW;
    float3 toEye = normalize(gEyePos - pos);
    float distToEye = distance(gEyePos, pos);
	// 从材质常量缓冲区构建当前渲染的材质数据
    Material mat;
    mat.diffuseAlbedo = gDiffuseMap.Sample(gPointWrapStaticSampler, pIn.TexCoord) * gDiffuseAlbedo;
    mat.fresnelR0 = gFresnelR0;
    mat.roughness = gRoughness;
	// 直接光
    float4 directLight = ComputeLights(gLights, mat, pos, normal, toEye);
	// 环境光
    float4 ambient = gAmbientLight * mat.diffuseAlbedo;
	// 最终光 = 直接光 + 间接光
    float4 litColor = directLight + ambient;
#if (ENABLE_FOG == 1)
	float fogFactor = saturate((distToEye - gFogStart) / (gFogEnd - gFogStart));
	litColor = lerp(litColor, gFogColor, fogFactor);
#endif	
	// 取漫反射材质的alpha
    litColor.a = gDiffuseAlbedo.a;
    return litColor;
}