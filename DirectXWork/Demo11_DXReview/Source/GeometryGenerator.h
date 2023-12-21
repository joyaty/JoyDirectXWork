
#pragma once

/// <summary>
/// 顶点数据结构
/// </summary>
struct Vertex
{
public:
	Vertex()
		: Position(0.f, 0.f, 0.f)
		, Color(1.f, 1.f, 1.f, 1.f)
		, Normal(0.f, 1.f, 0.f)
		, TangentU(1.f, 0.f, 0.f)
		, TexCoord(0.f, 0.f)
	{}

	Vertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT3& tangentU, const DirectX::XMFLOAT2& texCoord)
		: Position(position)
		, Color(color)
		, Normal(normal)
		, TangentU(tangentU)
		, TexCoord(texCoord)
	{}

	Vertex(float posX, float posY, float posZ
		, float colorR, float colorG, float colorB, float alpha
		, float normalX, float normalY, float normalZ
		, float tangentUX, float tangentUY, float tangentUZ
		, float texCoordX, float texCoordY)
		: Position(posX, posY, posZ)
		, Color(colorR, colorG, colorB, alpha)
		, Normal(normalX, normalY, normalZ)
		, TexCoord(texCoordX, texCoordY)
	{}

public:
	/// <summary>
	/// 顶点坐标
	/// </summary>
	DirectX::XMFLOAT3 Position{};
	/// <summary>
	/// 顶点颜色
	/// </summary>
	DirectX::XMFLOAT4 Color{};
	/// <summary>
	/// 顶点法线
	/// </summary>
	DirectX::XMFLOAT3 Normal{};
	/// <summary>
	/// 顶点切线
	/// </summary>
	DirectX::XMFLOAT3 TangentU{};
	/// <summary>
	/// 顶点纹理坐标
	/// </summary>
	DirectX::XMFLOAT2 TexCoord{};
};

struct Mesh
{
public:
	std::vector<uint16_t>& GetIndices16()
	{
		if (m_Indices16.empty())
		{
			m_Indices16.resize(Indices.size());
			for (size_t i = 0; i < Indices.size(); ++i)
			{
				m_Indices16[i] = static_cast<uint16_t>(Indices[i]);
			}
		}
		return m_Indices16;
	}

public:
	/// <summary>
	/// 顶点数据
	/// </summary>
	std::vector<Vertex> Vertices{};
	/// <summary>
	/// 顶点索引，每个索引长度32
	/// </summary>
	std::vector<uint32_t> Indices{};

private:
	/// <summary>
	/// 顶点索引，每个索引长度16
	/// </summary>
	std::vector<uint16_t> m_Indices16{};
};

/// <summary>
/// 几何体生成器
/// </summary>
class GeometryGenerator
{
public:
	/// <summary>
	/// 创建圆柱体网格
	/// </summary>
	/// <param name="bottomRadius">圆柱体底部截面半径</param>
	/// <param name="topRadidus">圆柱体顶部截面半径</param>
	/// <param name="height">圆柱体高度</param>
	/// <param name="sliceCount">切面数量(圆柱截面方向的切割块数)</param>
	/// <param name="stackCount">分层数量(圆柱高度方向的切割层数)</param>
	/// <param name="outCylinderMesh">输出网格数据</param>
	static void CreateCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, Mesh& outCylinderMesh);

	/// <summary>
	/// 创建立方体网格
	/// </summary>
	/// <param name="width">长</param>
	/// <param name="height">宽</param>
	/// <param name="depth">高</param>
	/// <param name="nSubDivisons">三角面细分层数(层数越高，顶点越多)</param>
	/// <param name="outCubeMesh">输出网格数据</param>
	static void CreateCube(float width, float height, float depth, uint32_t nSubDivisons, Mesh& outCubeMesh);

	/// <summary>
	/// 创建球体网格
	/// </summary>
	/// <param name="radius">球半径</param>
	/// <param name="nSubDivisions">三角面细分层数(层数越高，顶点越多，球表面越精细)</param>
	/// <param name="outSphereMesh">输出网格数据</param>
	static void CreateSphere(float radius, uint32_t nSubDivisions, Mesh& outSphereMesh);

	/// <summary>
	/// 创建地表XZ平面网格
	/// </summary>
	/// <param name="width">长</param>
	/// <param name="depth">宽</param>
	/// <param name="outGridMesh">输出网格数据</param>
	/// <param name="row">行数，默认1</param>
	/// <param name="col">列数，默认1</param>
	static void CreateGrid(float width, float depth, Mesh& outGridMesh, uint32_t row = 1u, uint32_t col = 1u);

	/// <summary>
	/// 创建立面XY平面网格
	/// </summary>
	/// <param name="width">长</param>
	/// <param name="height">高</param>
	/// <param name="outQuadMesh">输出网格数据</param>
	/// <param name="row">行数，默认1</param>
	/// <param name="col">列数，默认1</param>
	static void CreateQuad(float width, float height, Mesh& outQuadMesh, uint32_t row = 1u, uint32_t col = 1u);

private:
	/// <summary>
	/// 构建圆柱体顶截面的网格数据
	/// </summary>
	/// <param name="bottomRadius">圆柱体底部截面半径</param>
	/// <param name="topRadidus">圆柱体顶部截面半径</param>
	/// <param name="height">圆柱体高度</param>
	/// <param name="sliceCount">切面数量(圆柱截面方向的切割块数)</param>
	/// <param name="stackCount">分层数量(圆柱高度方向的切割层数)</param>
	/// <param name="outCylinderMesh">输出网格数据</param>
	static void BuildCylinderTopCap(float bottomRadius, float topRadidus, float height, uint32_t sliceCount, uint32_t stackCount, Mesh& outCylinderMesh);

	/// <summary>
	/// 构建圆柱体底截面的网格数据
	/// </summary>
	/// <param name="bottomRadius">圆柱体底部截面半径</param>
	/// <param name="topRadidus">圆柱体顶部截面半径</param>
	/// <param name="height">圆柱体高度</param>
	/// <param name="sliceCount">切面数量(圆柱截面方向的切割块数)</param>
	/// <param name="stackCount">分层数量(圆柱高度方向的切割层数)</param>
	/// <param name="outCylinderMesh">输出网格数据</param>
	static void BuildCylinderBottomCap(float bottomRadius, float topRadidus, float height, uint32_t sliceCount, uint32_t stackCount, Mesh& outCylinderMesh);

	/// <summary>
	/// 三角面细分(三边去中点，构成新的三角面)
	/// </summary>
	/// <param name="outMesh">输入输出网格数据</param>
	static void SubDivide(Mesh& outMesh);

	/// <summary>
	/// 两个顶点中间的顶点
	/// </summary>
	/// <param name="v0">顶点0</param>
	/// <param name="v1">顶点1</param>
	static Vertex MidVertex(const Vertex& v0, const Vertex& v1);

};