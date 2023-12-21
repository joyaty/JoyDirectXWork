
// 物体层级常量缓冲区
cbuffer PerObjectConstants : register(b0)
{
	// 本地到世界变换矩阵
	float4x4 gWorldMatrix;
};

// 渲染过程常量缓冲区
cbuffer PerPassConstants : register(b1)
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
};

// 顶点缓冲区输入
struct VertexIn
{
	float3 PosL : POSITION;
	float4 Color : COLOR;
	// float3 Normal : NORMAL;
	// float2 TexCoord : TEXCOORD;
};

// 顶点缓冲区输出
struct VertexOut
{
	float4 PosH : SV_POSITION;
	float4 PosW : POSITION;
	float4 Color : COLOR;
	// float3 NormalW : NORMAL;
	// float3 TexCoord : TEXCOORD;
};

VertexOut VSMain(VertexIn vIn)
{
	VertexOut vOut;
	vOut.PosW = mul(float4(vIn.PosL, 1.0f), gWorldMatrix);
    vOut.PosH = mul(vOut.PosW, gViewProjMatrix);
	vOut.Color = vIn.Color;
	return vOut;
}

float4 PSMain(VertexOut pIn) : SV_Target
{
    return pIn.Color;
}