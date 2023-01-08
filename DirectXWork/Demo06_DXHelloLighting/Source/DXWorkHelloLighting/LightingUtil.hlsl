/**
 * LightingUtil.hlsl
 * 定义一些光照模型辅助函数
 **/

// 最大光源数 - 16
#define MAX_LIGHTS 16

// HLSL中的光源数据结构
struct Light
{
	// 光强
	float3 Strength;
	// 光强开始衰减的距离，点光源和聚光灯光源使用
	float FalloffStart;
	// 光源方向，平行光源和聚光灯光源使用
	float3 Direction;
	// 光强衰减到0的距离，点光源和聚光灯光源使用
	float FalloffEnd;
	// 光源位置，点光源和聚光源光源使用
	float3 Position;
	// 夹角衰减参数幂，聚光灯光源使用
	float SpotPower;
};

// HLSL中材质数据结构
struct Material
{
	// 漫反射反照率
	float4 DiffuseAlbedo;
	// 菲涅尔效果R0值
	float3 FresnelR0;
	// 光滑度，与粗糙度是一对相反的属性，Shininess = 1 - Roughness
	float Shininess;
};

// 辅助函数 - 计算光强距离衰减因子
// d: 光源到目标位置距离
// falloffStart: 光源强度开始衰减的距离
// falloffEnd: 光源强度衰减到0的距离
// 返回: 光强在当前距离的衰减因子
float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
	return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

// 辅助函数 - 石里克近似法计算菲涅尔反射率
// R0: 材质属性R0
// normal: 法向量
// lightVec: 光向量
// 返回: 反射光百分比
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
	// 光向量与法向量夹角余弦值
	float cosIncidentAngle = saturate(dot(normal, lightVec));
	// 石里克近似法公式，计算反射光百分比
	float f0 = 1.0f - cosIncidentAngle;
	float3 relectionPercent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);
	return relectionPercent;
}

// 辅助函数 - 计算反射到观察者眼中的光量，为漫反射光量与镜面反射光量的总和
// lightStrength: 入射光强，经过朗伯定律修正
// lightVec: 光向量
// normal: 法向量
// toEye: 观察点到观察位置的方向向量
// mat: 材质相关属性
// 返回: 反射光光强
float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
	// 镜面反射光照模型中与粗糙度相关的幂m
	const float m = mat.Shininess * 256.0f;
	// 中间向量，即入射lightVec，反射toEye的微表面法向量
	float3 halfVec = normalize(toEye + lightVec);
	// 粗糙度因子
	float roughnessFactor = (m + 8.0f) * pow(max(dot(normal, halfVec), 0.0f), m) / 8.0f;
	// 菲涅尔因子
	float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, halfVec, lightVec);
	// 镜面反照率
	float3 specularAlbedo = fresnelFactor * roughnessFactor;
	// 尽管是LDR(low dynamic range，低动态范围)渲染，但是镜面反射公式得到的结果仍会超出范围[0,1]，因此按其比例缩小一些
	specularAlbedo = specularAlbedo / (specularAlbedo + 1.0f);
	// return specularAlbedo * lightStrength;
	// 叠加漫反射和镜面反射的反射到观察者光量
	return (mat.DiffuseAlbedo.rgb + specularAlbedo) * lightStrength;
}

// 方向光源实现
// light: 光源信息
// mat: 材质信息
// normal: 法向量
// toEye: 观察点到观察位置的方向向量
// 返回: 反射光强
float3 ComputeDirectionalLight(Light light, Material mat, float3 normal, float3 toEye)
{
	// 光向量，与光源光线方向相反
	float3 lightVec = -light.Direction;
	// Lambert定律修正的光强
	float3 lightStrength = light.Strength * max(dot(lightVec, normal), 0.0f);
	// BlinnPhong光照模型，叠加镜面反射和漫反射
	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

// 点光源实现
// light: 光源信息
// mat: 材质信息
// pos: 表面反射位置
// normal: 法向量
// toEye: 观察点到观察位置的方向向量
// 返回: 反射光强
float3 ComputePointLight(Light light, Material mat, float pos, float3 normal, float3 toEye)
{
	// 未规范化的光向量
	float3 lightVec = light.Position - pos;
	// 光源到表面的距离
	float d = distance(light.Position, pos);
	// 超出衰减距离，直接返回（Shader中应该尽量避免动态分支if，因为会破坏GPU的并行性）
	if (d > light.FalloffEnd)
	{
		return 0.0f;
	}
	// 光向量规范化
	lightVec /= d;
	// Lambert定律修正的光强
	float3 lightStrength = light.Strength * max(dot(lightVec, normal), 0.0f);
	// 距离衰减修正光强
	float att = CalcAttenuation(d, light.FalloffStart, light.FalloffEnd);
	lightStrength *= att;
	// BlinnPhong光照模型，叠加镜面反射和漫反射
	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

// 聚光灯光源实现
// light: 光源信息
// mat: 材质信息
// pos: 表面反射位置
// normal: 法向量
// toEye: 观察点到观察位置的方向向量
// 返回: 反射光强
float3 ComputeSpotLight(Light light, Material mat, float pos, float3 normal, float3 toEye)
{
	// 光向量
	float3 lightVec = light.Position - pos;
	// 光源到物体表面的距离
	float d = distance(light.Position, pos);
	// 超出衰减距离，直接返回（Shader中应该尽量避免动态分支if，因为会破坏GPU的并行性）
	if (d > light.FalloffEnd)
	{
		return 0.0f;
	}
	// 规范化光向量
	lightVec /= d;
	// Lambert定律修正的光强
	float3 lightStrength = light.Strength * max(dot(lightVec, normal), 0.0f);
	// 距离衰减修正光强
	float att = CalcAttenuation(d, light.FalloffStart, light.FalloffEnd);
	lightStrength *= att;
	// 聚光灯光源衰减
	float spotFactor = pow(max(dot(-lightVec, light.Direction), 0.0f), light.SpotPower);
	lightStrength *= spotFactor;
	// BlinnPhong光照模型，叠加镜面反射和漫反射
	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

// 叠加方向光源，点光源，聚光灯光源的多光照场景
// lights: 所有的光源数值，起始是方向光源，接下来是点光源，最后是聚光灯光源。所有光源按此顺序排列
float4 ComputeLight(Light lights[MAX_LIGHTS], Material mat, float3 pos, float3 normal, float3 toEye)
{
	// 叠加所有光源的最终反射光强
	float3 result = 0.0f;
	int i = 0;
	// 处理所有的平行光源
#if (NUM_DIR_LIGHTS > 0)
	for (i = 0; i < NUM_DIR_LIGHTS; ++i)
	{
		result += ComputeDirectionalLight(lights[i], mat, normal, toEye);
	}
#endif
	// 处理所有的点光源
#if (NUM_POINT_LIGHTS > 0)
	for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
	{
		result += ComputePointLight(lights[i], mat, pos, normal, toEye);
	}
#endif
	// 处理所有的聚光灯光源
#if (NUM_SPOT_LIGHTS > 0)
	for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
	{
		result += ComputeSpotLight(lights[i], mat, pos, normal, toEye);
	}
#endif
	// 返回最终的反射光强
	return float4(result, 0.0f);
}