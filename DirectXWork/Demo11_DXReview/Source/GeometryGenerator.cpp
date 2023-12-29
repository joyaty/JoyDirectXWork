
#include "stdafx.h"
#include "GeometryGenerator.h"

using namespace DirectX;

void GeometryGenerator::CreateCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, Mesh& outCylinderMesh)
{
	// 圆柱分层层高
	float stackHeight = height / stackCount;
	// 每个分层的半径增量步长
	float stackRadiusStep = (topRadius - bottomRadius) / stackCount;
	// 分层的边数，一层两边，两层三条环边
	uint32_t ringCount = stackCount + 1;
	// 纵向切面间的夹角
	float dSliceTheta = DirectX::XM_2PI / sliceCount;
	// 计算侧面的顶点信息
	for (uint32_t i = 0; i < ringCount; ++i)
	{
		float stackRadius = bottomRadius + i * stackRadiusStep;
		float stackY = -0.5f * height + i * stackHeight;
		// 第一个点同时作为起点和终点需要计算两次，虽然顶点坐标一样，但是纹理坐标不一样，是不一样的顶点
		for (uint32_t j = 0; j <= sliceCount; ++j)
		{
			Vertex vertex{};
			float fSinValue = sinf(j * dSliceTheta);
			float fCosValue = cosf(j * dSliceTheta);
			// 顶点位置
			vertex.Position.x = stackRadius * fCosValue;
			vertex.Position.y = stackY;
			vertex.Position.z = stackRadius * fSinValue;
			// 顶点纹理坐标，DX纹理坐标系原点在左上角
			vertex.TexCoord.x = static_cast<float>(j) / sliceCount;
			vertex.TexCoord.y = 1.0f - static_cast<float>(i) / stackCount;
			// 顶点切线
			vertex.TangentU = DirectX::XMFLOAT3(-fSinValue, 0.f, fCosValue);
			// 通过切线空间TBN计算法线
			float dr = bottomRadius - topRadius;
			DirectX::XMFLOAT3 bitangent(dr * fCosValue, -height, dr * fSinValue);
			DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&vertex.TangentU);
			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&bitangent);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(T, B));
			DirectX::XMStoreFloat3(&vertex.Normal, N);
			// 顶点颜色
			vertex.Color = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
			outCylinderMesh.Vertices.push_back(vertex);
		}
	}
	// 计算柱体侧面顶点索引
	// 第i+1层  B*-----*C
	//           |   / |
	//           | /   |
	// 第i层    A*-----*D
	int ringVertexCount = sliceCount + 1;
	for (uint32_t i = 0; i < stackCount; ++i)
	{
		for (uint32_t j = 0; j < sliceCount; ++j)
		{
			// 如上图，计算ABCD四个顶点的索引
			uint32_t indexA = i * ringVertexCount + j;
			uint32_t indexB = (i + 1) * ringVertexCount + j;
			uint32_t indexC = (i + 1) * ringVertexCount + j + 1;
			uint32_t indexD = i * ringVertexCount + j + 1;
			// 三角形ABC
			outCylinderMesh.Indices.push_back(indexA);
			outCylinderMesh.Indices.push_back(indexB);
			outCylinderMesh.Indices.push_back(indexC);
			// 三角形ACD
			outCylinderMesh.Indices.push_back(indexA);
			outCylinderMesh.Indices.push_back(indexC);
			outCylinderMesh.Indices.push_back(indexD);
		}
	}
	// 顶界面的网格数据
	BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, outCylinderMesh);
	// 底截面的网格数据
	BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, outCylinderMesh);
}

void GeometryGenerator::CreateCube(float width, float height, float depth, uint32_t nSubDivisons, Mesh& outCubeMesh)
{
	float fHalfWidth = width * 0.5f;
	float fHalfHeight = height * 0.5f;
	float fHalfDepth = depth * 0.5f;
	outCubeMesh.Vertices.resize(24);
	// 前面
	outCubeMesh.Vertices[0]  = Vertex(-fHalfWidth, -fHalfHeight, -fHalfDepth, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, 0.f, 1.f);
	outCubeMesh.Vertices[1]  = Vertex(-fHalfWidth, +fHalfHeight, -fHalfDepth, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, 0.f, 0.f);
	outCubeMesh.Vertices[2]  = Vertex(+fHalfWidth, +fHalfHeight, -fHalfDepth, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, 1.f, 0.f);
	outCubeMesh.Vertices[3]  = Vertex(+fHalfWidth, -fHalfHeight, -fHalfDepth, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, 1.f, 1.f);
	// 背面
	outCubeMesh.Vertices[4]  = Vertex(-fHalfWidth, -fHalfHeight, +fHalfDepth, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, 1.f, 1.f);
	outCubeMesh.Vertices[5]  = Vertex(+fHalfWidth, -fHalfHeight, +fHalfDepth, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, 0.f, 1.f);
	outCubeMesh.Vertices[6]  = Vertex(+fHalfWidth, +fHalfHeight, +fHalfDepth, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, 0.f, 0.f);
	outCubeMesh.Vertices[7]  = Vertex(-fHalfWidth, +fHalfHeight, +fHalfDepth, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, 1.f, 0.f);
	// 顶面
	outCubeMesh.Vertices[8]  = Vertex(-fHalfWidth, +fHalfHeight, -fHalfDepth, 1.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f);
	outCubeMesh.Vertices[9]  = Vertex(-fHalfWidth, +fHalfHeight, +fHalfDepth, 1.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f);
	outCubeMesh.Vertices[10] = Vertex(+fHalfWidth, +fHalfHeight, +fHalfDepth, 1.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f);
	outCubeMesh.Vertices[11] = Vertex(+fHalfWidth, +fHalfHeight, -fHalfDepth, 1.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f);
	// 底面
	outCubeMesh.Vertices[12] = Vertex(-fHalfWidth, -fHalfHeight, -fHalfDepth, 1.f, 1.f, 1.f, 1.f, 0.f, -1.f, 0.f, -1.f, 0.f, 0.f, 1.f, 1.f);
	outCubeMesh.Vertices[13] = Vertex(+fHalfWidth, -fHalfHeight, -fHalfDepth, 1.f, 1.f, 1.f, 1.f, 0.f, -1.f, 0.f, -1.f, 0.f, 0.f, 0.f, 1.f);
	outCubeMesh.Vertices[14] = Vertex(+fHalfWidth, -fHalfHeight, +fHalfDepth, 1.f, 1.f, 1.f, 1.f, 0.f, -1.f, 0.f, -1.f, 0.f, 0.f, 0.f, 0.f);
	outCubeMesh.Vertices[15] = Vertex(-fHalfWidth, -fHalfHeight, +fHalfDepth, 1.f, 1.f, 1.f, 1.f, 0.f, -1.f, 0.f, -1.f, 0.f, 0.f, 1.f, 0.f);
	// 左面
	outCubeMesh.Vertices[16] = Vertex(-fHalfWidth, -fHalfHeight, +fHalfDepth, 1.f, 1.f, 1.f, 1.f, -1.f, 0.f, 0.f, 0.f, 0.f, -1.f, 0.f, 1.f);
	outCubeMesh.Vertices[17] = Vertex(-fHalfWidth, +fHalfHeight, +fHalfDepth, 1.f, 1.f, 1.f, 1.f, -1.f, 0.f, 0.f, 0.f, 0.f, -1.f, 0.f, 0.f);
	outCubeMesh.Vertices[18] = Vertex(-fHalfWidth, +fHalfHeight, -fHalfDepth, 1.f, 1.f, 1.f, 1.f, -1.f, 0.f, 0.f, 0.f, 0.f, -1.f, 1.f, 0.f);
	outCubeMesh.Vertices[19] = Vertex(-fHalfWidth, -fHalfHeight, -fHalfDepth, 1.f, 1.f, 1.f, 1.f, -1.f, 0.f, 0.f, 0.f, 0.f, -1.f, 1.f, 1.f);
	// 右面
	outCubeMesh.Vertices[20] = Vertex(+fHalfWidth, -fHalfHeight, -fHalfDepth, 1.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f);
	outCubeMesh.Vertices[21] = Vertex(+fHalfWidth, +fHalfHeight, -fHalfDepth, 1.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f);
	outCubeMesh.Vertices[22] = Vertex(+fHalfWidth, +fHalfHeight, +fHalfDepth, 1.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f);
	outCubeMesh.Vertices[23] = Vertex(+fHalfWidth, -fHalfHeight, +fHalfDepth, 1.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f);
	// 创建索引，组合为三角形
	outCubeMesh.Indices.resize(36);
	// 前面
	outCubeMesh.Indices[0]  = 0; outCubeMesh.Indices[1]  = 1; outCubeMesh.Indices[2]  = 2;
	outCubeMesh.Indices[3]  = 0; outCubeMesh.Indices[4]  = 2; outCubeMesh.Indices[5]  = 3;
	// 背面
	outCubeMesh.Indices[6]  = 4; outCubeMesh.Indices[7]  = 5; outCubeMesh.Indices[8]  = 6;
	outCubeMesh.Indices[9]  = 4; outCubeMesh.Indices[10] = 6; outCubeMesh.Indices[11] = 7;
	// 顶面
	outCubeMesh.Indices[12] = 8; outCubeMesh.Indices[13] = 9; outCubeMesh.Indices[14] = 10;
	outCubeMesh.Indices[15] = 8; outCubeMesh.Indices[16] = 10; outCubeMesh.Indices[17] = 11;
	// 底面
	outCubeMesh.Indices[18] = 12; outCubeMesh.Indices[19] = 13; outCubeMesh.Indices[20] = 14;
	outCubeMesh.Indices[21] = 12; outCubeMesh.Indices[22] = 14; outCubeMesh.Indices[23] = 15;
	// 左面
	outCubeMesh.Indices[24] = 16; outCubeMesh.Indices[25] = 17; outCubeMesh.Indices[26] = 18;
	outCubeMesh.Indices[27] = 16; outCubeMesh.Indices[28] = 18; outCubeMesh.Indices[29] = 19;
	// 右面
	outCubeMesh.Indices[30] = 20; outCubeMesh.Indices[31] = 21; outCubeMesh.Indices[32] = 22;
	outCubeMesh.Indices[33] = 20; outCubeMesh.Indices[34] = 22; outCubeMesh.Indices[35] = 23;
	// 细分面
	nSubDivisons = std::min<uint32_t>(nSubDivisons, 6u);
	for (uint32_t i = 0; i < nSubDivisons; ++i)
	{
		SubDivide(outCubeMesh);
	}
}

void GeometryGenerator::CreateSphere(float radius, uint32_t nSubDivisions, Mesh& outSphereMesh)
{
	// 最多5个
	nSubDivisions = std::min<uint32_t>(nSubDivisions, 5u);
	// 正二十面体
	const float X = 0.525731f;
	const float Z = 0.850651f;
	DirectX::XMFLOAT3 pos[12] =
	{
		XMFLOAT3(-X, 0.0f, Z),  XMFLOAT3(X, 0.0f, Z),
		XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
		XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
		XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
		XMFLOAT3(Z, X, 0.0f),   XMFLOAT3(-Z, X, 0.0f),
		XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
	};
	uint32_t k[60] =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	outSphereMesh.Vertices.resize(12);
	outSphereMesh.Indices.assign(&k[0], &k[60]);
	// 构造正二十面体的顶点数据
	for (uint32_t i = 0; i < 12; ++i)
	{
		outSphereMesh.Vertices[i].Position = pos[i];
	}
	// 正二十面体曲面细分的方式，逼近球体
	for (uint32_t i = 0; i < nSubDivisions; ++i)
	{
		SubDivide(outSphereMesh);
	}
	// 计算各个顶点的法线切线纹理坐标
	for (uint32_t i = 0; i < outSphereMesh.Vertices.size(); ++i)
	{
		// Project onto unit sphere.
		XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&outSphereMesh.Vertices[i].Position));
		// Project onto sphere.
		XMVECTOR p = radius * n;
		XMStoreFloat3(&outSphereMesh.Vertices[i].Position, p);
		XMStoreFloat3(&outSphereMesh.Vertices[i].Normal, n);
		// Derive texture coordinates from spherical coordinates.
		float theta = atan2f(outSphereMesh.Vertices[i].Position.z, outSphereMesh.Vertices[i].Position.x);
		// Put in [0, 2pi].
		if (theta < 0.0f)
		{
			theta += DirectX::XM_2PI;
		}
		float phi = acosf(outSphereMesh.Vertices[i].Position.y / radius);
		outSphereMesh.Vertices[i].TexCoord.x = theta / DirectX::XM_2PI;
		outSphereMesh.Vertices[i].TexCoord.y = phi / DirectX::XM_PI;
		// Partial derivative of P with respect to theta
		outSphereMesh.Vertices[i].TangentU.x = -radius * sinf(phi) * sinf(theta);
		outSphereMesh.Vertices[i].TangentU.y = 0.0f;
		outSphereMesh.Vertices[i].TangentU.z = +radius * sinf(phi) * cosf(theta);
		XMVECTOR T = XMLoadFloat3(&outSphereMesh.Vertices[i].TangentU);
		XMStoreFloat3(&outSphereMesh.Vertices[i].TangentU, XMVector3Normalize(T));
		// 顶点颜色
		outSphereMesh.Vertices[i].Color = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	}
}

void GeometryGenerator::CreateGrid(float width, float depth, Mesh& outGridMesh, uint32_t row/* = 1u*/, uint32_t col/* = 1u*/)
{
	// 总的顶点数
	uint32_t totalVertexCount = (row + 1) * (col + 1);
	// 行间隔
	float dx = width / col;
	// 列间隔
	float dz = depth / row;
	// uv坐标间隔
	float du = 1.f / col;
	float dv = 1.f / row;
	outGridMesh.Vertices.resize(totalVertexCount);
	for (uint32_t i = 0; i < row + 1; ++i)
	{
		float posZ = -depth * 0.5f + i * dz;
		for (uint32_t j = 0; j < col + 1; ++j)
		{
			float posX = -width * 0.5f + j * dx;
			uint32_t index = i * (row + 1) + j;
			outGridMesh.Vertices[index].Position = DirectX::XMFLOAT3(posX, 0.f, posZ);
			outGridMesh.Vertices[index].Color = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
			outGridMesh.Vertices[index].Normal = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
			outGridMesh.Vertices[index].TangentU = DirectX::XMFLOAT3(1.f, 0.f, 0.f);
			outGridMesh.Vertices[index].TexCoord = DirectX::XMFLOAT2(j * du, 1.f - i * dv);
		}
	}
	outGridMesh.Indices.resize(row * col * 2 * 3);
	uint32_t k = 0;
	for (uint32_t i = 0; i < row; ++i)
	{
		for (uint32_t j = 0; j < col; ++j)
		{
			// 第一个三角形
			outGridMesh.Indices[k] = i * (col + 1) + j;
			outGridMesh.Indices[k + 1] = (i + 1) * (col + 1) + j + 1;
			outGridMesh.Indices[k + 2] = i * (col + 1) + j + 1;
			// 第二个三角形
			outGridMesh.Indices[k + 3] = i * (col + 1) + j;
			outGridMesh.Indices[k + 4] = (i + 1) * (col + 1) + j;
			outGridMesh.Indices[k + 5] = (i + 1) * (col + 1) + j + 1;
			// 四边形，4个顶点，组成2个三角形共6个索引值
			k += 6;
		}
	}
}

void GeometryGenerator::CreateQuad(float width, float height, Mesh& outQuadMesh, uint32_t row/* = 1u*/, uint32_t col/* = 1u*/)
{
	// 总的顶点数
	uint32_t totalVertexCount = (row + 1) * (col + 1);
	// 行间隔
	float dx = width / col;
	// 列间隔
	float dy = height / row;
	// uv坐标间隔
	float du = 1.f / col;
	float dv = 1.f / row;
	outQuadMesh.Vertices.resize(totalVertexCount);
	for (uint32_t i = 0; i < row + 1; ++i)
	{
		float posY = -height * 0.5f + i * dy;
		for (uint32_t j = 0; j < col + 1; ++j)
		{
			float posX = -width * 0.5f + j * dx;
			uint32_t index = i * (row + 1) + j;
			outQuadMesh.Vertices[index].Position = DirectX::XMFLOAT3(posX, posY, 0.f);
			outQuadMesh.Vertices[index].Color = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
			outQuadMesh.Vertices[index].Normal = DirectX::XMFLOAT3(0.f, 0.f, -1.f);
			outQuadMesh.Vertices[index].TangentU = DirectX::XMFLOAT3(-1.f, 0.f, 0.f);
			outQuadMesh.Vertices[index].TexCoord = DirectX::XMFLOAT2(j * du, 1.f - i * dv);
		}
	}
	outQuadMesh.Indices.resize(row * col * 2 * 3);
	uint32_t k = 0;
	for (uint32_t i = 0; i < row; ++i)
	{
		for (uint32_t j = 0; j < col; ++j)
		{
			// 第一个三角形
			outQuadMesh.Indices[k] = i * (col + 1) + j;
			outQuadMesh.Indices[k + 1] = (i + 1) * (col + 1) + j + 1;
			outQuadMesh.Indices[k + 2] = i * (col + 1) + j + 1;
			// 第二个三角形
			outQuadMesh.Indices[k + 3] = i * (col + 1) + j;
			outQuadMesh.Indices[k + 4] = (i + 1) * (col + 1) + j;
			outQuadMesh.Indices[k + 5] = (i + 1) * (col + 1) + j + 1;
			// 四边形，4个顶点，组成2个三角形共6个索引值
			k += 6;
		}
	}
}

void GeometryGenerator::BuildCylinderTopCap(float bottomRadius, float topRadidus, float height, uint32_t sliceCount, uint32_t stackCount, Mesh& outCylinderMesh)
{
	uint32_t beginVertexIndex = static_cast<uint32_t>(outCylinderMesh.Vertices.size());
	float stackY = 0.5f * height;
	float dSliceTheta = DirectX::XM_2PI / sliceCount;
	for (uint32_t i = 0; i <= sliceCount; ++i)
	{
		float posX = topRadidus * cosf(i * dSliceTheta);
		float posZ = topRadidus * sinf(i * dSliceTheta);
		float u = posX / height + 0.5f;
		float v = posZ / height + 0.5f;
		outCylinderMesh.Vertices.emplace_back(
			posX, stackY, posZ,
			1.f, 1.f, 1.f, 1.f,
			0.f, 1.f, 0.f,
			1.f, 0.f, 0.f,
			u, v);
	}
	// 额外添加一个顶层中心顶点
	outCylinderMesh.Vertices.emplace_back(0.f, stackY, 0.f,
		1.f, 1.f, 1.f, 1.f,
		0.f, 1.f, 0.f,
		1.f, 0.f, 0.f,
		0.5f, 0.5f);
	uint32_t centerVertexIndex = static_cast<uint32_t>(outCylinderMesh.Vertices.size() - 1);
	// 计算顶界面的三角形索引
	for (uint32_t i = 0; i < sliceCount; ++i)
	{
		outCylinderMesh.Indices.push_back(centerVertexIndex);
		outCylinderMesh.Indices.push_back(beginVertexIndex + i + 1);
		outCylinderMesh.Indices.push_back(beginVertexIndex + i);
	}
}

void GeometryGenerator::BuildCylinderBottomCap(float bottomRadius, float topRadidus, float height, uint32_t sliceCount, uint32_t stackCount, Mesh& outCylinderMesh)
{
	uint32_t beginVertexIndex = static_cast<uint32_t>(outCylinderMesh.Vertices.size());
	float stackY = -0.5f * height;
	float dSliceTheta = DirectX::XM_2PI / sliceCount;
	for (uint32_t i = 0; i <= sliceCount; ++i)
	{
		float posX = bottomRadius * cosf(i * dSliceTheta);
		float posZ = bottomRadius * sinf(i * dSliceTheta);
		float u = posX / height + 0.5f;
		float v = posZ / height + 0.5f;
		outCylinderMesh.Vertices.emplace_back(
			posX, stackY, posZ,
			1.f, 1.f, 1.f, 1.f,
			0.f, -1.f, 0.f,
			1.f, 0.f, 0.f,
			u, v);
	}
	// 额外添加一个顶层中心顶点
	outCylinderMesh.Vertices.emplace_back(0.f, stackY, 0.f,
		1.f, 1.f, 1.f, 1.f,
		0.f, -1.f, 0.f,
		1.f, 0.f, 0.f,
		0.5f, 0.5f);
	uint32_t centerVertexIndex = static_cast<uint32_t>(outCylinderMesh.Vertices.size() - 1);
	// 计算顶界面的三角形索引
	for (uint32_t i = 0; i < sliceCount; ++i)
	{
		outCylinderMesh.Indices.push_back(centerVertexIndex);
		outCylinderMesh.Indices.push_back(beginVertexIndex + i);
		outCylinderMesh.Indices.push_back(beginVertexIndex + i + 1);
	}
}

void GeometryGenerator::SubDivide(Mesh& outMesh)
{
	Mesh inputCopy = outMesh;
	outMesh.Vertices.resize(0);
	outMesh.Indices.resize(0);
	//       v1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2
	// 三角面的个数
	uint32_t numTris = (uint32_t)inputCopy.Indices.size() / 3;
	// 遍历所有三角面
	for (uint32_t i = 0; i < numTris; ++i)
	{
		// 获取三角面三个顶点数据
		Vertex v0 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 0]];
		Vertex v1 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 1]];
		Vertex v2 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 2]];
		// 计算三个边的中点
		Vertex m0 = MidVertex(v0, v1);
		Vertex m1 = MidVertex(v1, v2);
		Vertex m2 = MidVertex(v0, v2);
		// 添加细分的顶点和索引
		outMesh.Vertices.push_back(v0); // 0
		outMesh.Vertices.push_back(v1); // 1
		outMesh.Vertices.push_back(v2); // 2
		outMesh.Vertices.push_back(m0); // 3
		outMesh.Vertices.push_back(m1); // 4
		outMesh.Vertices.push_back(m2); // 5
		// 三角形v0m0m2
		outMesh.Indices.push_back(i * 6 + 0);
		outMesh.Indices.push_back(i * 6 + 3);
		outMesh.Indices.push_back(i * 6 + 5);
		// 三角形m0m1m2
		outMesh.Indices.push_back(i * 6 + 3);
		outMesh.Indices.push_back(i * 6 + 4);
		outMesh.Indices.push_back(i * 6 + 5);
		// 三角形m2m1v2
		outMesh.Indices.push_back(i * 6 + 5);
		outMesh.Indices.push_back(i * 6 + 4);
		outMesh.Indices.push_back(i * 6 + 2);
		// 三角形m0v1m1
		outMesh.Indices.push_back(i * 6 + 3);
		outMesh.Indices.push_back(i * 6 + 1);
		outMesh.Indices.push_back(i * 6 + 4);
	}
}

Vertex GeometryGenerator::MidVertex(const Vertex& v0, const Vertex& v1)
{
	// 两端顶点位置
	DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&v0.Position);
	DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&v1.Position);
	// 两端顶点颜色
	DirectX::XMVECTOR c0 = DirectX::XMLoadFloat4(&v0.Color);
	DirectX::XMVECTOR c1 = DirectX::XMLoadFloat4(&v1.Color);
	// 两端顶点法线
	DirectX::XMVECTOR n0 = DirectX::XMLoadFloat3(&v0.Normal);
	DirectX::XMVECTOR n1 = DirectX::XMLoadFloat3(&v1.Normal);
	// 两端顶点切线
	DirectX::XMVECTOR tan0 = DirectX::XMLoadFloat3(&v0.TangentU);
	DirectX::XMVECTOR tan1 = DirectX::XMLoadFloat3(&v1.TangentU);
	// 两端顶点纹理坐标
	DirectX::XMVECTOR tex0 = DirectX::XMLoadFloat2(&v0.TexCoord);
	DirectX::XMVECTOR tex1 = DirectX::XMLoadFloat2(&v1.TexCoord);
	// 计算居中顶点的信息
	DirectX::XMVECTOR pos = 0.5f * (p0 + p1);
	DirectX::XMVECTOR color = 0.5f * (c0 + c1);
	DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(0.5f * (n0 + n1));
	DirectX::XMVECTOR tangent = DirectX::XMVector3Normalize(0.5f * (tan0 + tan1));
	DirectX::XMVECTOR texCoord = 0.5f * (tex0 + tex1);
	// 填充中间顶点数据
	Vertex midVert{};
	DirectX::XMStoreFloat3(&midVert.Position, pos);
	DirectX::XMStoreFloat4(&midVert.Color, color);
	DirectX::XMStoreFloat3(&midVert.Normal, normal);
	DirectX::XMStoreFloat3(&midVert.TangentU, tangent);
	DirectX::XMStoreFloat2(&midVert.TexCoord, texCoord);

	return midVert;
}