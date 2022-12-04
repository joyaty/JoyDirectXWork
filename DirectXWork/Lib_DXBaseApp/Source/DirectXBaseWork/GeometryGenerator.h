/*
 * GeometryGenerator.h
 * 程序化几何体生成器
 */

#pragma once

 /// <summary>
 /// 程序化几何体生成器
 /// </summary>
class GeometryGenerator
{
public:
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;

public:
	/// <summary>
	/// 程序化几何体的顶点数据结构
	/// </summary>
	struct Vertex
	{
	public:
		Vertex() {}
		Vertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT3& tangent, const DirectX::XMFLOAT2& texCoord)
			: Position(position)
			, Normal(normal)
			, TangentU(tangent)
			, TexCoord(texCoord)
		{
		}
		Vertex(float posX, float posY, float posZ,
			float normalX, float normalY, float normalZ,
			float tangentX, float tangentY, float tangentZ,
			float texCoordX, float texCoordY)
			: Position(posX, posY, posZ)
			, Normal(normalX, normalY, normalZ)
			, TangentU(tangentX, tangentY, tangentZ)
			, TexCoord(texCoordX, texCoordY)
		{
		}

	public:
		/// <summary>
		/// 顶点坐标
		/// </summary>
		DirectX::XMFLOAT3 Position{};
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

	/// <summary>
	/// 程序化几何体的网格数据结构
	/// </summary>
	struct MeshData
	{
	public:
		/// <summary>
		/// 顶点数据集合
		/// </summary>
		std::vector<Vertex> Vertices{};
		/// <summary>
		/// 索引数据集合
		/// </summary>
		std::vector<uint32> Indices32{};

		/// <summary>
		/// 获取16位的索引，索引数小于65535
		/// </summary>
		/// <returns></returns>
		std::vector<uint16>& GetIndices16()
		{
			if (m_Indices16.empty())
			{
				m_Indices16.resize(Indices32.size());
				for (size_t i = 0; i < Indices32.size(); ++i)
				{
					m_Indices16[i] = static_cast<uint16>(Indices32[i]);
				}
			}
			return m_Indices16;
		}

	private:
		/// <summary>
		/// 16位的索引数据集合
		/// </summary>
		std::vector<uint16> m_Indices16{};
	};

public:
	/// <summary>
	/// 构建柱体网格数据
	/// </summary>
	/// <param name="bottomRadius">底面半径</param>
	/// <param name="topRadius">顶面半径</param>
	/// <param name="height">柱体高度</param>
	/// <param name="sliceCount">切面数量，顶底方向的分割块数</param>
	/// <param name="stackCount">分层数量，高度方向的分割块数</param>
	/// <returns>柱体网格数据</returns>
	static GeometryGenerator::MeshData CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount);

	/// <summary>
	/// 构建长方体网格数据
	/// </summary>
	/// <param name="width">长</param>
	/// <param name="height">高</param>
	/// <param name="depth">宽</param>
	/// <param name="numSubdivisions">面细分数</param>
	/// <returns></returns>
	static GeometryGenerator::MeshData CreateCube(float width, float height, float depth, uint32 numSubdivisions);

	/// <summary>
	/// 构建球体网格数据
	/// </summary>
	/// <param name="radius">半径</param>
	/// <param name="sliceCount">纵向切分数</param>
	/// <param name="stackCount">横向切分数</param>
	/// <returns></returns>
	static GeometryGenerator::MeshData CreateSphere(float radius, uint32 sliceCount, uint32 stackCount);

	/// <summary>
	/// 构建球体网格数据(正二十面体细分逼近)
	/// </summary>
	/// <param name="radius">半径</param>
	/// <param name="numSubdivisions">面细分数</param>
	/// <returns></returns>
	static GeometryGenerator::MeshData CreateGeoShpere(float radius, uint32 numSubdivisions);

	/// <summary>
	/// 构建网格XZ平面
	/// </summary>
	/// <param name="width">平面长度</param>
	/// <param name="depth">平面深度</param>
	/// <param name="row">行数</param>
	/// <param name="col">列数</param>
	/// <returns></returns>
	static GeometryGenerator::MeshData CreateGrid(float width, float depth, uint32 row, uint32 col);

	//static GeometryGenerator::MeshData CreateQuad()

private:
	/// <summary>
	/// 构建柱体顶截面的网格数据
	/// </summary>
	/// <param name="bottomRadius">底面半径</param>
	/// <param name="topRadius">顶面半径</param>
	/// <param name="height">柱体高度</param>
	/// <param name="sliceCount">切面数量，顶底方向的分割块数</param>
	/// <param name="stackCount">分层数量，高度方向的分割块数</param>
	/// <param name="meshData">柱体网格数据引用</param>
	static void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, GeometryGenerator::MeshData& meshData);

	/// <summary>
	/// 构建柱体底截面的网格数据
	/// </summary>
	/// <param name="bottomRadius">底面半径</param>
	/// <param name="topRadius">顶面半径</param>
	/// <param name="height">柱体高度</param>
	/// <param name="sliceCount">切面数量，顶底方向的分割块数</param>
	/// <param name="stackCount">分层数量，高度方向的分割块数</param>
	/// <param name="meshData">柱体网格数据引用</param>
	static void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, GeometryGenerator::MeshData& meshData);

	/// <summary>
	/// 细分三角面
	/// </summary>
	/// <param name="meshData">网格数据</param>
	static void SubDivide(GeometryGenerator::MeshData& meshData);
	
	/// <summary>
	/// 计算两个顶点中间顶点数据
	/// </summary>
	/// <param name="v0">顶点0</param>
	/// <param name="v1">顶点1</param>
	/// <returns>中间顶点数据</returns>
	static GeometryGenerator::Vertex MidVertex(const Vertex& v0, const Vertex& v1);
};
