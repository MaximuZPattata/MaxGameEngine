#include "cPhysics.h"


#include <iostream>

// HACK:
//void g_DrawDebugSphere(glm::vec3 position, float scale, glm::vec4 colourRGBA);


void cPhysics::Update(double deltaTime)
{
	for (sPhysicsProperties* pObject : this->m_vec_pPhysicalProps)
	{
		if (pObject->inverse_mass >= 0.0f)
		{
			pObject->oldPosition = pObject->position;

			glm::vec3 deltaVelocityThisFrame = pObject->acceleration * (float)deltaTime;

			pObject->velocity += deltaVelocityThisFrame;

			glm::vec3 deltaPosition = pObject->velocity * (float)deltaTime;

			pObject->position.x += deltaPosition.x;
			pObject->position.y += deltaPosition.y;
			pObject->position.z += deltaPosition.z;
		}
	}

	for (sPhysicsProperties* pObjectA : this->m_vec_pPhysicalProps)
	{
		for (sPhysicsProperties* pObjectB : this->m_vec_pPhysicalProps)
		{
			if (pObjectA == pObjectB)
				continue;

			// We could do the matrix this way...
			//			if ((pObjectA->shapeType == sPhysicsProperties::SPHERE) &&
			//				(pObjectA->shapeType == sPhysicsProperties::PLANE))
			//			{
			//
			//			}// Sphere - Plane
			//
			//			if ((pObjectA->shapeType == sPhysicsProperties::SPHERE) &&
			//				(pObjectA->shapeType == sPhysicsProperties::SPHERE))
			//			{
			//
			//			}// Sphere - Sphere

						// What's the collision? 
			switch (pObjectA->shapeType)
			{
			case sPhysicsProperties::SPHERE:
				switch (pObjectB->shapeType)
				{
				case sPhysicsProperties::SPHERE:
					// Sphere - Sphere
					break;
				case sPhysicsProperties::PLANE:
					// Shpere - Plane
					break;
				case sPhysicsProperties::TRIANGLE:
					// Sphere - Triangle
					break;
				case sPhysicsProperties::AABB:
					// Sphere - AABB
					break;
				case sPhysicsProperties::CAPSULE:
					// Sphere - Capsule
					break;
				case sPhysicsProperties::MESH_OF_TRIANGLES_INDIRECT:
					// Sphere - Mesh triangle (indirect)
					if (this->m_Sphere_TriMeshIndirect_IntersectionTest(pObjectA, pObjectB))
					{
						std::cout << "Hazzah!" << std::endl;
					}
					break;
				case sPhysicsProperties::MESH_OF_TRIANGLES_LOCAL_VERTICES:
					// Shpere - Mesh (local vertices)
					break;
				}//switch (pObjectB->shapeType)
				break;

			case sPhysicsProperties::CAPSULE:
				switch (pObjectB->shapeType)
				{
				case sPhysicsProperties::SPHERE:
					// Capsule - Sphere
					break;
				case sPhysicsProperties::PLANE:
					// Capsule - Plane
					break;
				case sPhysicsProperties::TRIANGLE:
					// Capsule - Triangle
					break;
				case sPhysicsProperties::AABB:
					// Capsule - AABB
					break;
				case sPhysicsProperties::CAPSULE:
					// Capsule - Capsule
					break;
				case sPhysicsProperties::MESH_OF_TRIANGLES_INDIRECT:
					// Capsule - Mesh triangle (indirect)
					break;
				case sPhysicsProperties::MESH_OF_TRIANGLES_LOCAL_VERTICES:
					// Capsule - Mesh (local vertices)
					break;
				}//switch (pObjectB->shapeType)
				break;
			}
		}
	}

	for (sPhysicsProperties* pObject : this->m_vec_pPhysicalProps)
	{
		if (pObject->pTheAssociatedMesh)
		{
			pObject->pTheAssociatedMesh->setDrawPosition(pObject->position);
			pObject->pTheAssociatedMesh->setDrawOrientation(pObject->get_qOrientation());
		}
	}


	// HACK:
	if (!this->m_vecCollisionsThisFrame.empty())
	{
		std::cout << "BREAK ME!" << std::endl;

		std::cout << this->m_vecCollisionsThisFrame.size() << std::endl;
		for (sCollisionEvent col : this->m_vecCollisionsThisFrame)
		{
			std::cout
				<< col.pObjectA->getShapeTypeAsString()
				<< " hit "
				<< col.pObjectB->getShapeTypeAsString()
				<< std::endl;
		}
		std::cout << "******************************************" << std::endl;
		// HACK:
		this->m_vecCollisionsThisFrame.clear();
	}

	return;
}
