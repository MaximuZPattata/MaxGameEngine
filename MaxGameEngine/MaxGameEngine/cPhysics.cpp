#include "cPhysics.h"
#include "sPhysicsProperties.h"
#include "OpenGLCommon.h"
#include "cMesh.h"

#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp>

#include<iostream>
#include <vector>
#include <limits.h>
#include <float.h>

cPhysics::cPhysics()
{
}

cPhysics::~cPhysics()
{
}

void cPhysics::setVAOManager(cVAOManager* pTheMeshManager)
{
	this->m_pMeshManager = pTheMeshManager;
	return;
}

glm::vec3 cPhysics::m_ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 bc = c - b;

	float snom = glm::dot(p - a, ab), sdenom = glm::dot(p - b, a - b);

	float tnom = glm::dot(p - a, ac), tdenom = glm::dot(p - c, a - c);

	if (snom <= 0.0f && tnom <= 0.0f)
		return a;

	float unom = glm::dot(p - b, bc), udenom = glm::dot(p - c, b - c);

	if (sdenom <= 0.0f && unom <= 0.0f)
		return b;

	if (tdenom <= 0.0f && udenom <= 0.0f)
		return c;

	glm::vec3 n = glm::cross(b - a, c - a);

	float vc = glm::dot(n, glm::cross(a - p, b - p));

	if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
		return a + snom / (snom + sdenom) * ab;

	float va = glm::dot(n, glm::cross(b - p, c - p));

	if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
		return b + unom / (unom + udenom) * bc;

	float vb = glm::dot(n, glm::cross(c - p, a - p));

	if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
		return a + tnom / (tnom + tdenom) * ac;

	float u = va / (va + vb + vc);
	float v = vb / (va + vb + vc);
	float w = 1.0f - u - v;

	return u * a + v * b + w * c;
}

bool cPhysics::CheckForCollision(cVAOManager* checkMesh, std::string filename, sModelDrawInfo* drawInfo, glm::vec3 spherePosition, float sphereRadius, cMesh* groundMesh, sPhysicsProperties* spherePhysicalProps)
{
	glm::vec3 theMostClosestPoint = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	float closestDistanceSoFar = FLT_MAX;
	unsigned int indexOfClosestTriangle = INT_MAX;

	if (checkMesh->FindDrawInfoByModelName(filename, *drawInfo))
	{
		for (unsigned int index = 0; index != drawInfo->numberOfIndices; index += 3)
		{
			//---------------Calculate vertex position-----------------------------

			glm::vec3 verts[3];

			verts[0].x = drawInfo->pVertices[drawInfo->pIndices[index]].x;
			verts[0].y = drawInfo->pVertices[drawInfo->pIndices[index]].y;
			verts[0].z = drawInfo->pVertices[drawInfo->pIndices[index]].z;

			verts[1].x = drawInfo->pVertices[drawInfo->pIndices[index + 1]].x;
			verts[1].y = drawInfo->pVertices[drawInfo->pIndices[index + 1]].y;
			verts[1].z = drawInfo->pVertices[drawInfo->pIndices[index + 1]].z;

			verts[2].x = drawInfo->pVertices[drawInfo->pIndices[index + 2]].x;
			verts[2].y = drawInfo->pVertices[drawInfo->pIndices[index + 2]].y;
			verts[2].z = drawInfo->pVertices[drawInfo->pIndices[index + 2]].z;

			//----------------Convert to world space------------------------------

			glm::mat4 matModel = glm::mat4(1.0f);

			glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
				glm::vec3(groundMesh->drawPosition.x,
					groundMesh->drawPosition.y,
					groundMesh->drawPosition.z));

			glm::mat4 matRotateX = glm::rotate(glm::mat4(1.0f),
				groundMesh->drawOrientation.x,
				glm::vec3(1.0f, 0.0, 0.0f));

			glm::mat4 matRotateY = glm::rotate(glm::mat4(1.0f),
				groundMesh->drawOrientation.y,
				glm::vec3(0.0f, 1.0, 0.0f));

			glm::mat4 matRotateZ = glm::rotate(glm::mat4(1.0f),
				groundMesh->drawOrientation.z,
				glm::vec3(0.0f, 0.0, 1.0f));

			glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
				glm::vec3(groundMesh->drawScale.x,
					groundMesh->drawScale.y,
					groundMesh->drawScale.z));

			matModel = matModel * matTranslate;

			matModel = matModel * matRotateX;
			matModel = matModel * matRotateY;
			matModel = matModel * matRotateZ;

			matModel = matModel * matScale;

			glm::vec4 vertsWorld[3];

			vertsWorld[0] = (matModel * glm::vec4(verts[0], 1.0f));
			vertsWorld[1] = (matModel * glm::vec4(verts[1], 1.0f));
			vertsWorld[2] = (matModel * glm::vec4(verts[2], 1.0f));

			//-------------Calculate closest point-----------------------------------

			glm::vec3 closestPointToTriangle = m_ClosestPtPointTriangle(spherePosition, vertsWorld[0], vertsWorld[1], vertsWorld[2]);

			float distanceToTriangle = glm::distance(closestPointToTriangle, spherePosition);

			if (distanceToTriangle < closestDistanceSoFar)
			{
				closestDistanceSoFar = distanceToTriangle;

				indexOfClosestTriangle = index;

				spherePhysicalProps->closestTriangleVertices[0] = vertsWorld[0];
				spherePhysicalProps->closestTriangleVertices[1] = vertsWorld[1];
				spherePhysicalProps->closestTriangleVertices[2] = vertsWorld[2];
			}
		}

		//-------------Check for collision------------------------------------------

		if (closestDistanceSoFar < sphereRadius)
		{
			return true;
		}
	}

	return false;
}
