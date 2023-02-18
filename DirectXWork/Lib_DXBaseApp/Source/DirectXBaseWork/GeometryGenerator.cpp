/*
 * GeometryGenerator.cpp
 * 程序化几何体生成器
 */

#include "stdafx.h"
#include "GeometryGenerator.h"

using namespace DirectX;

GeometryGenerator::MeshData GeometryGenerator::CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount)
{
	GeometryGenerator::MeshData meshData{};
	// 分层层高间隔
	float stackHeight = height / stackCount;
	// 每个分层的半径增量
	float stackRadiusStep = (topRadius - bottomRadius) / stackCount;
	// 分层的环数
	uint32 ringCount = stackCount + 1;
	// 纵向切片的均分夹角
	float dSliceTheta = 2.f * DirectX::XM_PI / sliceCount;
	// 计算每个分层环上的顶点坐标
	for (uint32 i = 0; i < ringCount; ++i)
	{
		// 当前分层环的半径
		float stackRadius = bottomRadius + i * stackRadiusStep;
		// 当前分层Y轴高度
		float stackY = -0.5f * height + i * stackHeight;
		// 计算当前分层上的侧面顶点信息，首尾两个顶点位置重合但纹理坐标不一样，故这里用<=，需要多一个顶点信息
		for (uint32 j = 0; j <= sliceCount; ++j)
		{
			Vertex vertex{};
			float sinValue = sinf(j * dSliceTheta);
			float cosValue = cosf(j * dSliceTheta);
			// 顶点坐标
			vertex.Position = DirectX::XMFLOAT3(stackRadius * cosValue, stackY, stackRadius * sinValue);
			// 顶点纹理坐标
			vertex.TexCoord.x = (float)j / sliceCount;
			vertex.TexCoord.y = 1.f - (float)i / stackCount;
			// 顶点切线
			vertex.TangentU = DirectX::XMFLOAT3(-sinValue, 0.f, cosValue);
			// 通过切线空间TBN计算法线
			float dr = bottomRadius - topRadius;
			DirectX::XMFLOAT3 bitangent(dr * cosValue, -height, dr * sinValue);
			DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&vertex.TangentU);
			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&bitangent);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(T, B));
			DirectX::XMStoreFloat3(&vertex.Normal, N);
			// 填充到顶点集合中
			meshData.Vertices.push_back(vertex);
		}
	}
	// 每个圆环上顶点的数量，第一个顶点和最后一个顶点位置重合，但是两个顶点的纹理坐标不一样，因此视为两个顶点
	int ringVertexCount = sliceCount + 1;
	// 计算柱体侧面顶点索引
	// 第i+1层  B*-----*C
	//           |   / |
	//           | /   |
	// 第i层    A*-----*D
	for (uint32 i = 0; i < ringCount; ++i)
	{
		for (uint32 j = 0; j < sliceCount; ++j)
		{
			// 参看上图，计算四边形ABCD的索引
			uint32 indexA = i * ringVertexCount + j;
			uint32 indexB = (i + 1) * ringVertexCount + j;
			uint32 indexC = (i + 1) * ringVertexCount + j + 1;
			uint32 indexD = i * ringVertexCount + j + 1;
			// ABC构成第一个三角形，顶点顺序顺时针
			meshData.Indices32.push_back(indexA);
			meshData.Indices32.push_back(indexB);
			meshData.Indices32.push_back(indexC);
			// ACD构成第二个三角形，顶点顺序顺时针
			meshData.Indices32.push_back(indexA);
			meshData.Indices32.push_back(indexC);
			meshData.Indices32.push_back(indexD);
		}
	}
	// 构建柱体顶截面网格数据
	BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
	// 构建柱体底截面网格数据
	BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);

	return meshData;
}

void GeometryGenerator::BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, GeometryGenerator::MeshData& meshData)
{
	uint32 beginIndex = (uint32)meshData.Vertices.size();
	float topY = 0.5f * height;
	float dSliceTheta = 2.0f * DirectX::XM_PI / sliceCount;
	// 顶层圆环与顶截面的顶点位置重合，但顶点的纹理坐标和法线不同
	for (uint32 i = 0; i <= sliceCount; ++i)
	{
		float posX = topRadius * cosf(i * dSliceTheta);
		float posZ = topRadius * sinf(i * dSliceTheta);
		// 纹理坐标，按截面半径和柱体高度比例缩放
		float u = posX / height + 0.5f;
		float v = posZ / height + 0.5f;
		meshData.Vertices.push_back(Vertex(posX, topY, posZ, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, u, v));
	}
	// 顶层中心点
	meshData.Vertices.push_back(Vertex(0.f, topY, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.5f, 0.5f));
	// 计算顶截面的三角形索引数据
	uint32 centerIndex = (uint32)meshData.Vertices.size() - 1;
	for (uint32 i = 0; i < sliceCount; ++i)
	{
		meshData.Indices32.push_back(centerIndex);
		meshData.Indices32.push_back(beginIndex + i + 1);
		meshData.Indices32.push_back(beginIndex + i);
	}
}

void GeometryGenerator::BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, GeometryGenerator::MeshData& meshData)
{
	uint32 beginIndex = (uint32)meshData.Vertices.size();
	float bottomY = -0.5f * height;
	float dSliceTheta = 2.0f * DirectX::XM_PI / sliceCount;
	// 顶层圆环与顶截面的顶点位置重合，但顶点的纹理坐标和法线不同
	for (uint32 i = 0; i <= sliceCount; ++i)
	{
		float posX = topRadius * cosf(i * dSliceTheta);
		float posZ = topRadius * sinf(i * dSliceTheta);
		// 纹理坐标，按截面半径和柱体高度比例缩放
		float u = posX / height + 0.5f;
		float v = posZ / height + 0.5f;
		meshData.Vertices.push_back(Vertex(posX, bottomY, posZ, 0.f, -1.f, 0.f, 1.f, 0.f, 0.f, u, v));
	}
	// 顶层中心点
	meshData.Vertices.push_back(Vertex(0.f, bottomY, 0.f, 0.f, -1.f, 0.f, 1.f, 0.f, 0.f, 0.5f, 0.5f));
	// 计算顶截面的三角形索引数据
	uint32 centerIndex = (uint32)meshData.Vertices.size() - 1;
	for (uint32 i = 0; i < sliceCount; ++i)
	{
		meshData.Indices32.push_back(centerIndex);
		meshData.Indices32.push_back(beginIndex + i);
		meshData.Indices32.push_back(beginIndex + i + 1);
	}
}

GeometryGenerator::MeshData GeometryGenerator::CreateCube(float width, float height, float depth, uint32 numSubdivisions)
{
	MeshData meshData{};
	// 立方体中心为局部坐标系原点，左手坐标系
	float halfWidth = width * 0.5f;
	float halfHeight = height * 0.5f;
	float halfDepth = depth * 0.5f;
	// 每个面有4个顶点，位置有重合，但是法线和顶点方向不一致，故需要24个顶点数据
	Vertex vertices[24];
	// 前面
	vertices[0]  = Vertex(-halfWidth, -halfHeight, -halfDepth, 0.f, 0.f, -1.f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[1]  = Vertex(-halfWidth, +halfHeight, -halfDepth, 0.f, 0.f, -1.f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[2]  = Vertex(+halfWidth, +halfHeight, -halfDepth, 0.f, 0.f, -1.f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	vertices[3]  = Vertex(+halfWidth, -halfHeight, -halfDepth, 0.f, 0.f, -1.f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	// 背面
	vertices[4]  = Vertex(-halfWidth, -halfHeight, +halfDepth, 0.f, 0.f, 1.f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	vertices[5]  = Vertex(+halfWidth, -halfHeight, +halfDepth, 0.f, 0.f, 1.f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[6]  = Vertex(+halfWidth, +halfHeight, +halfDepth, 0.f, 0.f, 1.f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[7]  = Vertex(-halfWidth, +halfHeight, +halfDepth, 0.f, 0.f, 1.f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	// 顶面
	vertices[8]  = Vertex(-halfWidth, +halfHeight, -halfDepth, 0.f, 1.f, 0.f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[9]  = Vertex(-halfWidth, +halfHeight, +halfDepth, 0.f, 1.f, 0.f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[10] = Vertex(+halfWidth, +halfHeight, +halfDepth, 0.f, 1.f, 0.f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	vertices[11] = Vertex(+halfWidth, +halfHeight, -halfDepth, 0.f, 1.f, 0.f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	// 底面
	vertices[12] = Vertex(-halfWidth, -halfHeight, -halfDepth, 0.f, -1.f, 0.f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	vertices[13] = Vertex(+halfWidth, -halfHeight, -halfDepth, 0.f, -1.f, 0.f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[14] = Vertex(+halfWidth, -halfHeight, +halfDepth, 0.f, -1.f, 0.f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[15] = Vertex(-halfWidth, -halfHeight, +halfDepth, 0.f, -1.f, 0.f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	// 左面
	vertices[16] = Vertex(-halfWidth, -halfHeight, +halfDepth, -1.f, 0.f, 0.f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	vertices[17] = Vertex(-halfWidth, +halfHeight, +halfDepth, -1.f, 0.f, 0.f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	vertices[18] = Vertex(-halfWidth, +halfHeight, -halfDepth, -1.f, 0.f, 0.f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	vertices[19] = Vertex(-halfWidth, -halfHeight, -halfDepth, -1.f, 0.f, 0.f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	// 右面
	vertices[20] = Vertex(+halfWidth, -halfHeight, -halfDepth, 1.f, 0.f, 0.f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vertices[21] = Vertex(+halfWidth, +halfHeight, -halfDepth, 1.f, 0.f, 0.f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vertices[22] = Vertex(+halfWidth, +halfHeight, +halfDepth, 1.f, 0.f, 0.f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vertices[23] = Vertex(+halfWidth, -halfHeight, +halfDepth, 1.f, 0.f, 0.f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	// 填充到网格顶点数据集合
	meshData.Vertices.assign(&vertices[0], &vertices[24]);
	// 创建索引
	uint32 indices[36];
	// 前面
	indices[0]  = 0;  indices[1]  = 1;  indices[2]  = 2;
	indices[3]  = 0;  indices[4]  = 2;  indices[5]  = 3;
	// 后面
	indices[6]  = 4;  indices[7]  = 5;  indices[8]  = 6;
	indices[9]  = 4;  indices[10] = 6;  indices[11] = 7;
	// 顶面
	indices[12] = 8;  indices[13] = 9;  indices[14] = 10;
	indices[15] = 8;  indices[16] = 10; indices[17] = 11;
	// 底面
	indices[18] = 12; indices[19] = 13; indices[20] = 14;
	indices[21] = 12; indices[22] = 14; indices[23] = 15;
	// 左面
	indices[24] = 16; indices[25] = 17; indices[26] = 18;
	indices[27] = 16; indices[28] = 18; indices[29] = 19;
	// 右面
	indices[30] = 20; indices[31] = 21; indices[32] = 22;
	indices[33] = 20; indices[34] = 22; indices[35] = 23;
	// 填充到网格索引数据集合
	meshData.Indices32.assign(&indices[0], &indices[36]);
	// 细分面
	numSubdivisions = std::min<uint32>(numSubdivisions, 6u);
	for (uint32 i = 0; i < numSubdivisions; ++i)
	{
		SubDivide(meshData);
	}

	return meshData;
}

GeometryGenerator::MeshData GeometryGenerator::CreateSphere(float radius, uint32 sliceCount, uint32 stackCount)
{
	MeshData meshData{};


	return meshData;
}

GeometryGenerator::MeshData GeometryGenerator::CreateGeoSphere(float radius, uint32 numSubdivisions)
{
	MeshData meshData{};
	numSubdivisions = std::min<uint32>(numSubdivisions, 6u);
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
	uint32 k[60] =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	meshData.Vertices.resize(12);
	meshData.Indices32.assign(&k[0], &k[60]);
	for (uint32 i = 0; i < 12; ++i)
	{
		meshData.Vertices[i].Position = pos[i];
	}
	for (uint32 i = 0; i < numSubdivisions; ++i)
	{
		SubDivide(meshData);
	}
	for (uint32 i = 0; i < meshData.Vertices.size(); ++i)
	{
		// Project onto unit sphere.
		XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&meshData.Vertices[i].Position));

		// Project onto sphere.
		XMVECTOR p = radius * n;

		XMStoreFloat3(&meshData.Vertices[i].Position, p);
		XMStoreFloat3(&meshData.Vertices[i].Normal, n);

		// Derive texture coordinates from spherical coordinates.
		float theta = atan2f(meshData.Vertices[i].Position.z, meshData.Vertices[i].Position.x);

		// Put in [0, 2pi].
		if (theta < 0.0f)
			theta += XM_2PI;

		float phi = acosf(meshData.Vertices[i].Position.y / radius);

		meshData.Vertices[i].TexCoord.x = theta / XM_2PI;
		meshData.Vertices[i].TexCoord.y = phi / XM_PI;

		// Partial derivative of P with respect to theta
		meshData.Vertices[i].TangentU.x = -radius * sinf(phi) * sinf(theta);
		meshData.Vertices[i].TangentU.y = 0.0f;
		meshData.Vertices[i].TangentU.z = +radius * sinf(phi) * cosf(theta);

		XMVECTOR T = XMLoadFloat3(&meshData.Vertices[i].TangentU);
		XMStoreFloat3(&meshData.Vertices[i].TangentU, XMVector3Normalize(T));
	}

	return meshData;
}

GeometryGenerator::MeshData GeometryGenerator::CreateGrid(float width, float depth, uint32 row, uint32 col)
{
	MeshData meshData{};
	// 顶点总数
	uint32 vertexCount = row * col;
	// 三角面总数
	uint32 trisCount = (row - 1) * (col - 1) * 2;

	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;
	// 列间隔X
	float dx = width / (col - 1);
	// 行间隔Z
	float dz = depth / (row - 1);
	// 列纹理间隔U
	float du = 1.f / (col - 1);
	// 行纹理间隔V
	float dv = 1.f / (row - 1);
	// 填充网格顶点数据集合
	meshData.Vertices.resize(vertexCount);
	for (uint32 i = 0; i < row; ++i)
	{
		float z = halfDepth - i * dz;
		for (uint32 j = 0; j < col; ++j)
		{
			float x = -halfWidth + j * dx;
			meshData.Vertices[i * col + j].Position = XMFLOAT3(x, 0.f, z);
			meshData.Vertices[i * col + j].Normal = XMFLOAT3(0.f, 1.f, 0.f);
			meshData.Vertices[i * col + j].TangentU = XMFLOAT3(1.f, 0.f, 0.f);
			meshData.Vertices[i * col + j].TexCoord = XMFLOAT2(j * du, i * dv);
		}
	}
	// 填充网格索引数据集合
	meshData.Indices32.resize(trisCount * 3);
	uint32 k = 0;
	for (uint32 i = 0; i < row - 1; ++i)
	{
		for (uint32 j = 0; j < col - 1; ++j)
		{
			meshData.Indices32[k] = i * col + j;
			meshData.Indices32[k + 1] = i * col + j + 1;
			meshData.Indices32[k + 2] = (i + 1) * col + j;

			meshData.Indices32[k + 3] = (i + 1) * col + j;
			meshData.Indices32[k + 4] = i * col + j + 1;
			meshData.Indices32[k + 5] = (i + 1) * col + j + 1;
			// 下一个网格子块
			k += 6;
		}
	}

	return meshData;
}

void GeometryGenerator::SubDivide(GeometryGenerator::MeshData& meshData)
{
	MeshData inputCopy = meshData;
	meshData.Vertices.resize(0);
	meshData.Indices32.resize(0);
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
	uint32 numTris = (uint32)inputCopy.Indices32.size() / 3;
	// 遍历所有三角面
	for (uint32 i = 0; i < numTris; ++i)
	{
		// 获取三角面三个顶点数据
		Vertex v0 = inputCopy.Vertices[inputCopy.Indices32[i * 3 + 0]];
		Vertex v1 = inputCopy.Vertices[inputCopy.Indices32[i * 3 + 1]];
		Vertex v2 = inputCopy.Vertices[inputCopy.Indices32[i * 3 + 2]];
		// 计算三个边的中点
		Vertex m0 = MidVertex(v0, v1);
		Vertex m1 = MidVertex(v1, v2);
		Vertex m2 = MidVertex(v0, v2);
		// 添加细分的顶点和索引
		meshData.Vertices.push_back(v0); // 0
		meshData.Vertices.push_back(v1); // 1
		meshData.Vertices.push_back(v2); // 2
		meshData.Vertices.push_back(m0); // 3
		meshData.Vertices.push_back(m1); // 4
		meshData.Vertices.push_back(m2); // 5
		// 三角形v0m0m2
		meshData.Indices32.push_back(i * 6 + 0);
		meshData.Indices32.push_back(i * 6 + 3);
		meshData.Indices32.push_back(i * 6 + 5);
		// 三角形m0m1m2
		meshData.Indices32.push_back(i * 6 + 3);
		meshData.Indices32.push_back(i * 6 + 4);
		meshData.Indices32.push_back(i * 6 + 5);
		// 三角形m2m1v2
		meshData.Indices32.push_back(i * 6 + 5);
		meshData.Indices32.push_back(i * 6 + 4);
		meshData.Indices32.push_back(i * 6 + 2);
		// 三角形m0v1m1
		meshData.Indices32.push_back(i * 6 + 3);
		meshData.Indices32.push_back(i * 6 + 1);
		meshData.Indices32.push_back(i * 6 + 4);
	}
}

GeometryGenerator::Vertex GeometryGenerator::MidVertex(const Vertex& v0, const Vertex& v1)
{
	// 两端顶点位置
	DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&v0.Position);
	DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&v1.Position);
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
	DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(0.5f * (n0 + n1));
	DirectX::XMVECTOR tangent = DirectX::XMVector3Normalize(0.5f * (tan0 + tan1));
	DirectX::XMVECTOR texCoord = 0.5f * (tex0 + tex1);
	// 填充中间顶点数据
	Vertex midVert{};
	DirectX::XMStoreFloat3(&midVert.Position, pos);
	DirectX::XMStoreFloat3(&midVert.Normal, normal);
	DirectX::XMStoreFloat3(&midVert.TangentU, tangent);
	DirectX::XMStoreFloat2(&midVert.TexCoord, texCoord);

	return midVert;
}