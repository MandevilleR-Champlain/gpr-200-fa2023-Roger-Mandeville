#include "ProcGen.h"

ew::MeshData rm::createPlane(float width, float height, int subdivisions)
{
	ew::MeshData result;

	int totalCols = subdivisions + 1;
	for (int row = 0; row <= subdivisions; row++)
	{
		for (int col = 0; col <= subdivisions; col++)
		{
			//Generate vert
			ew::Vertex currentVert;
			currentVert.pos.x = width * col / float(subdivisions);
			currentVert.pos.y = height * row / float(subdivisions);

			//Generate normals
			currentVert.normal = ew::Vec3(0.f, 0.f, 1.f);

			//Generate UVs
			currentVert.uv = ew::Vec2(width * (float(col) / subdivisions), height * (float(row) / subdivisions));

			result.vertices.push_back(currentVert);

			//Skip last quad
			if (row == subdivisions || col == subdivisions) continue;

			//Generate indicies
			int startIndex = row * totalCols + col;
			//Bottom right
			result.indices.push_back(startIndex);
			result.indices.push_back(startIndex + 1);
			result.indices.push_back(startIndex + totalCols + 1);
			//Top left
			result.indices.push_back(startIndex + totalCols + 1);
			result.indices.push_back(startIndex + totalCols);
			result.indices.push_back(startIndex);
		}
	}

	return result;
}

ew::MeshData rm::createCylidner(float height, float radius, int segments)
{
	ew::MeshData result;

	float topY = height / 2.f;
	ew::Vertex topCenterVert;
	topCenterVert.pos = ew::Vec3(0.f, topY, 0.f);
	topCenterVert.normal = ew::Vec3(0.f, 1.f, 0.f);
	topCenterVert.uv = ew::Vec2(0.5f, 0.5f);
	result.vertices.push_back(topCenterVert);

	float angularStep = 2 * M_PI / float(segments);
	for (int i = 0; i <= segments; i++)
	{
		float currentAngle = i * angularStep;

		ew::Vertex currentVert;
		currentVert.pos = ew::Vec3(cos(currentAngle) * radius, topY, sin(currentAngle) * radius);
		currentVert.normal = ew::Vec3(0.f, 1.f, 0.f);
		currentVert.uv = ew::Vec2(cos(currentAngle) / 2.f + 0.5f, sin(currentAngle) / 2.f + 0.5f);
		result.vertices.push_back(currentVert);

		result.indices.push_back(i);
		result.indices.push_back(0); //Top center
		result.indices.push_back(i + 1);
	}
	for (int i = 0; i <= segments; i++)
	{
		float currentAngle = i * angularStep;

		ew::Vertex currentVert;
		currentVert.pos = ew::Vec3(cos(currentAngle) * radius, topY, sin(currentAngle) * radius);
		currentVert.normal = ew::Normalize(currentVert.pos - topCenterVert.pos);
		currentVert.uv = ew::Vec2(i / float(segments), 1.f);
		result.vertices.push_back(currentVert);
	}

	float bottomY = -topY;
	ew::Vertex bottomCenterVert;
	bottomCenterVert.pos = ew::Vec3(0.f, bottomY, 0.f);
	bottomCenterVert.normal = ew::Vec3(0.f, -1.f, 0.f);
	bottomCenterVert.uv = ew::Vec2(0.5f, 0.5f);
	result.vertices.push_back(bottomCenterVert);

	int bottomCenterIndex = segments * 2 + 3;
	for (int i = 0; i <= segments; i++)
	{
		float currentAngle = i * angularStep;

		ew::Vertex currentVert;
		currentVert.pos = ew::Vec3(cos(currentAngle) * radius, bottomY, sin(currentAngle) * radius);
		currentVert.normal = ew::Vec3(0.f, -1.f, 0.f);
		currentVert.uv = ew::Vec2(cos(currentAngle) / 2.f + 0.5f, sin(currentAngle) / 2.f + 0.5f);
		result.vertices.push_back(currentVert);

		result.indices.push_back(bottomCenterIndex); //Bottom center
		result.indices.push_back(bottomCenterIndex + i);
		result.indices.push_back(bottomCenterIndex + i + 1);
	}
	for (int i = 0; i <= segments; i++)
	{
		float currentAngle = i * angularStep;

		ew::Vertex currentVert;
		currentVert.pos = ew::Vec3(cos(currentAngle) * radius, bottomY, sin(currentAngle) * radius);
		currentVert.normal = ew::Normalize(currentVert.pos - bottomCenterVert.pos);
		currentVert.uv = ew::Vec2(i / float(segments), 0.f);
		result.vertices.push_back(currentVert);
	}

	
	int topVertIndex = segments + 2;
	for (int i = 0; i < segments; i++)
	{
		int startIndex = topVertIndex + i;

		result.indices.push_back(startIndex);
		result.indices.push_back(startIndex + 1);
		result.indices.push_back(startIndex + segments * 2 + 1 + 2);

		result.indices.push_back(startIndex + 1);
		result.indices.push_back(startIndex + segments * 2 + 1 + 3);
		result.indices.push_back(startIndex + segments * 2 + 1 + 2);
	}

	return result;
}

ew::MeshData rm::createSphere(float radius, int segments)
{
	ew::MeshData result;

	float yawStep = 2.f * M_PI / segments; //Theta (pls use descriptive names instead of random letters)
	float pitchStep = M_PI / segments; //Phi
	for (int row = 0; row <= segments; row++)
	{
		float pitch = row * pitchStep;
		for (int col = 0; col <= segments; col++)
		{
			float yaw = col * yawStep;

			ew::Vertex currentVertex;
			currentVertex.pos.x = radius * sin(pitch) * sin(yaw);
			currentVertex.pos.y = radius * cos(pitch);
			currentVertex.pos.z = radius * sin(pitch) * cos(yaw);
			currentVertex.uv = ew::Vec2(col / float(segments), (row + 1) / float(segments + 2));
			currentVertex.normal = ew::Normalize(currentVertex.pos);
			result.vertices.push_back(currentVertex);
		}
	}

	
	int poleVertexIndex = 0;
	int lastCapVertexIndex = segments + 1;
	for (int i = 0; i < segments; i++)
	{
		result.indices.push_back(poleVertexIndex + i);
		result.indices.push_back(lastCapVertexIndex + i);
		result.indices.push_back(lastCapVertexIndex + i + 1);
	}

	
	poleVertexIndex = pow(segments, 2) + segments;
	lastCapVertexIndex = poleVertexIndex - segments - 1;
	for (int i = 0; i < segments; i++)
	{
		result.indices.push_back(lastCapVertexIndex + i);
		result.indices.push_back(poleVertexIndex + i);
		result.indices.push_back(lastCapVertexIndex + i + 1);
	}

	
	for (int row = 1; row < segments - 1; row++)
	{
		for (int col = 0; col < segments; col++)
		{
			int startIndex = row * (segments + 1) + col;

			result.indices.push_back(startIndex);
			result.indices.push_back(startIndex + segments + 1);
			result.indices.push_back(startIndex + 1);

			result.indices.push_back(startIndex + 1);
			result.indices.push_back(startIndex + segments + 1);
			result.indices.push_back(startIndex + segments + 2);
		}
	}

	return result;
}