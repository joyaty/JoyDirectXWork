
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
};

// 顶点着色器输入
struct VertexIn
{
	// 本地坐标系顶点位置
	float3 PosL : POSITION;
	// 顶点颜色
	float4 Color : COLOR;
	// 顶点法线
	float3 Normal : NORMAL;
};

// 顶点着色器输出
struct VertexOut
{
	// 裁剪空间的顶点坐标
	float4 PosH : SV_POSITION;
	// 顶点颜色
	float4 Color : COLOR;
	// 顶点法线
	float3 Normal : NORMAL;
};

// 顶点着色器入口
VertexOut VSMain(VertexIn vIn)
{
	VertexOut vOut;
	// 转换为齐次坐标，本地坐标系变换到世界空间坐标系下
	float4 posW = mul(float4(vIn.PosL, 1.0f), gWorldMatrix);
	// 世界空间坐标系变换到裁剪空间
	vOut.PosH = mul(posW, gViewProjMatrix);
	// 原样传递顶点颜色
	vOut.Color = vIn.Color;

	vOut.Normal = vIn.Normal;
	return vOut;
}

// 像素着色器入口
float4 PSMain(VertexOut pIn) : SV_TARGET
{
	return pIn.Color;
}