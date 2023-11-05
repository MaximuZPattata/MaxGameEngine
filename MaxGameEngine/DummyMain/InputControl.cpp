#include "GLWF_CallBacks.h"

#include <string>
#include <iostream>
#include <vector>

#include "cControlGameEngine.h"


extern cControlGameEngine gameEngine;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    const float CAMERA_MOVEMENT_SPEED = 3.0f;

    //-----------------------------------Rotate--------------------------------------------------

    if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)
    {
        glm::vec3 cam_target = gameEngine.GetCurrentCameraTarget();

        if (key == GLFW_KEY_A)
        {
            cam_target.x -= CAMERA_MOVEMENT_SPEED;

            gameEngine.MoveCameraTarget(cam_target.x, cam_target.y, cam_target.z);
        }
        if (key == GLFW_KEY_D)
        {
            cam_target.x += CAMERA_MOVEMENT_SPEED;

            gameEngine.MoveCameraTarget(cam_target.x, cam_target.y, cam_target.z);
        }

        if (key == GLFW_KEY_W)
        {
            cam_target.z -= CAMERA_MOVEMENT_SPEED;

            gameEngine.MoveCameraTarget(cam_target.x, cam_target.y, cam_target.z);
        }
        if (key == GLFW_KEY_S)
        {
            cam_target.z += CAMERA_MOVEMENT_SPEED;

            gameEngine.MoveCameraTarget(cam_target.x, cam_target.y, cam_target.z);
        }

        if (key == GLFW_KEY_Q)
        {
            cam_target.y -= CAMERA_MOVEMENT_SPEED;

            gameEngine.MoveCameraTarget(cam_target.x, cam_target.y, cam_target.z);
        }
        if (key == GLFW_KEY_E)
        {
            cam_target.y += CAMERA_MOVEMENT_SPEED;

            gameEngine.MoveCameraTarget(cam_target.x, cam_target.y, cam_target.z);
        }
    }

    if (mods == 0)
    {
        //------------------------------Camera Showcases------------------------------------------------

        glm::vec3 cam_position = gameEngine.GetCurrentCameraPosition();

        if (key == GLFW_KEY_1 && action)
        {
            gameEngine.MoveCameraPosition(0.0f, 50.0f, 300.0f);
            gameEngine.MoveCameraTarget(0.0f, 0.0f, 0.0f);
        }

        if (key == GLFW_KEY_2 && action)
        {
            gameEngine.MoveCameraPosition(318.0f, 44.0f, 15.0f);
            gameEngine.MoveCameraTarget(0.0f, 0.0f, 0.0f);
        }

        if (key == GLFW_KEY_3 && action)
        {
            gameEngine.MoveCameraPosition(0.0f, 122.0f, -351.0f);
            gameEngine.MoveCameraTarget(0.0f, 0.0f, 0.0f);
        }

        if (key == GLFW_KEY_4 && action)
        {
            gameEngine.MoveCameraPosition(-255.0f, 125.0f, 3.0f);
            gameEngine.MoveCameraTarget(0.0f, 0.0f, 0.0f);
        }

        if (key == GLFW_KEY_5 && action)
        {
            gameEngine.MoveCameraPosition(0.0f, 461.0f, -3.0f);
            gameEngine.MoveCameraTarget(0.0f, 0.0f, 0.0f);
        }

        if (key == GLFW_KEY_SPACE && action) //Question 5
        {
            //gameEngine.PushAsteroidsAway();
        }

        //------------------------------Move Camera--------------------------------------------------------

        if (key == GLFW_KEY_A && action)
        {
            cam_position.x -= CAMERA_MOVEMENT_SPEED;

            gameEngine.MoveCameraPosition(cam_position.x, cam_position.y, cam_position.z);
        }
        if (key == GLFW_KEY_D && action)
        {
            cam_position.x += CAMERA_MOVEMENT_SPEED;

            gameEngine.MoveCameraPosition(cam_position.x, cam_position.y, cam_position.z);
        }
        if (key == GLFW_KEY_W && action)
        {
            cam_position.z -= CAMERA_MOVEMENT_SPEED;

            gameEngine.MoveCameraPosition(cam_position.x, cam_position.y, cam_position.z);
        }
        if (key == GLFW_KEY_S && action)
        {
            cam_position.z += CAMERA_MOVEMENT_SPEED;

            gameEngine.MoveCameraPosition(cam_position.x, cam_position.y, cam_position.z);
        }
        if (key == GLFW_KEY_Q && action)
        {
            cam_position.y -= CAMERA_MOVEMENT_SPEED;

            gameEngine.MoveCameraPosition(cam_position.x, cam_position.y, cam_position.z);
        }
        if (key == GLFW_KEY_E && action)
        {
            cam_position.y += CAMERA_MOVEMENT_SPEED;

            gameEngine.MoveCameraPosition(cam_position.x, cam_position.y, cam_position.z);
        }
    }

    return;
}