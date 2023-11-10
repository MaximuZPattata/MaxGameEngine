#ifndef _cPhysics_HG_
#define _cPhysics_HG_

#include "sPhysicsProperties.h"
#include "cMesh.h"
#include "cVAOManager.h"	
#include <vector>

class cPhysics
{
public:

	cPhysics();

	~cPhysics();

	void setVAOManager(cVAOManager* pTheMeshManager);

	bool CheckForPlaneCollision(cVAOManager* modelVAO, std::string filename, sModelDrawInfo* drawInfo, cMesh* groundMesh, sPhysicsProperties* spherePhysicalProps);

	bool CheckForSphereCollision(sPhysicsProperties* sphere1PhysicalProps, sPhysicsProperties* sphere2PhysicalProps);

private:

	cVAOManager* m_pMeshManager = NULL;

	std::vector< sPhysicsProperties* > m_vec_pPhysicalProps;

	glm::vec3 m_ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);

};

#endif 