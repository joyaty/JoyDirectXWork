/*
 * @Description: BlinnPhong光照模型相关的辅助函数
 * @Date: 2023-02-15 09:49:22
 */

// 最大支持16个光源，包含方向光源 + 点光源 + 聚光源
#define MAX_LIGHTS 16

// 光源数据结构，数据结构成员顺序有意为之，HLSL会将元素打包为4D向量，因此这里有意按(float3 + float)顺序定义数据元素来构成一个4D向量
struct Light
{
    // 光照强度，平行光/点光/聚光通用
    float3 Strength;
    // 点光/聚光衰减距离
    float FalloffStart;
    // 平行光/聚光的光源方向
    float3 Direction;
    // 点光/聚光衰减到0的距离
    float FalloffEnd;
    // 点光/聚光的位置
    float3 Position;
    // 夹角衰减参数幂，聚光使用
    float SpotPower;
};

// 材质数据结构
struct Material
{
    // 材质漫反射反照率
    float4 DiffuseAlbedo;
    // 材质FrenselR0属性
    float3 FresnelR0;
    // 材质光滑度，光滑度 = 1 - 粗糙度
    float Shininess;
};

// 石里克近似计算fresnel方程
// R0: 材质属性FresnelR0
// normal：法向量
// lightVec：光向量
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
    // 入射角余弦值
    float cosTheta = dot(normal, lightVec);
    // 石里克近似法
    return R0 + (1 - R0) * pow(1 - cosTheta, 5);
}

// 距离衰减
// d：光源到反射位置距离
// falloffStart：
// falloffEnd：衰减到0距离
float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

// BlinnPhong光照模型，通过入射光强、光向量、法向量、观察向量与材质属性，计算反射光强，包含镜面反射和漫反射
// lightStrength: 入射光强，经Lambert余弦定律修正
// normal：法向量，单位向量
// lightVec：光向量，单位向量
// toEye：观察点到观察位置向量，单位向量
// mat：材质属性
// 返回：反射光强
float3 BlinnPhong(float3 lightStrength, float3 normal, float3 lightVec, float3 toEye, Material mat)
{
    float3 halfVec = normalize(lightVec + toEye);
    // fresnel因子
    float fresnelFactor = SchlickFresnel(mat.FresnelR0, halfVec, lightVec);
    // 粗糙度因子
    const float m = mat.Shininess * 256.0f;
    float roughnessFactor = (m + 8) * pow(max(dot(halfVec, lightVec), 0), m) / 8;
    // 镜面反射反照率
    float3 specularAlbedo = fresnelFactor * roughnessFactor;
    // 镜面反射公式得到的结果可能会超出范围[0,1]，等比例缩小一些
    specularAlbedo = specularAlbedo / (specularAlbedo + 1);
    // 反射光强
    return lightStrength * (mat.DiffuseAlbedo.rgb + specularAlbedo);
}

// 方向光源实现
// light：方向光源信息
// mat：材质信息
// normal：法向量
// toEye: 观察向量
// 返回：反射光强
float3 ComputeDirectLight(Light light, Material mat, float3 normal, float3 toEye)
{
    // 光向量与入射光方向相反
    float3 lightVec = -light.Direction;
    // Lambert余弦定律修正光强
    float3 lightStrength = light.Strength * max(dot(normal, lightVec), 0);
    // 应用BlinnPhong光照模型
    return BlinnPhong(lightStrength, normal, lightVec, toEye, mat);
}

// 点光源实现
// light：点光源信息
// mat：材质信息
// normal：法向量
// toEye: 观察向量
// pos：表面位置
// 返回：反射光强
float3 ComputePointLight(Light light, Material mat, float3 normal, float3 toEye, float3 pos)
{
    // 光向量方向
    float3 lightVec = light.Position - pos;
    // 光源到表面的距离
    float d = distance(light.Position, pos);
    // 规范化光向量
    lightVec = normalize(lightVec);
    // Lambert余弦定律修正光强
    float3 lightStrength = light.Strength * max(dot(normal, lightVec), 0);
    // 点光源距离衰减
    float att = CalcAttenuation(d, light.FalloffStart, light.FalloffEnd);
    lightStrength *= att;
    // 应用BlinnPhong光照模型
    return BlinnPhong(lightStrength, normal, lightVec, toEye, mat);
}

// 聚光灯光源实现
// light：点光源信息
// mat：材质信息
// normal：法向量
// toEye: 观察向量
// pos：表面位置
// 返回：反射光强
float3 ComputeSpotLight(Light light, Material mat, float3 normal, float3 toEye, float3 pos)
{
    // 光向量方向
    float3 lightVec = light.Position - pos;
    // 光源到表面的距离
    float d = distance(light.Position, pos);
    // 规范化光向量
    lightVec = normalize(lightVec);
    // Lambert余弦定律修正光强
    float3 lightStrength = light.Strength * max(dot(normal, lightVec), 0);
    // 点光源距离衰减
    float att = CalcAttenuation(d, light.FalloffStart, light.FalloffEnd);
    lightStrength *= att;
    // 聚光灯光源光强夹角衰减
    float spotFactor = pow(max(dot(-lightVec, light.Direction), 0), light.SpotPower);
    lightStrength *= spotFactor;
    // 应用BlinnPhong光照模型
    return BlinnPhong(lightStrength, normal, lightVec, toEye, mat);
}

// 叠加场景中所有的光源效果
// lights: 所有的光源数值，起始是方向光源，接下来是点光源，最后是聚光灯光源。所有光源按此顺序排列
float3 ComputeLight(Light lights[MAX_LIGHTS], Material mat, float3 normal, float3 toEye, float3 pos)
{
    float3 result = 0.0f;
    int i = 0;
    // 处理所有方向光源
#if (NUM_DIR_LIGHTS > 0)
    for (i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        result += ComputeDirectLight(lights[i], mat, normal, toEye);
    }
#endif
    // 处理所有点光源
#if (NUM_POINT_LIGHTS > 0)
    for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
    {
        result += ComputePointLight(lights[i], mat, normal, toEye, pos);
    }
#endif
    // 处理所有聚光源
#if (NUM_SPOT_LIGHTS > 0)
    for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        result += ComputeSpotLight(lights[i], mat, normal, toEye, pos);
    }
#endif
    return float4(result, 0.0f);
}