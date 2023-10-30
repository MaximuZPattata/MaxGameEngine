#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "iPhysicsMeshTransformAccess.h"
#include <string>
#include <limits.h>
#include <vector>


struct sPhysicsProperties
{
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	std::string modelName;

	float inverse_mass = 0.0f;

	sPhysicsProperties();
};
