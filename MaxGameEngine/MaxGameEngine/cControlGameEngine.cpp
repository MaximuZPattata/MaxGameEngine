#include "cControlGameEngine.h"
#include "cVAOManager.h"
#include "cShaderManager.h"
#include "GLWF_CallBacks.h"
#include "cMesh.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <sstream>

cMesh* cControlGameEngine::g_pFindMeshByFriendlyName(std::string friendlyNameToFind)
{
    for (unsigned int index = 0; index != g_vec_pMeshesToDraw.size(); index++)
    {
        if (g_vec_pMeshesToDraw[index]->friendlyName == friendlyNameToFind)
        {
            return g_vec_pMeshesToDraw[index];
        }
    }
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

    GLint bUseDebugColour_UL = glGetUniformLocation(shaderProgramID, "bUseDebugColour");

    if (pCurrentMesh->bUseDebugColours)
    {
        glUniform1f(bUseDebugColour_UL, (GLfloat)GL_TRUE);

        GLint debugColourRGBA_UL = glGetUniformLocation(shaderProgramID, "debugColourRGBA");
        glUniform4f(debugColourRGBA_UL,
            pCurrentMesh->wholeObjectDebugColourRGBA.r,
            pCurrentMesh->wholeObjectDebugColourRGBA.g,
            pCurrentMesh->wholeObjectDebugColourRGBA.b,
            pCurrentMesh->wholeObjectDebugColourRGBA.a);
    }
    else
        glUniform1f(bUseDebugColour_UL, (GLfloat)GL_FALSE);

    //-------------------------Find Model Info and Draw----------------------------------------

    sModelDrawInfo modelInfo;

    if (gMeshManager->FindDrawInfoByModelName(pCurrentMesh->meshName, modelInfo))
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
    pShaderThing = new cShaderManager();

    pShaderThing->setBasePath("Assets/Shaders");

    vertexShader.fileName = "vertexShader01.glsl";

    fragmentShader.fileName = "fragmentShader01.glsl";

    if (!pShaderThing->createProgramFromFile("shader01", vertexShader, fragmentShader))
    {
        std::cout << "Error: Couldn't compile or link:" << std::endl;
        std::cout << pShaderThing->getLastError();
        return -1;
    }

    shaderProgramID = pShaderThing->getIDFromFriendlyName("shader01");

    return 0;
}

//---------------------------------------------------Public Functions-----------------------------------------------------------------------

void cControlGameEngine::MoveCameraPosition(float translate_x, float translate_y, float translate_z)
{
    g_cameraEye = glm::vec3(translate_x, translate_y, translate_z);
}

glm::vec3 cControlGameEngine::GetCurrentCameraPosition()
{
    return g_cameraEye;
}

void cControlGameEngine::LoadModelsInto3DSpace(std::string filePath,
    std::string modelName, float initial_x, float initial_y, float initial_z)
{
    sModelDrawInfo* newModel = new sModelDrawInfo;

    cMesh* newMesh = new cMesh();

    gMeshManager->LoadModelIntoVAO(filePath, *newModel, shaderProgramID);

    newMeshAdd.push_back(newModel);

    newMesh->meshName = filePath;

    newMesh->friendlyName = modelName;

    newMesh->drawPosition = glm::vec3(initial_x, initial_y, initial_z);

    std::cout << "Loaded: " << newMesh->friendlyName << " | Vertices : " << newModel->numberOfVertices << std::endl;

    g_vec_pMeshesToDraw.push_back(newMesh);
}

int cControlGameEngine::InitializeGameEngine()
{
    //-------------------------------------Shader Initialize----------------------------------------------------------------

    int result = InitializeShader();

    if (result != 0)
        return -1;

    //-------------------------------------VAO Initialize---------------------------------------------------------------------

    gMeshManager = new cVAOManager();

    gMeshManager->setBasePath("Assets/Models");

    gPhysics = new cPhysics();

    gPhysics->setVAOManager(gMeshManager);


    //------------------------------------Lights Initialize-----------------------------------------------------------------------

    gTheLights = new cLightManager();

    gTheLights->SetUniformLocations(shaderProgramID);

    gTheLights->theLights[0].param2.x = 1.0f;

    gTheLights->theLights[0].param1.x = 2.0f;   // 0 = point light , 1 = spot light , 2 = directional light

    gTheLights->theLights[0].param1.y = 15.0f;

    gTheLights->theLights[0].param1.z = 25.0f;

    gTheLights->theLights[0].position.x = -46.0f;

    gTheLights->theLights[0].position.y = 23.0f;

    gTheLights->theLights[0].position.z = -26.0f;

    gTheLights->theLights[0].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);  // x = lightType, y = inner angle, z = outer angle, w = TBD

    gTheLights->theLights[0].atten.x = 0.0f;        // Constant attenuation

    gTheLights->theLights[0].atten.y = 0.01f;        // Linear attenuation

    gTheLights->theLights[0].atten.z = 0.01f;        // Quadratic attenuation

    return 0;
}

int cControlGameEngine::RunGameEngine(GLFWwindow* window)
{


    while (!glfwWindowShouldClose(window))
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

        gTheLights->UpdateUniformValues(shaderProgramID);

        //---------------------------Camera Values----------------------------------------------

        GLint eyeLocation_UL = glGetUniformLocation(shaderProgramID, "eyeLocation");
        glUniform4f(eyeLocation_UL,
            g_cameraEye.x, g_cameraEye.y, g_cameraEye.z, 1.0f);

        glm::mat4 matProjection = glm::perspective(0.6f, ratio, 0.1f, 1000.0f);

        glm::mat4 matView = glm::lookAt(g_cameraEye, g_cameraTarget, g_upVector);

        GLint matProjection_UL = glGetUniformLocation(shaderProgramID, "matProjection");
        glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

        GLint matView_UL = glGetUniformLocation(shaderProgramID, "matView");
        glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

        //---------------------------- Draw all the objects--------------------------------------

        for (unsigned int index = 0; index != g_vec_pMeshesToDraw.size(); index++)
        {
            cMesh* pCurrentMesh = g_vec_pMeshesToDraw[index];

            if (pCurrentMesh->bIsVisible)
            {
                glm::mat4 matModel = glm::mat4(1.0f);

                DrawObject(pCurrentMesh, matModel, shaderProgramID);
            }
        }

        //----------------------------Title Screen Values---------------------------------------------

        std::stringstream ssTitle;

        ssTitle << "Camera (x,y,z): "
            << g_cameraEye.x << ", "
            << g_cameraEye.y << ", "
            << g_cameraEye.z << ") "
            << "Light[" << gSelectedLight << "]: "
            << gTheLights->theLights[gSelectedLight].position.x << ", "
            << gTheLights->theLights[gSelectedLight].position.y << ", "
            << gTheLights->theLights[gSelectedLight].position.z << "  "
            << "const:" << gTheLights->theLights[gSelectedLight].atten.x << " "
            << "linear:" << gTheLights->theLights[gSelectedLight].atten.y << " "
            << "quad:" << gTheLights->theLights[gSelectedLight].atten.z << " "
            << "inner: " << gTheLights->theLights[gSelectedLight].param1.y << " "
            << "outer: " << gTheLights->theLights[gSelectedLight].param1.z << " ";

        std::string theTitle = ssTitle.str();

        glfwSwapBuffers(window);

        glfwPollEvents();

        glfwSetWindowTitle(window, theTitle.c_str());
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    exit(EXIT_SUCCESS);
}
