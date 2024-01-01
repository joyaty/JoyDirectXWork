/*
 * @Description: HelloStenciling示例的Shader主入口
 * @Date: 2023-02-22 00:58:22
 */

// 定义方向光源数量
#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 2
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
    // 纹理坐标变换矩阵
    float4x4 gTexMatrix;
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
    // uv变换矩阵
    float4x4 gMatMatrix;
};

// 渲染过程常量缓冲区
cbuffer cbPerPass : register(b2)
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
    // 雾的颜色
    float4 gFogColor;
    // 雾开始的距离
    float gFogStart;
    // 雾的范围
    float gFogRange;
    // 占位，与gFogStart,gFogRange构成4D向量
    float2 __padding1;
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
SamplerState gLinearClampSampler : register(s3);
// 静态采样器 各向异性过滤 + Wrap寻址
SamplerState gAnisotropicWrapSampler : register(s4);
// 静态采样器 各向异性过滤 + Clamp寻址
SamplerState gAnisotropicClampSampler : register(s5);

// 漫反射纹理贴图
Texture2D gDiffuseMap : register(t0);

// 顶点着色器输入
struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 TexCoord : TEXCOORD;
};

// 顶点着色器输出
struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 TexCoord : TEXCOORD;
};

VertexOut VSMain(VertexIn vIn)
{
    VertexOut vOut;
    // 变换到世界空间
    float4 posW = mul(float4(vIn.PosL, 1.0f), gWorldMatrix);
    vOut.PosW = posW.xyz;
    // 变换到裁剪空间
    vOut.PosH = mul(posW, gViewProjMatrix);
    // 法线变换到世界空间下，这里假设进行等比缩放，非等比缩放需要乘世界变换矩阵的逆转置矩阵
    vOut.NormalW = mul(vIn.NormalL, (float3x3) gWorldMatrix);
    // 纹理坐标经过texMatrix变换和matMatrix变换
    // float4 texCoord = mul(float4(vIn.TexCoord, 1.0f, 1.0f), gTexMatrix);
    // texCoord = mul(texCoord, gMatMatrix);
    vOut.TexCoord = vIn.TexCoord; // texCoord.xy;
    return vOut;
}

float4 PSMain(VertexOut pIn) : SV_TARGET
{
    // 采样当前纹理坐标位置的纹素
    float4 diffuseAlbedo = gDiffuseMap.Sample(gLinearWrapSampler, pIn.TexCoord) * gDiffuseAlbedo;
#if (ALPHA_TEST == 1)
    // 开启ALPHA_TEST宏，检查是否是全透明像素，是全透明像素则丢弃
    clip(diffuseAlbedo.a - 0.1f);
#endif
    // 法线变换可能导致规范化丢失，重新规范化
    float3 normal = normalize(pIn.NormalW);
    // 观察向量
    float distToEye = distance(gEyePos, pIn.PosW);
    float3 toEye = (gEyePos - pIn.PosW) / distToEye;
    // 构建材质
    Material mat;
    mat.DiffuseAlbedo = diffuseAlbedo; // 叠加材质的反照率和漫反射贴图的反照率
    mat.FresnelR0 = gFresnelR0;
    mat.Shininess = 1.0f - gRoughness;
    // 计算直接光反射光强
    float4 directDiffuse = ComputeLight(gLights, mat, normal, toEye, pIn.PosW);
    // 计算环境光反射
    float4 ambientDiffuse = gAmbientLight * diffuseAlbedo;
    // 表面光= 直接光 + 间接光
    float4 litColor = directDiffuse + ambientDiffuse;
#if (FOG_ENABLE == 1)
    float fogFactor = saturate((distToEye - gFogStart) / gFogRange);
    litColor = lerp(litColor, gFogColor, fogFactor);
#endif
    litColor.a = diffuseAlbedo.a;
    return litColor;
}