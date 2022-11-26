/*
 * HelloCube实例程序使用的Shader
 */

// 常量缓冲区数据结构
cbuffer cbPerObject : register(b0)
{
	// 世界到裁剪空间变换矩阵
	float4x4 gWorldViewProj;
	// 时间
	float gTime;
};

// 顶点着色器输入数据结构
struct VertexIn
{
	float3 Position : POSITION;
	float4 Color : COLOR;
};

// 顶点着色器输出数据结构
struct VertexOut
{
	float4 PosH : SV_POSITION;
	float4 Color : COLOR;
};

// 顶点着色器入口
VertexOut VSMain(VertexIn vIn)
{
	VertexOut vOut;
	vIn.Position.xy += 0.5f * sin(vIn.Position.x) * sin(3.0f * gTime);
	vIn.Position.z *= 0.6f + 0.4f * sin(2.0f * gTime);
	// 顶点坐标转换到裁剪空间下，变为齐次坐标
	vOut.PosH = mul(float4(vIn.Position, 1.0f), gWorldViewProj);
	// 直接将顶点颜色输出到像素着色器
	vOut.Color = vIn.Color;

	return vOut;
}

// 像素着色器入口
float4 PSMain(VertexOut pIn) : SV_TARGET
{
	return pIn.Color;
}

