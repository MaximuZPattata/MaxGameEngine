#include <cControlGameEngine.h>
#include "GLWF_CallBacks.h"
#include "cMesh.h"
#include "cJsonReader.h"

#include <iostream>
#include <vector>

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

//---------------------------Global Objects-----------------------------------------------

GLFWwindow* window;
cControlGameEngine gameEngine;
cJsonReader jsonReader;
std::vector<sModelDetailsFromFile> modelDetailsList;
std::vector<sLightDetailsFromFile> lightDetailsList;
sCameraDetailsFromFile camDetails;

int main()
{
    //-----------------------------------Initialize Window--------------------------------------

    int result = 0;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(800, 600, "Simple example", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //------------------------------Input key and Cursor initialize-----------------------------

    glfwSetKeyCallback(window, key_callback);

    glfwSetCursorPosCallback(window, mouse_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    //--------------------------------Initialize Game Engine----------------------------------------

    result = gameEngine.InitializeGameEngine();

    if (result != 0)
        return -1;

    //--------------------------------Loading Models, Lights and initial camera position from Json file---------------------------------------------

    bool jsonresult = jsonReader.ReadScene("SceneDescription.json", modelDetailsList, lightDetailsList, camDetails);

    if (jsonresult)
    {
        std::cout << "File read successfully !" << std::endl;

        std::string modelName;
        int lightId;

        // Loading Models
        for (int index = 0; index < modelDetailsList.size(); index++)
        {
            modelName = modelDetailsList[index].modelName;

            gameEngine.LoadModelsInto3DSpace(modelDetailsList[index].modelFilePath, modelName,
                modelDetailsList[index].modelPosition.x, modelDetailsList[index].modelPosition.y, modelDetailsList[index].modelPosition.z);

            float angleRadians = glm::radians(modelDetailsList[index].modelOrientation.w);

            gameEngine.RotateMeshModel(modelName, angleRadians, modelDetailsList[index].modelOrientation.x,
                modelDetailsList[index].modelOrientation.y, modelDetailsList[index].modelOrientation.z);

            gameEngine.ScaleModel(modelName, modelDetailsList[index].modelScaleValue);

            if (modelDetailsList[index].wireframeModeOn)
                gameEngine.TurnWireframeModeOn(modelName);

            if (modelDetailsList[index].meshLightsOn)
                gameEngine.TurnMeshLightsOn(modelName);

            if (modelDetailsList[index].manualColors)
            {
                gameEngine.UseDifferentColors(modelName, true);
                gameEngine.ChangeColor(modelName, modelDetailsList[index].modelColorRGB.x, modelDetailsList[index].modelColorRGB.y, modelDetailsList[index].modelColorRGB.z);
            }
        }

        // Loading Lights
        for (int index = 0; index < lightDetailsList.size(); index++)
        {
            lightId = lightDetailsList[index].lightId;

            gameEngine.CreateLight(lightId, lightDetailsList[index].lightPosition.x, lightDetailsList[index].lightPosition.y, lightDetailsList[index].lightPosition.z);
            gameEngine.ChangeLightType(lightId, lightDetailsList[index].lightType);
            gameEngine.ChangeLightIntensity(lightId, lightDetailsList[index].linearAttenuation, lightDetailsList[index].quadraticAttenuation);
            gameEngine.ChangeLightDirection(lightId, lightDetailsList[index].lightDirection.x, lightDetailsList[index].lightDirection.y, lightDetailsList[index].lightDirection.z);
            gameEngine.ChangeLightColour(lightId, lightDetailsList[index].lightColorRGB.r, lightDetailsList[index].lightColorRGB.g, lightDetailsList[index].lightColorRGB.b);
            gameEngine.ChangeLightAngle(lightId, lightDetailsList[index].innerAngle, lightDetailsList[index].outerAngle);

            if (lightDetailsList[index].lightOn)
                gameEngine.TurnOffLight(lightId, false);
            else
                gameEngine.TurnOffLight(lightId, true);
        }

        // Loading Initial Camera Position
        gameEngine.MoveCameraPosition(camDetails.initialCameraPosition.x, camDetails.initialCameraPosition.y, camDetails.initialCameraPosition.z);
    }

    //-------------------------------Frame loop---------------------------------------------

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        //-------------------Calculate Delta time-------------------------------------------

        double currentTime = glfwGetTime();
        gameEngine.deltaTime = currentTime - lastTime;

        const double LARGEST_DELTA_TIME = 1.0f / 30.0f;

        if (gameEngine.deltaTime > LARGEST_DELTA_TIME)
            gameEngine.deltaTime = LARGEST_DELTA_TIME;

        lastTime = currentTime;

        //--------------------Run Engine----------------------------------------------------

        gameEngine.RunGameEngine(window);
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    exit(EXIT_SUCCESS);

    std::cout << "Window closed !" << std::endl;

    return 0;
}