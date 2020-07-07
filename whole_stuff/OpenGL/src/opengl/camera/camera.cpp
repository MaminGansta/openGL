
#include "pch.h"
#include "camera.h"


namespace gl
{
   
    // Constructor with vectors
    Camera::Camera(const glm::vec3& position, float yaw, float pitch, const glm::vec3& up)
        :
        m_MaxSpeed(MAX_SPEED),
        m_MouseSensitivity(SENSITIVTY),
        m_Fov(FOV)
    {
        m_Position = position;
        m_WorldUp = up;
        m_Yaw = yaw;
        m_Pitch = pitch;
        UpdateCameraVectors();
    }


    // Constructor with scalar values
    Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        :
        m_MaxSpeed(MAX_SPEED),
        m_MouseSensitivity(SENSITIVTY),
        m_Fov(FOV)
    {
        m_Position = glm::vec3(posX, posY, posZ);
        m_WorldUp = glm::vec3(upX, upY, upZ);
        m_Yaw = yaw;
        m_Pitch = pitch;
        UpdateCameraVectors();
    }


    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 Camera::GetLookatMat()
    {
        return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
    }

    glm::mat4 Camera::GetPprojectionMat(int WindowWidth, int WindowHeight)
    {
        float aspect = (float)WindowWidth / WindowHeight;
        return glm::perspective<float>(m_Fov, aspect, 0.1f, 100.0f);
    }


    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
    {
        if (direction == CameraMovement::FORWARD)
            m_SpeedX += m_DeltaSpeed;
        else if (direction == CameraMovement::BACKWARD)
            m_SpeedX -= m_DeltaSpeed;
        else
            m_SpeedX -= sgn(m_SpeedX) * m_DeltaSpeed;

        if (direction == CameraMovement::LEFT)
            m_SpeedY += m_DeltaSpeed;
        else if (direction == CameraMovement::RIGHT)
            m_SpeedY -= m_DeltaSpeed;
        else
            m_SpeedY -= sgn(m_SpeedY) * m_DeltaSpeed;

        if (fabs(m_SpeedX) > m_MaxSpeed) m_SpeedX = sgn(m_SpeedX) * m_MaxSpeed;
        if (fabs(m_SpeedY) > m_MaxSpeed) m_SpeedY = sgn(m_SpeedY) * m_MaxSpeed;

        m_Position -= m_Front * m_SpeedX * deltaTime;
        m_Position += m_Right * m_SpeedY * deltaTime;
        UpdateCameraVectors();
    }


    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void Camera::ProcessMouseMovement(float MousePosX, float MousePosY)
    {
        float xoffset = (m_MouseX - MousePosX) * m_MouseSensitivity;
        float yoffset = (m_MouseY - MousePosY) * m_MouseSensitivity;

        m_MouseX = MousePosX;
        m_MouseY = MousePosY;

        m_Yaw -= xoffset;
        m_Pitch -= yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (m_Pitch > PI / 2 -0.1f)
            m_Pitch = PI / 2 -0.1f;

        if (m_Pitch < -PI / 2 - 0.1f)
            m_Pitch = -PI / 2 - 0.1f;

        // Update Front, Right and Up Vectors using the updated Eular angles
        UpdateCameraVectors();
    }

    void Camera::ProcessMouseMovementShift(float xoffset, float yoffset)
    {
        xoffset *= m_MouseSensitivity;
        yoffset *= m_MouseSensitivity;

        m_Yaw += xoffset;
        m_Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (m_Pitch > PI/2 - 0.1f)
            m_Pitch = PI/2 - 0.1f;

        if (m_Pitch < -PI / 2 - 0.1f)
            m_Pitch = -PI / 2 - 0.1f;

        // Update Front, Right and Up Vectors using the updated Eular angles
        UpdateCameraVectors();
    }


    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void Camera::ProcessMouseScroll(float yoffset)
    {
        if (m_Fov >= 0.1f && m_Fov <= PI / 2)
            m_Fov += yoffset * m_DeltaFov;

        if (m_Fov < 0.1f)
            m_Fov = 0.1f;

        if (m_Fov > PI / 2)
            m_Fov = PI / 2;
    }


    // Calculates the front vector from the Camera's (updated) Eular Angles
    void Camera::UpdateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cosf(m_Yaw) * cosf(m_Pitch);
        front.y = sinf(m_Pitch);
        front.z = sinf(m_Yaw) * cosf(m_Pitch);
        m_Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }

}