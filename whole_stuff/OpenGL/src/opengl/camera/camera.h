#pragma once

#include "glm.hpp"

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    NO_DIRECTION
};


namespace gl
{
    template <typename T>
    inline int sgn(T num) { return (num > T(0)) - (num < T(0)); }

    const float PI = 3.14159265359f;

    // Default camera values
    const static float YAW = -PI / 2;
    const static float PITCH = 0;
    const static float MAX_SPEED = 20.0f;
    const static float DELTA_SPEED = 1.0f;

    const static float SENSITIVTY = 0.25f;
    const static float FOV = PI / 4;
    
    struct Camera
    {
    public:
        // Camera Attributes
        glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 m_Up;
        glm::vec3 m_Right;
        glm::vec3 m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

        // Eular Angles
        float m_Yaw = -PI / 2;
        float m_Pitch = 0;

        // Camera options
        float m_MaxSpeed = MAX_SPEED;
        float m_SpeedX = 0;
        float m_SpeedY = 0;
        float m_DeltaSpeed = DELTA_SPEED;
        float m_MouseSensitivity = SENSITIVTY;
        float m_Fov = FOV;
        float m_DeltaFov = 0.05f;
        float m_NearPlane = 0.1f;
        float m_FrarPlane = 120.0f;

        // Last mouse position
        float m_MouseX = 0.5f;
        float m_MouseY = 0.5f;

        bool m_NewFrame = true;

        // Constructor with vectors
        Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
            float yaw = YAW,
            float pitch = PITCH,
            const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f)
        );


        // Constructor with scalar values
        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

        glm::mat4 GetLookatMat();

        glm::mat4 GetPprojectionMat(int WindowWidth, int WindowHeight);
        
        void ProcessKeyboard(CameraMovement direction, float deltaTime);
        
        void ProcessMouseMovement(float xMousePos, float yMousePos);
        void ProcessMouseMovementShift(float xoffset, float yoffset);

        void ProcessMouseScroll(float yoffset);
      
        void UpdateCameraVectors();
    };
}