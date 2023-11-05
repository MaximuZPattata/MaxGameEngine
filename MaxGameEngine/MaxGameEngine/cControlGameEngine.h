#pragma once
#include "cMesh.h"
#include "cPhysics.h"
//#include "sPhysicsProperties.h"
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

    std::vector < sPhysicsProperties* > gModelPhysicalProps;

    std::vector< cMesh* > g_vec_pMeshesToDraw;

    std::vector < sModelDrawInfo* > newMeshAdd;

    cShaderManager::cShader vertexShader;

    cShaderManager::cShader fragmentShader;

    void DrawObject(cMesh* pCurrentMesh, glm::mat4 matModel, GLuint shaderProgramID);

    cMesh* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

    sModelDrawInfo* g_pFindModelInfoByFriendlyName(std::string friendlyNameToFind);

    sPhysicsProperties* FindPhysicalModelByName(std::string modelName);

    int InitializeShader();

public:

    //-------------------Camera Controls---------------------------------------------------

    void MoveCameraPosition(float translate_x, float translate_y, float translate_z);

    void MoveCameraTarget(float translate_x, float translate_y, float translate_z);

    glm::vec3 GetCurrentCameraPosition();

    glm::vec3 GetCurrentCameraTarget();

    //-------------------Mesh Controls---------------------------------------------------

    void ChangeColor(std::string modelName, float r, float g, float b);

    void UseDifferentColors(std::string modelName, bool useColor);

    void ScaleModel(std::string modelModel, float scale_value);

    void MoveModel(std::string modelModel, float translate_x, float translate_y, float translate_z);

    glm::vec3 GetModelPosition(std::string modelModel);

    void RotateMeshModel(std::string modelModel, float scalar, float rotate_x, float rotate_y, float rotate_z);

    void TurnVisibilityOn(std::string modelModel);

    void TurnWireframeModeOn(std::string modelModel);

    void TurnMeshLightsOn(std::string modelModel);

    void DeleteMesh(std::string modelName);

    //-------------------Light Controls---------------------------------------------------

    void CreateLight(int lightId, float initial_x, float initial_y, float initial_z);

    void TurnOffLight(int lightId, bool turnOff);

    void PositionLight(int lightId, float translate_x, float translate_y, float translate_z);

    void ChangeLightIntensity(int lightId, float linearAttentuation, float quadraticAttentuation);

    void ChangeLightType(int lightId, float lightType);

    void ChangeLightAngle(int lightId, float innerAngle, float outerAngle);

    void ChangeLightDirection(int lightId, float direction_x, float direction_y, float direction_z);

    void ChangeLightColour(int lightId, float color_r, float color_g, float color_b);

    //------------------Physics Controls---------------------------------------------------

    void CheckForPhysicalModel(double deltaTime, std::string modelName);

    void DoPhysics(sPhysicsProperties* physicsModel, std::string Model2, double deltaTime);

    void ChangeModelPhysicsPosition(std::string modelName, float newPositionX, float newPositionY, float newPositionZ);

    void AddPhysicsToMesh(std::string modelName, float objectRadius);

    void ChangeModelPhysicsVelocity(std::string modelName, glm::vec3 velocityChange);

    void ChangeModelPhysicsAcceleration(std::string modelName, glm::vec3 accelerationChange);

    void CollisionResponse(sPhysicsProperties* physicsModel);

    //-------------------Engine Controls---------------------------------------------------

    void LoadModelsInto3DSpace(std::string filePath, std::string modelName, float initial_x, float initial_y, float initial_z);

    int InitializeGameEngine();

    void RunGameEngine(GLFWwindow* window);
};

