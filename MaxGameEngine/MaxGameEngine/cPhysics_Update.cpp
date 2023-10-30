//#include "cPhysics.h"
//
//#include <iostream>
//
//void cPhysics::Update(double deltaTime)
//{
//	for (sPhysicsProperties* pObject : this->m_vec_pPhysicalProps)
//	{
//		if (pObject->inverse_mass >= 0.0f)
//		{
//			pObject->oldPosition = pObject->position;
//
//			glm::vec3 deltaVelocityThisFrame = pObject->acceleration * (float)deltaTime;
//
//			pObject->velocity += deltaVelocityThisFrame;
//
//			glm::vec3 deltaPosition = pObject->velocity * (float)deltaTime;
//
//			pObject->position.x += deltaPosition.x;
//			pObject->position.y += deltaPosition.y;
//			pObject->position.z += deltaPosition.z;
//		}
//	}
//
//	for (sPhysicsProperties* pObjectA : this->m_vec_pPhysicalProps)
//	{
//		for (sPhysicsProperties* pObjectB : this->m_vec_pPhysicalProps)
//		{
//			if (pObjectA == pObjectB)
//				continue;
//
//			switch (pObjectA->shapeType)
//			{
//			case sPhysicsProperties::SPHERE:
//				switch (pObjectB->shapeType)
//				{
//				case sPhysicsProperties::SPHERE:
//					break;
//				case sPhysicsProperties::PLANE:
//					break;
//				case sPhysicsProperties::TRIANGLE:
//					break;
//				case sPhysicsProperties::AABB:
//					break;
//				case sPhysicsProperties::CAPSULE:
//					break;
//				case sPhysicsProperties::MESH_OF_TRIANGLES_INDIRECT:
//
//					if (this->m_Sphere_TriMeshIndirect_IntersectionTest(pObjectA, pObjectB))
//					{
//						std::cout << "Hazzah!" << std::endl;
//					}
//					break;
//				case sPhysicsProperties::MESH_OF_TRIANGLES_LOCAL_VERTICES:
//					break;
//				}
//				break;
//
//			case sPhysicsProperties::CAPSULE:
//				switch (pObjectB->shapeType)
//				{
//				case sPhysicsProperties::SPHERE:
//					break;
//				case sPhysicsProperties::PLANE:
//					break;
//				case sPhysicsProperties::TRIANGLE:
//					break;
//				case sPhysicsProperties::AABB:
//					break;
//				case sPhysicsProperties::CAPSULE:
//					break;
//				case sPhysicsProperties::MESH_OF_TRIANGLES_INDIRECT:
//					break;
//				case sPhysicsProperties::MESH_OF_TRIANGLES_LOCAL_VERTICES:
//					break;
//				}
//				break;
//			}
//		}
//	}
//
//	for (sPhysicsProperties* pObject : this->m_vec_pPhysicalProps)
//	{
//		if (pObject->pTheAssociatedMesh)
//		{
//			pObject->pTheAssociatedMesh->setDrawPosition(pObject->position);
//			pObject->pTheAssociatedMesh->setDrawOrientation(pObject->get_qOrientation());
//		}
//	}
//
//	if (!this->m_vecCollisionsThisFrame.empty())
//	{
//		std::cout << "BREAK ME!" << std::endl;
//
//		std::cout << this->m_vecCollisionsThisFrame.size() << std::endl;
//		for (sCollisionEvent col : this->m_vecCollisionsThisFrame)
//		{
//			std::cout
//				<< col.pObjectA->getShapeTypeAsString()
//				<< " hit "
//				<< col.pObjectB->getShapeTypeAsString()
//				<< std::endl;
//		}
//		std::cout << "******************************************" << std::endl;
//
//		this->m_vecCollisionsThisFrame.clear();
//	}
//
//	return;
//}
