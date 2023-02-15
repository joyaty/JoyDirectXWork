/*
 * @Description: WaveWithBlend示例的Shader主入口
 * @Date: 2023-02-15 09:49:22
 */

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

#include "Lighting.hlsl"

// 物体常量缓冲区
cbuffer cbPerObject : register(b0)
{
    // 世界变换矩阵
    float4x4 gWorldMatrix;
};

// 材质常量缓冲区
cbuffer cbPerMaterial : register(b1)
{
    // 漫反射反照率
    float4 gDiffuseAlbedo;
    // FresnelR0属性
    float3 gFresnelR0;
    // 粗糙度
    float gRoughness;
};

// 渲染过程常量缓冲区
cbuffer cbPerPass: register(b2)
{
    // 观察矩阵
    float4x4 gViewMatrix;
    // 观察矩阵逆矩阵
    float4x4 gInvViewMatrix;
    // 投影矩阵
    float4x4 gProjMatrix;
    // 投影矩阵逆矩阵
    float4x4 gInvProjMatirx;
    // 观察投影矩阵
    float4x4 gViewProjMatrix;
    // 观察投影变换矩阵逆矩阵
    float4x4 gInvViewProjMatrix;
    // 观察者世界空间位置
    float3 gEyePos;
    // 渲染目标尺寸宽高
    float2 gRenderTargetSize;
    // 渲染目标尺寸宽高倒数
    float2 gInvRenderTargetSize;
    // 近平面
    float gNearZ;
    // 远平面
    float gFarZ;
    // 总时间
    float gTotalTime;
    // 帧间隔时间
    float gDeltaTime;
    // 环境光
    float4 gAmbientLight;
    // 所有光源数据
    Light gLights[MAX_LIGHTS];
};

// 静态采样器 点过滤 + Wrap寻址
SamplerState gPointWrapSampler : register(s0);
// 静态采样器 点过滤 + Clamp寻址
SamplerState gPointClampSampler : register(s1);
// 静态采样器 线性过滤 + Wrap寻址
SamplerState gLinearWrapSampler : register(s2);
// 静态采样器 线性过滤 + Clamp寻址
SamplerState gLinearClampSamper : register(s3);
// 静态采样器 各向异性过滤 + Wrap寻址
SamplerState gAnisotropicWrapSampler : register(s4);
// 静态采样器 各向异性过滤 + Clamp寻址
SamplerState gAnisotropicWrapSampler : register(s5);

// 漫反射纹理贴图
Texture2D gDiffuseMap : register(t0);

// 顶点着色器输入
struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 TexCoord : TEXCOORD;
}

// 顶点着色器输出
struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 TexCoord : TEXCOORD;
}

VertexOut VSMain(Vertexin vIn)
{
    VertexOut vOut;
    // TODO 顶点着色器实现
    return vOut;
}

float4 PSMain(VertexOut pIn) : SV_TARGET
{
    // TODO 像素着色器实现
    return float4(1, 1, 1, 1);
}