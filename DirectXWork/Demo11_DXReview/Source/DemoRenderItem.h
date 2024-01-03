
#pragma once

#include "GeometryGenerator.h"
#include "Utils.h"

/// <summary>
/// 帧资源数量
/// </summary>
constexpr int kFrameResourceCount = 2;

/// <summary>
/// 最大光源数
/// </summary>
constexpr int kMaxLights = 16;

/// <summary>
/// 渲染层枚举定义
/// </summary>
enum class EnumRenderLayer
{
	kLayerOpaque = 0,			// 不透明物体
	kLayerTransparent = 1,		// 透明物体
	kLayerStencilMask = 2,		// 模版缓冲区写入层
	kLayerReflection = 3,		// 反射物件层

	kLayerCount
};

/// <summary>
/// 光源数据结构体
/// </summary>
struct Light
{
	/// <summary>
	/// 光强
	/// </summary>
	DirectX::XMFLOAT3 m_Strength;
	/// <summary>
	/// 光强衰减开始距离，点光源和聚光源类型有用
	/// </summary>
	float m_FalloffStart;
	/// <summary>
	/// 光照方向，方向光源和聚光源类型有用
	/// </summary>
	DirectX::XMFLOAT3 m_Direction;
	/// <summary>
	/// 光强衰减到0距离，点光源和聚光源类型有用
	/// </summary>
	float m_FalloffEnd;
	/// <summary>
	/// 光源位置，点光源和聚光源类型有用
	/// </summary>
	DirectX::XMFLOAT3 m_Position;
	/// <summary>
	/// 聚光源的夹角指数幂
	/// </summary>
	float m_SpotPower;
};

/// <summary>
/// 网格几何体
/// </summary>
struct MeshGeometry
{
public:
	/// <summary>
	/// 子网格相关的偏移数据
	/// 一个大的顶点和索引缓冲区数据可能是几个小Mesh顶点和索引数据的合并
	/// 这里记录各个子Mesh的偏移数据，便于绘制各个小Mesh
	/// </summary>
	struct SubMeshGeometry
	{
		/// <summary>
		/// 子Mesh索引个数
		/// </summary>
		uint32_t m_IndexCount;
		/// <summary>
		/// 子Mesh在索引缓冲区位置的偏移
		/// </summary>
		uint32_t m_StartIndexLocation;
		/// <summary>
		/// 子Mesh的基准顶点索引
		/// 几个子Mesh组合成大的顶点缓冲区，子Mesh的真实索引发生偏移
		/// 真实索引 = 子Mesh索引缓冲区的值 + 子Mesh第一个顶点在顶点缓冲区的索引
		/// </summary>
		uint32_t m_BaseVertexLocation;
	};

public:
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

public:
	/// <summary>
	/// 几何体的名称
	/// </summary>
	std::string m_Name;

	/// <summary>
	/// 指向GPU内存默认堆上的内存块，作为顶点数据存储位置
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBufferGPU{ nullptr };
	/// <summary>
	/// 指向GPU内存上传堆中的内存块，作为CPU提交顶点数据到GPU默认堆内存上的中介
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBufferUploader{ nullptr };
	/// <summary>
	/// 单个顶点数据的步长
	/// </summary>
	uint32_t m_VertexStride;
	/// <summary>
	/// 顶点数据的大小
	/// </summary>
	uint32_t m_VertexSize;

	/// <summary>
	/// 指向GPU内存默认堆上的内存块，作为索引数据存储位置
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBufferGPU{ nullptr };
	/// <summary>
	/// 指向GPU内存上传堆中的内存块，作为CPU提交索引数据到GPU默认堆内存上的中介
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBufferUploader{ nullptr };
	/// <summary>
	/// 索引数据的格式，一般为16位or32位
	/// </summary>
	DXGI_FORMAT m_IndexFormat;
	/// <summary>
	/// 索引数据的总大小
	/// </summary>
	uint32_t m_IndexSize;

	/// <summary>
	/// 子Mesh相关的数据，一个复杂的几何体可能由几个简单的子Mesh组合而成
	/// </summary>
	std::unordered_map<std::string, SubMeshGeometry> m_SubMeshGeometrys{};
};

/// <summary>
/// 材质属性结构
/// </summary>
struct Material
{
	/// <summary>
	/// 材质名称
	/// </summary>
	std::string m_Name;
	/// <summary>
	/// 材质漫反射照率
	/// </summary>
	DirectX::XMFLOAT4 m_DiffuseAlbedo;
	/// <summary>
	/// 菲涅尔效应R0属性
	/// </summary>
	DirectX::XMFLOAT3 m_FresnelR0;
	/// <summary>
	/// 粗糙度[0, 1]，0-光滑，1-粗糙
	/// </summary>
	float m_Roughness;
	/// <summary>
	/// 关联的材质常量缓冲区索引
	/// </summary>
	uint32_t m_CbvIndex;
	/// <summary>
	/// 关联的漫反射反照率贴图索引
	/// </summary>
	uint32_t m_DiffuseMapIndex;

	/// <summary>
	/// 材质动画变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_MatMatrix{ MathUtil::Identity4x4() };

	/// <summary>
	/// 脏标记
	/// </summary>
	int m_NumFrameDirty = kFrameResourceCount;
};

/// <summary>
/// 纹理数据结构
/// </summary>
struct Texture
{
	std::string m_Name;
	std::wstring m_FilePath;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_TextureGPU{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Resource> m_TextureUploader{ nullptr };
};

/// <summary>
/// 每个物体层级的常量缓冲区数据结构
/// </summary>
struct PerObjectConstants
{
	/// <summary>
	/// 本地空间变换到世界空间的变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_LocalToWorldMatrix{ MathUtil::Identity4x4() };

	/// <summary>
	/// 纹理坐标变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_TexMatrix{ MathUtil::Identity4x4() };
};

/// <summary>
/// 材质层级的常量缓冲区数据结构
/// </summary>
struct PerMaterialConstants
{
	/// <summary>
	/// 漫反射照率
	/// </summary>
	DirectX::XMFLOAT4 m_DiffuseAlbedo;
	/// <summary>
	/// Fresnel效应R0属性
	/// </summary>
	DirectX::XMFLOAT3 m_FresnelR0;
	/// <summary>
	/// 粗糙度
	/// </summary>
	float m_Roughness;
	/// <summary>
	/// UV变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_UVMatrix{ MathUtil::Identity4x4() };
};

/// <summary>
/// 每个渲染过程的常量缓冲区数据结构
/// </summary>
struct PerPassConstants
{
	/// <summary>
	/// 观察变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_ViewMatrix;
	/// <summary>
	/// 观察矩阵逆矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_InvViewMatrix;
	/// <summary>
	/// 投影变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_ProjMatrix;
	/// <summary>
	/// 投影矩阵逆矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_InvProjMatrix;
	/// <summary>
	/// 观察投影矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_ViewProjMatrix;
	/// <summary>
	/// 观察投影矩阵逆矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_InvViewProjMatrix;
	/// <summary>
	/// 观察者位置
	/// </summary>
	DirectX::XMFLOAT3 m_EyePos;
	/// <summary>
	/// 展位字节，常量缓冲区按4*32字节打包，这里填充一个32位的展位字节，和m_EyePos组合一个4*32长度，避免拷贝的GPU错位到后面的数据
	/// </summary>
	float __padding0;
	/// <summary>
	/// 渲染目标宽高
	/// </summary>
	DirectX::XMFLOAT2 m_RenderTargetSize;
	/// <summary>
	/// 渲染对象的宽高倒数
	/// </summary>
	DirectX::XMFLOAT2 m_InvRenderTargetSize;
	/// <summary>
	/// 近平面
	/// </summary>
	float m_NearPlane;
	/// <summary>
	/// 原平面
	/// </summary>
	float m_FarPlane;
	/// <summary>
	/// 帧间隔时间
	/// </summary>
	float m_DeltaTime;
	/// <summary>
	/// 总时间
	/// </summary>
	float m_TotalTime;
	/// <summary>
	/// 雾颜色
	/// </summary>
	DirectX::XMFLOAT4 m_FogColor;
	/// <summary>
	/// 雾开始位置
	/// </summary>
	float m_FogStart;
	/// <summary>
	/// 雾不可见位置
	/// </summary>
	float m_FogEnd;

	/// <summary>
	/// 占位，与m_FogStart、m_FogEnd组合成4D向量
	/// </summary>
	float __padding1[2];

	/// <summary>
	/// 环境光
	/// </summary>
	DirectX::XMFLOAT4 m_AmbientLight;
	/// <summary>
	/// 所有的直接光
	/// </summary>
	Light m_AllLights[kMaxLights];
};

/// <summary>
/// 渲染项，渲染一个场景物件的数据集合
/// </summary>
struct DemoRenderItem
{
	/// <summary>
	/// 本地空间到世界空间的变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_LoclToWorldMatrix = MathUtil::Identity4x4();

	/// <summary>
	/// 纹理坐标变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_TexMatrix{ MathUtil::Identity4x4() };

	/// <summary>
	/// 渲染项绑定的网格集合体
	/// </summary>
	MeshGeometry* m_MeshGeo;
	/// <summary>
	/// 渲染项关联的物体常量缓冲区索引
	/// </summary>
	uint32_t m_ObjectCBIndex;

	/// <summary>
	/// 关联的材质
	/// </summary>
	Material* m_Material;

	/// <summary>
	/// 图元拓扑
	/// </summary>
	D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexInstance的参数
	uint32_t m_IndexCount;
	uint32_t m_StartIndexLocation;
	uint32_t m_StartVertexLocation;

	int m_NumFrameDirty = kFrameResourceCount;
};