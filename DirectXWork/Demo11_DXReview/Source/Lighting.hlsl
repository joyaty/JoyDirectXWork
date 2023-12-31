/// 光照计算 - BlinnPhong光照模型
/// 实现三种光源效果，方向光源、点光源、聚光源

// 最大光源数的定义
#define MAX_LIGHTS 16

// 材质数据结构体
struct Material
{
    // 材质的漫反射照率
    float4 diffuseAlbedo;
    // 菲涅尔效应属性R0
    float3 fresnelR0;
    // 粗糙度
    float roughness;
};

// 光源数据结构体
struct Light
{
    // 光强
    float3 lightStrength;
    // 光源衰减开始距离，点光源和聚光源生效
    float falloffStart;
    // 光照方向，平行光源和聚光源生效
    float3 direction;
    // 光源衰减到0距离，点光源和聚光源生效
    float falloffEnd;
    // 光源的位置，点光源和聚光源生效
    float3 position;
    // 聚光源的夹角衰减参数幂
    float spotPower;
};

// 距离衰减因子
// d - 光源到受光位置的距离
// falloffStart - 光强衰减开始距离
// falloffEnd - 光强衰减到0距离
// 返回，当前d距离的衰减因子
float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

// 石里克近似计算菲涅尔效应因子
// R0 - 材质的菲涅尔效应属性R0
// normal - 法向量，需求为单位向量
// lightVec - 光向量，需求为单位向量
// 返回：菲涅尔效应后反射光的百分比
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
    float cosIncidentAngle = saturate(dot(normal, lightVec));
    float f0 = 1 - cosIncidentAngle;
    float3 relectionPercent = R0 + (1.0f - R0) * pow(f0, 5);
    return relectionPercent;
}

// BlinnPhone光照模型，光照效果 = 漫反射 + 镜面反射(菲涅尔效应+粗糙度影响)
// lightStrength - 入射光强，经过朗伯定律修正
// lightVec - 光向量，单位向量
// normal - 法向量，单位向量
// toEye - 被观察点到观察者位置的方向向量
// mat - 材质属性集合
float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
    // 菲涅尔效应因子
    float3 fresnelFactor = SchlickFresnel(mat.fresnelR0, normal, lightVec);
    // 粗糙度范围[0,1],m受粗糙度控制，m越小，越粗糙
    const float m = (1.0f - mat.roughness) * 256.0f;
    // 计算中间向量
    float3 halfVec = normalize(toEye + lightVec);
    // 粗糙度因子
    float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal), 0), m) / 8.0f;
    // 镜面反射照率，菲涅尔效应与粗糙度乘算
    float specularAlbedo = fresnelFactor * roughnessFactor;
    // 尽管是LDR(low dynamic range，低动态范围)渲染，但是镜面反射公式得到的结果仍会超出范围[0,1]，因此按其比例缩小一些
    specularAlbedo = specularAlbedo / (specularAlbedo + 1.0f);
    // 漫反射叠加镜面反射
    return (mat.diffuseAlbedo.rgb + specularAlbedo) * lightStrength;
}

// 方向光源
// light - 光源信息
// mat - 光照的材质信息
// normal - 表面法线信息
// toEye - 被观察位置到观察者的方向向量，单位向量
// 返回 - 反射光强
float3 ComputeDirectionalLight(Light light, Material mat, float3 normal, float3 toEye)
{
    // 光向量与入射光方向相反
    float3 lightVec = -light.direction;
    // 入射光强受Lambert定律修正
    float3 lightStrength = max(dot(lightVec, normal), 0.0f) * light.lightStrength;
    // 使用BlinnPhong光照模型计算反射光
    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

// 点光源光照
// light - 光源信息
// mat - 光照的材质信息
// pos - 光照影响的位置
// normal - 表面法线信息
// toEye - 被观察位置到观察者的方向向量，单位向量
// 返回 - 反射光强
float3 ComputePointLight(Light light, Material mat, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = light.position - pos;
    float d = distance(light.position, pos);
    // 距离超出光照衰减范围，直接返回。(P.S. 实际应用中应尽量避免使用动态分支语句if。因为会打破GPU的并行)
    if (d > light.falloffEnd)
    {
        return 0.0f;
    }
    // 规范化光向量
    lightVec /= d;
    // 入射光强受Lambert定律修正
    float3 lightStrength = max(dot(lightVec, normal), 0.0f) * light.lightStrength;
    // 点光源还会受到距离衰减
    float disAttenuation = CalcAttenuation(d, light.falloffStart, light.falloffEnd);
    lightStrength *= disAttenuation;
    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

// 聚光源
// light - 光源信息
// mat - 光照的材质信息
// pos - 光照影响的位置
// normal - 表面法线信息
// toEye - 被观察位置到观察者的方向向量，单位向量
// 返回 - 反射光强
float3 ComputeSpotLight(Light light, Material mat, float3 pos, float3 normal, float3 toEye)
{
    // 未规范化光向量
    float lightVec = light.position - pos;
    // 光源到受光位置的距离
    float d = distance(light.position, pos);
    if(d > light.falloffEnd)
    {
        return 0.0f;
    }
    // 规范化光向量
    lightVec /= d;
    // Lambert定律修正的光强
    float3 lightStrength = light.lightStrength * max(dot(lightVec, normal), 0.0f);
	// 距离衰减修正光强
    float disFactor = CalcAttenuation(d, light.falloffStart, light.falloffEnd);
    lightStrength *= disFactor;
    // 聚光灯有额外的光照角度衰减
    float spotFactor = pow(max(dot(-lightVec, light.direction), 0.0f), light.spotPower);
    lightStrength *= spotFactor;
    
    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

// 计算所有的光源
// lights - 所有的光源信息数组
// mat - 受光表面的材质信息
// pos - 光照影响的位置
// normal - 表面法线
// toEye - 被观察位置和观察者的方向向量
// 返回 - 反射光强
float4 ComputeLights(Light lights[MAX_LIGHTS], Material mat, float3 pos, float3 normal, float3 toEye)
{
    // 最终所有的光照效果
    float3 result = 0.0f;
    int i = 0;
#if(NUM_DIR_LIGHTS > 0)
    // 叠加所有的方向光源
    for (i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        result += ComputeDirectionalLight(lights[i], mat, normal, toEye);
    }
#endif
    
#if(NUM_POINT_LIGHTS > 0)
    // 叠加所有的点光源
    for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
    {
        result += ComputePointLight(lights[i], mat, pos, normal, toEye);
    }
#endif
    
#if(NUM_SPOT_LIGHTS > 0)
    // 叠加所有的聚光灯
    for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        result += ComputeSpotLight(lights[i], mat, pos, normal, toEye);
    }
#endif
    return float4(result, 0.0f);
}