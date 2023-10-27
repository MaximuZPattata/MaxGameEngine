#pragma once
#include "cMesh.h"
#include "cPhysics.h"
#include "cLightManager.h"
#include "cLightHelper.h"
#include "cVAOManager.h"
#include "cShaderManager.h"

class cControlGameEngine
{
private:

    int gSelectedLight = 0;

    glm::vec3 g_cameraEye = glm::vec3(0.0, 0.0f, 0.0f);
    glm::vec3 g_cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 g_upVector = glm::vec3(0.0f, 1.0f, 0.0f);

    GLuint shaderProgramID = 0;

    cShaderManager* pShaderThing = NULL;

    cVAOManager* gMeshManager = NULL;

    cPhysics* gPhysics = NULL;

    cLightManager* gTheLights = NULL;

    std::vector< cMesh* > g_vec_pMeshesToDraw;

    std::vector < sModelDrawInfo* > newMeshAdd;

    cShaderManager::cShader vertexShader;

    cShaderManager::cShader fragmentShader;

    void DrawObject(cMesh* pCurrentMesh, glm::mat4 matModel, GLuint shaderProgramID);

    cMesh* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

    int InitializeShader();

public:

    void MoveCameraPosition(float translate_x, float translate_y, float translate_z);

    glm::vec3 GetCurrentCameraPosition();

    void LoadModelsInto3DSpace(std::string filePath, std::string modelName, float initial_x, float initial_y, float initial_z);

    int InitializeGameEngine();

    int RunGameEngine(GLFWwindow* window);
};

