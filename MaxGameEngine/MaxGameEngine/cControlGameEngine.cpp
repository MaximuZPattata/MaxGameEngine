#include "cControlGameEngine.h"
#include "cVAOManager.h"
#include "cShaderManager.h"
#include "GLWF_CallBacks.h"
#include "cMesh.h"

#include <iostream>
#include <vector>
#include <sstream>

float getRandomFloat(float a, float b) {
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

//-------------------------------------------------Private Functions-----------------------------------------------------------------------

cMesh* cControlGameEngine::g_pFindMeshByFriendlyName(std::string friendlyNameToFind)
{
    for (unsigned int index = 0; index != TotalMeshList.size(); index++)
    {
        if (TotalMeshList[index]->friendlyName == friendlyNameToFind)
        {
            return TotalMeshList[index];
        }
    }

    std::cout << "Cannot find mesh model for the name provided : " << friendlyNameToFind << std::endl;

    return NULL;
}

sModelDrawInfo* cControlGameEngine::g_pFindModelInfoByFriendlyName(std::string friendlyNameToFind)
{
    for (unsigned int index = 0; index != MeshDrawInfoList.size(); index++)
    {
        if (MeshDrawInfoList[index]->friendlyName == friendlyNameToFind)
        {
            return MeshDrawInfoList[index];
        }
    }

    std::cout << "Cannot find model info for the name provided : " << friendlyNameToFind << std::endl;

    return NULL;
}

sPhysicsProperties* cControlGameEngine::FindPhysicalModelByName(std::string modelName)
{
    for (unsigned int index = 0; index != PhysicsModelList.size(); index++)
    {
        if (PhysicsModelList[index]->modelName == modelName)
        {
            return PhysicsModelList[index];
        }
    }

    std::cout << "Cannot find physical mesh model for the name provided : " << modelName << std::endl;

    return NULL;
}

void cControlGameEngine::DrawObject(cMesh* pCurrentMesh, glm::mat4 matModelParent, GLuint shaderProgramID)
{
    //--------------------------Calculate Matrix Model Transformation--------------------------------

    glm::mat4 matModel = matModelParent;

    glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
        glm::vec3(pCurrentMesh->drawPosition.x, pCurrentMesh->drawPosition.y, pCurrentMesh->drawPosition.z));

    glm::mat4 matRotation = glm::mat4(pCurrentMesh->get_qOrientation());

    glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
        glm::vec3(pCurrentMesh->drawScale.x, pCurrentMesh->drawScale.y, pCurrentMesh->drawScale.z));

    matModel = matModel * matTranslate;

    matModel = matModel * matRotation;

    matModel = matModel * matScale;

    //-------------------------Get Model Info--------------------------------------------------------

    GLint matModel_UL = glGetUniformLocation(shaderProgramID, "matModel");
    glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(matModel));

    glm::mat4 matModel_InverseTranspose = glm::inverse(glm::transpose(matModel));

    GLint matModel_IT_UL = glGetUniformLocation(shaderProgramID, "matModel_IT");
    glUniformMatrix4fv(matModel_IT_UL, 1, GL_FALSE, glm::value_ptr(matModel_InverseTranspose));

    // ---------------------Check Light and Wireframe-------------------------------------------------

    GLint bDoNotLight_UL = glGetUniformLocation(shaderProgramID, "bDoNotLight");

    if (pCurrentMesh->bIsWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (pCurrentMesh->bDoNotLight)
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_TRUE);
    else
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_FALSE);

    //-------------------------Get Debug Color from Shader----------------------------------------

    GLint bUseManualColour_UL = glGetUniformLocation(shaderProgramID, "bUseManualColour");

    if (pCurrentMesh->bUseManualColours)
    {
        glUniform1f(bUseManualColour_UL, (GLfloat)GL_TRUE);

        GLint manualColourRGBA_UL = glGetUniformLocation(shaderProgramID, "manualColourRGBA");
        glUniform4f(manualColourRGBA_UL,
            pCurrentMesh->wholeObjectManualColourRGBA.r,
            pCurrentMesh->wholeObjectManualColourRGBA.g,
            pCurrentMesh->wholeObjectManualColourRGBA.b,
            pCurrentMesh->wholeObjectManualColourRGBA.a);
    }
    else
        glUniform1f(bUseManualColour_UL, (GLfloat)GL_FALSE);

    //-------------------------Find Model Info and Draw----------------------------------------

    sModelDrawInfo modelInfo;

    if (mVAOManager->FindDrawInfoByModelName(pCurrentMesh->friendlyName, modelInfo))
    {
        // Found it!!!

        glBindVertexArray(modelInfo.VAO_ID);
        glDrawElements(GL_TRIANGLES,
            modelInfo.numberOfIndices,
            GL_UNSIGNED_INT,
            0);
        glBindVertexArray(0);

    }

    //-------------------------Remove Scaling----------------------------------------

    glm::mat4 matRemoveScaling = glm::scale(glm::mat4(1.0f),
        glm::vec3(1.0f / pCurrentMesh->drawScale.x, 1.0f / pCurrentMesh->drawScale.y, 1.0f / pCurrentMesh->drawScale.z));

    matModel = matModel * matRemoveScaling;


    return;
}

int cControlGameEngine::InitializeShader()
{
    mShaderManager = new cShaderManager();

    mShaderManager->setBasePath("Assets/Shaders");

    vertexShader.fileName = "vertexShader01.glsl";

    fragmentShader.fileName = "fragmentShader01.glsl";

    if (!mShaderManager->createProgramFromFile("shader01", vertexShader, fragmentShader))
    {
        std::cout << "Error: Couldn't compile or link:" << std::endl;
        std::cout << mShaderManager->getLastError();
        return -1;
    }

    shaderProgramID = mShaderManager->getIDFromFriendlyName("shader01");

    return 0;
}

//---------------------------------------------------Public Functions-----------------------------------------------------------------------

//--------------------------------------Camera Controls----------------------------------------------------------------

void cControlGameEngine::MoveCameraPosition(float translate_x, float translate_y, float translate_z)
{
    cameraEye = glm::vec3(translate_x, translate_y, translate_z);
}

void cControlGameEngine::MoveCameraTarget(float translate_x, float translate_y, float translate_z)
{
    cameraTarget = glm::vec3(translate_x, translate_y, translate_z);
}

glm::vec3 cControlGameEngine::GetCurrentCameraPosition()
{
    return cameraEye;
}

glm::vec3 cControlGameEngine::GetCurrentCameraTarget()
{
    return cameraTarget;
}

//--------------------------------------Mesh Controls-----------------------------------------------------------------

void cControlGameEngine::ChangeColor(std::string modelName, float r, float g, float b)
{
    cMesh* meshToBeScaled = g_pFindMeshByFriendlyName(modelName);

    meshToBeScaled->wholeObjectManualColourRGBA = glm::vec4(r, g, b, 1.0f);
}

void cControlGameEngine::UseDifferentColors(std::string modelName, bool useColor)
{
    cMesh* meshToBeScaled = g_pFindMeshByFriendlyName(modelName);

    if (useColor)
        meshToBeScaled->bUseManualColours = true;
    else
        meshToBeScaled->bUseManualColours = false;
}

void cControlGameEngine::ScaleModel(std::string modelName, float scale_value)
{
    cMesh* meshToBeScaled = g_pFindMeshByFriendlyName(modelName);

    meshToBeScaled->setUniformDrawScale(scale_value);
}

void cControlGameEngine::MoveModel(std::string modelName, float translate_x, float translate_y, float translate_z)
{
    cMesh* meshToBeTranslated = g_pFindMeshByFriendlyName(modelName);

    const glm::vec3& position = glm::vec3(translate_x, translate_y, translate_z);

    meshToBeTranslated->setDrawPosition(position);
}

glm::vec3 cControlGameEngine::GetModelPosition(std::string modelName)
{
    cMesh* meshPosition = g_pFindMeshByFriendlyName(modelName);

    return meshPosition->getDrawPosition();
}

float cControlGameEngine::GetModelScaleValue(std::string modelName)
{
    cMesh* meshScaleValue = g_pFindMeshByFriendlyName(modelName);

    return meshScaleValue->drawScale.x;
}

void cControlGameEngine::RotateMeshModel(std::string modelName, float angleRadians, float rotate_x, float rotate_y, float rotate_z)
{
    cMesh* meshToBeRotated = g_pFindMeshByFriendlyName(modelName);

    glm::quat rotation = glm::quat(angleRadians, rotate_x, rotate_y, rotate_z);

    meshToBeRotated->setDrawOrientation(rotation);

    //meshToBeRotated->setRotationFromEuler(rotation);
}

void cControlGameEngine::TurnVisibilityOn(std::string modelName)
{
    cMesh* meshVisibility = g_pFindMeshByFriendlyName(modelName);

    if (meshVisibility->bIsVisible != true)
        meshVisibility->bIsVisible = true;
    else
        meshVisibility->bIsVisible = false;
}

void cControlGameEngine::TurnWireframeModeOn(std::string modelName)
{
    cMesh* meshWireframe = g_pFindMeshByFriendlyName(modelName);

    if (meshWireframe->bIsWireframe == true)
        meshWireframe->bIsWireframe = false;
    else
        meshWireframe->bIsWireframe = true;
}

void cControlGameEngine::TurnMeshLightsOn(std::string modelName)
{
    cMesh* meshLights = g_pFindMeshByFriendlyName(modelName);

    if (meshLights->bDoNotLight == true)
        meshLights->bDoNotLight = false;
    else
        meshLights->bDoNotLight = true;
}

void cControlGameEngine::DeleteMesh(std::string modelName)
{
    cMesh* meshModel = g_pFindMeshByFriendlyName(modelName);

    sPhysicsProperties* physicalModel = FindPhysicalModelByName(modelName);

    sModelDrawInfo* modelInfo = g_pFindModelInfoByFriendlyName(modelName);

    if (meshModel != NULL)
        TotalMeshList.erase(std::remove(TotalMeshList.begin(), TotalMeshList.end(), meshModel), TotalMeshList.end());

    if (physicalModel != NULL)
        PhysicsModelList.erase(std::remove(PhysicsModelList.begin(), PhysicsModelList.end(), physicalModel), PhysicsModelList.end());

    if (modelInfo != NULL)
        MeshDrawInfoList.erase(std::remove(MeshDrawInfoList.begin(), MeshDrawInfoList.end(), modelInfo), MeshDrawInfoList.end());
}

cMesh* cControlGameEngine::ShiftToNextMeshInList()
{
    cMesh* existingMeshModel = TotalMeshList[meshListIndex];

    meshListIndex++;

    if (meshListIndex == TotalMeshList.size())
        meshListIndex = 0;

    return existingMeshModel;
}

cMesh* cControlGameEngine::ShiftToPreviousMeshInList()
{
    cMesh* existingMeshModel = TotalMeshList[meshListIndex];

    meshListIndex--;

    if (meshListIndex < 0)
        meshListIndex = TotalMeshList.size() - 1;

    return existingMeshModel;
}

cMesh* cControlGameEngine::GetCurrentModelSelected()
{
    return TotalMeshList[meshListIndex];
}

void cControlGameEngine::ShiftToNextLightInList()
{
    lightListIndex++;

    if (lightListIndex > 10)
        lightListIndex = 0;
}

int cControlGameEngine::GetCurrentLightSelected()
{
    return lightListIndex;
}

//--------------------------------------Lights Controls-----------------------------------------------------------------

void cControlGameEngine::CreateLight(int lightId, float initial_x, float initial_y, float initial_z)
{
    if (lightId > 15)
    {
        std::cout << "Light Id is more than 15 ! Cannot create light !" << std::endl;
        return;
    }
    std::cout << "Light : " << lightId << " Created !" << std::endl;

    mLightManager->SetUniformLocations(shaderProgramID, lightId);

    mLightManager->theLights[lightId].param2.x = 1.0f; // Turn on

    mLightManager->theLights[lightId].param1.x = 2.0f;   // 0 = point light , 1 = spot light , 2 = directional light

    mLightManager->theLights[lightId].param1.y = 50.0f; // inner angle

    mLightManager->theLights[lightId].param1.z = 50.0f; // outer angle

    mLightManager->theLights[lightId].position.x = initial_x;

    mLightManager->theLights[lightId].position.y = initial_y;

    mLightManager->theLights[lightId].position.z = initial_z;

    mLightManager->theLights[lightId].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);

    mLightManager->theLights[lightId].atten.x = 0.0f;        // Constant attenuation

    mLightManager->theLights[lightId].atten.y = 0.1f;        // Linear attenuation

    mLightManager->theLights[lightId].atten.z = 0.0f;        // Quadratic attenuation
}

void cControlGameEngine::TurnOffLight(int lightId, bool turnOff)
{
    if (turnOff)
        mLightManager->theLights[lightId].param2.x = 0.0f;
    else
        mLightManager->theLights[lightId].param2.x = 1.0f;
}

void cControlGameEngine::PositionLight(int lightId, float translate_x, float translate_y, float translate_z)
{
    mLightManager->theLights[lightId].position.x = translate_x;

    mLightManager->theLights[lightId].position.y = translate_y;

    mLightManager->theLights[lightId].position.z = translate_z;
}

void cControlGameEngine::ChangeLightIntensity(int lightId, float linearAttentuation, float quadraticAttentuation)
{
    mLightManager->theLights[lightId].atten.y = linearAttentuation;

    mLightManager->theLights[lightId].atten.z = quadraticAttentuation;
}

void cControlGameEngine::ChangeLightType(int lightId, float lightType)
{
    mLightManager->theLights[lightId].param1.x = lightType;
}

void cControlGameEngine::ChangeLightAngle(int lightId, float innerAngle, float outerAngle)
{
    mLightManager->theLights[lightId].param1.y = innerAngle; // inner angle

    mLightManager->theLights[lightId].param1.z = outerAngle; // outer angle
}

void cControlGameEngine::ChangeLightDirection(int lightId, float direction_x, float direction_y, float direction_z)
{
    mLightManager->theLights[lightId].direction = glm::vec4(direction_x, direction_y, direction_z, 1.0f);

}

void cControlGameEngine::ChangeLightColour(int lightId, float color_r, float color_g, float color_b)
{
    mLightManager->theLights[lightId].diffuse = glm::vec4(color_r, color_g, color_b, 1.0f);
}

glm::vec3 cControlGameEngine::GetLightPosition(int lightId)
{
    return mLightManager->theLights[lightId].position;
}

glm::vec3 cControlGameEngine::GetLightDirection(int lightId)
{
    return mLightManager->theLights[lightId].direction;
}

float cControlGameEngine::GetLightLinearAttenuation(int lightId)
{
    return mLightManager->theLights[lightId].atten.y;
}

float cControlGameEngine::GetLightQuadraticAttenuation(int lightId)
{
    return mLightManager->theLights[lightId].atten.z;
}

float cControlGameEngine::GetLightType(int lightId)
{
    return mLightManager->theLights[lightId].param1.x;
}

float cControlGameEngine::GetLightInnerAngle(int lightId)
{
    return mLightManager->theLights[lightId].param1.y;
}

float cControlGameEngine::GetLightOuterAngle(int lightId)
{
    return mLightManager->theLights[lightId].param1.z;
}

glm::vec3 cControlGameEngine::GetLightColor(int lightId)
{
    return mLightManager->theLights[lightId].diffuse;
}

float cControlGameEngine::IsLightOn(int lightId)
{
    return mLightManager->theLights[lightId].param2.x;
}

//--------------------------------------Physics Controls---------------------------------------------------------------

void  cControlGameEngine::MakePhysicsHappen()
{
    for (int physicalModelCount = 0; physicalModelCount < PhysicsModelList.size(); physicalModelCount++)
    {
        if (PhysicsModelList[physicalModelCount]->physicsMeshType == "Sphere")
        {
            sPhysicsProperties* spherePhysicalModel = FindPhysicalModelByName(PhysicsModelList[physicalModelCount]->modelName);

            if (spherePhysicalModel != NULL)
            {
                for (int anotherModelCount = 0; anotherModelCount < PhysicsModelList.size(); anotherModelCount++)
                {
                    if (anotherModelCount == physicalModelCount)
                        continue;
                    else
                    {
                        if (PhysicsModelList[anotherModelCount]->physicsMeshType == "Plane")
                        {
                            DoPhysics(spherePhysicalModel, PhysicsModelList[anotherModelCount]->modelName);
                        }
                    }
                }
               
            }
        }
    }
}

void cControlGameEngine::DoPhysics(sPhysicsProperties* physicsModel, std::string Model2)
{
    //---------------------Calculate acceleration & velocity-------------------------------

    glm::vec3 velocityChange = (physicsModel->sphereProps->acceleration + getRandomFloat(0.2, 0.5)) * (float)deltaTime; //Change later

    physicsModel->sphereProps->velocity += velocityChange;

    glm::vec3 positionChange = physicsModel->sphereProps->velocity * (float)deltaTime;

    physicsModel->position.x += positionChange.x;
    physicsModel->position.y += positionChange.y;
    physicsModel->position.z += positionChange.z;

    //---------------------Set sphere's position based on new velocity--------------------

    cMesh* sphereMesh = g_pFindMeshByFriendlyName(physicsModel->modelName);

    sphereMesh->setDrawPosition(physicsModel->position);

    //---------------------Get second model's position--------------------------------------------

    cMesh* model2Mesh = g_pFindMeshByFriendlyName(Model2);

    model2Mesh->getDrawPosition();

    sModelDrawInfo* modelInfo = g_pFindModelInfoByFriendlyName(Model2);

    //----------------------Check for Collision---------------------------------------------------

    bool result = false;

    /*if (physicsModel->position.x < model2Mesh->drawPosition.x + 70.0f && physicsModel->position.x > model2Mesh->drawPosition.x - 70.0f &&
        physicsModel->position.y < model2Mesh->drawPosition.y + 70.0f && physicsModel->position.y > model2Mesh->drawPosition.y - 70.0f &&
        physicsModel->position.z < model2Mesh->drawPosition.z + 200.0f && physicsModel->position.z > model2Mesh->drawPosition.z - 200.0f)
    {*/
        result = mPhysicsManager->CheckForCollision(mVAOManager, modelInfo->friendlyName, modelInfo, physicsModel->position, physicsModel->sphereProps->radius, model2Mesh, physicsModel);
    //}

    if (result == true)
    {
        CollisionResponse(physicsModel);
    }
}

void cControlGameEngine::CollisionResponse(sPhysicsProperties* physicsModel)
{
    glm::vec3 sphereDirection = physicsModel->sphereProps->velocity;
    sphereDirection = glm::normalize(sphereDirection);

    glm::vec3 edgeA = physicsModel->sphereProps->closestTriangleVertices[1] - physicsModel->sphereProps->closestTriangleVertices[0];
    glm::vec3 edgeB = physicsModel->sphereProps->closestTriangleVertices[2] - physicsModel->sphereProps->closestTriangleVertices[0];

    glm::vec3 triNormal = glm::normalize(glm::cross(edgeA, edgeB));

    glm::vec3 reflectionVector = glm::reflect(sphereDirection, triNormal);

    float sphereSpeed = glm::length(physicsModel->sphereProps->velocity);

    glm::vec3 newVelocity = reflectionVector * sphereSpeed;

    physicsModel->sphereProps->velocity = newVelocity;

    //std::cout << "Hit!!" << std::endl;

    //----------------Find Centroid of triangle-------------------------

    //float triCentreX = (closestTriangleVertices[0].x + closestTriangleVertices[1].x + closestTriangleVertices[2].x) / 3;
    //float triCentreY = (closestTriangleVertices[0].y + closestTriangleVertices[1].y + closestTriangleVertices[2].y) / 3;
    //float triCentreZ = (closestTriangleVertices[0].z + closestTriangleVertices[1].z + closestTriangleVertices[2].z) / 3;

    //spherePhysicalProps->collisionPoint.x = triCentreX;
    //spherePhysicalProps->collisionPoint.y = triCentreY;
    //spherePhysicalProps->collisionPoint.z = triCentreZ;

}

void cControlGameEngine::ChangeModelPhysicsPosition(std::string modelName, float newPositionX, float newPositionY, float newPositionZ)
{
    sPhysicsProperties* physicalModelFound = FindPhysicalModelByName(modelName);

    physicalModelFound->position.x = newPositionX;
    physicalModelFound->position.y = newPositionY;
    physicalModelFound->position.z = newPositionZ;
}

void cControlGameEngine::AddSpherePhysicsToMesh(std::string modelName, std::string physicsMeshType, float objectRadius)
{
    sPhysicsProperties* newPhysicsModel = new sPhysicsProperties(physicsMeshType);

    cMesh* meshDetails = g_pFindMeshByFriendlyName(modelName);

    glm::vec3 modelPosition = meshDetails->getDrawPosition();

    newPhysicsModel->physicsMeshType = physicsMeshType;

    newPhysicsModel->modelName = modelName;

    newPhysicsModel->sphereProps->radius = objectRadius;

    newPhysicsModel->position = modelPosition;

    PhysicsModelList.push_back(newPhysicsModel);
}

void cControlGameEngine::AddPlanePhysicsToMesh(std::string modelName, std::string physicsMeshType)
{
    sPhysicsProperties* newPhysicsModel = new sPhysicsProperties(physicsMeshType);

    cMesh* meshDetails = g_pFindMeshByFriendlyName(modelName);

    glm::vec3 modelPosition = meshDetails->getDrawPosition();

    newPhysicsModel->physicsMeshType = physicsMeshType;

    newPhysicsModel->modelName = modelName;

    newPhysicsModel->position = modelPosition;

    PhysicsModelList.push_back(newPhysicsModel);
}

void cControlGameEngine::ChangeModelPhysicsVelocity(std::string modelName, glm::vec3 velocityChange)
{
    sPhysicsProperties* physicalModelFound = FindPhysicalModelByName(modelName);

    physicalModelFound->sphereProps->velocity = velocityChange;
}

void cControlGameEngine::ChangeModelPhysicsAcceleration(std::string modelName, glm::vec3 accelerationChange)
{
    sPhysicsProperties* physicalModelFound = FindPhysicalModelByName(modelName);

    physicalModelFound->sphereProps->acceleration = accelerationChange;
}

//--------------------------------------Engine Controls-----------------------------------------------------------------

void cControlGameEngine::LoadModelsInto3DSpace(std::string filePath, std::string modelName, float initial_x, float initial_y, float initial_z)
{
    sModelDrawInfo* newModel = new sModelDrawInfo;

    cMesh* newMesh = new cMesh();

    bool result = mVAOManager->LoadModelIntoVAO(modelName, filePath, *newModel, shaderProgramID);

    if (!result)
    {
        std::cout << "Cannot load model - " << modelName << std::endl;
        return;
    }

    MeshDrawInfoList.push_back(newModel);

    newMesh->meshName = filePath;

    newMesh->friendlyName = modelName;

    newMesh->drawPosition = glm::vec3(initial_x, initial_y, initial_z);

    std::cout << "Loaded: " << newMesh->friendlyName << " | Vertices : " << newModel->numberOfVertices << std::endl;

    TotalMeshList.push_back(newMesh);
}

int cControlGameEngine::InitializeGameEngine()
{
    //-------------------------------------Shader Initialize----------------------------------------------------------------

    int result = InitializeShader();

    if (result != 0)
        return -1;

    //-------------------------------------VAO Initialize---------------------------------------------------------------------

    mVAOManager = new cVAOManager();

    mVAOManager->setBasePath("Assets/Models");

    mPhysicsManager = new cPhysics();

    mPhysicsManager->setVAOManager(mVAOManager);

    //------------------------------------Lights Initialize-----------------------------------------------------------------------

    mLightManager = new cLightManager();

    return 0;
}

void cControlGameEngine::RunGameEngine(GLFWwindow* window)
{
    float ratio;
    int width, height;

    glUseProgram(shaderProgramID);

    glfwGetFramebufferSize(window, &width, &height);

    ratio = width / (float)height;

    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    glCullFace(GL_BACK);

    //---------------------------Light Values Update----------------------------------------

    mLightManager->UpdateUniformValues(shaderProgramID);

    //---------------------------Camera Values----------------------------------------------

    GLint eyeLocation_UL = glGetUniformLocation(shaderProgramID, "eyeLocation");
    glUniform4f(eyeLocation_UL,
        cameraEye.x, cameraEye.y, cameraEye.z, 1.0f);

    glm::mat4 matProjection = glm::perspective(0.6f, ratio, 0.1f, 1000.0f);

    glm::mat4 matView = glm::lookAt(cameraEye, cameraEye + cameraTarget, upVector);

    GLint matProjection_UL = glGetUniformLocation(shaderProgramID, "matProjection");
    glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

    GLint matView_UL = glGetUniformLocation(shaderProgramID, "matView");
    glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

    //----------------------------Draw all the objects--------------------------------------

    for (unsigned int index = 0; index != TotalMeshList.size(); index++)
    {
        cMesh* pCurrentMesh = TotalMeshList[index];

        if (pCurrentMesh->bIsVisible)
        {
            glm::mat4 matModel = glm::mat4(1.0f);

            DrawObject(pCurrentMesh, matModel, shaderProgramID);
        }
    }

    //----------------------------Title Screen Values---------------------------------------------

    std::stringstream ssTitle;

    //-----------------Light values displayed - Commented------------------------

   /* int lightId = lightListIndex;

    glm::vec3 lightPosition = GetLightPosition(lightId);
    float lightLinearAtten = GetLightLinearAttenuation(lightId);
    float lightQuadraticAtten = GetLightQuadraticAttenuation(lightId);
    glm::vec3 lightDirection = GetLightDirection(lightId);
    float lightType = GetLightType(lightId);
    float lightInnerAngle = GetLightInnerAngle(lightId);
    float lightOuterAngle = GetLightOuterAngle(lightId);

    ssTitle << "Light Id : "
        << lightId << " | Light Position : ("
        << lightPosition.x << ", "
        << lightPosition.y << ", "
        << lightPosition.z << ") | Direction : ("
        << lightDirection.x << ", "
        << lightDirection.y << ", "
        << lightDirection.z << ") |  Linear Atten : "
        << lightLinearAtten << " | Quadratic Atten : "
        << lightQuadraticAtten << " | Type : "
        << lightType << " | Inner Angle : "
        << lightInnerAngle << " | Outer Angle : "
        << lightOuterAngle;*/

        //----------------Cam and Model values displayed-----------------------------

    cMesh* meshObj = GetCurrentModelSelected();

    ssTitle << "Camera Eye(x, y, z) : ("
        << cameraEye.x << ", "
        << cameraEye.y << ", "
        << cameraEye.z << ") | "
        << "Camera Target(x,y,z): ("
        << cameraTarget.x << ", "
        << cameraTarget.y << ", "
        << cameraTarget.z << ") | Yaw/Pitch : ("
        << yaw << ", " << pitch << ") | ModelName : "
        << meshObj->friendlyName << " | ModelPos : ("
        << meshObj->drawPosition.x << ", "
        << meshObj->drawPosition.y << ", "
        << meshObj->drawPosition.z << ") | ModelScaleVal : "
        << meshObj->drawScale.x;

    std::string theTitle = ssTitle.str();

    glfwSwapBuffers(window);

    glfwPollEvents();

    glfwSetWindowTitle(window, theTitle.c_str());
}
