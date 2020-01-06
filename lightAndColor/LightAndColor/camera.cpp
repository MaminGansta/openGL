
struct Camera
{
public:
    // Camera Attributes
    glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    // Eular Angles
    GLfloat Yaw = -PI / 2;
    GLfloat Pitch = 0;
    // Camera options
    float MaxSpeed = 6.0f;
    float speed_x = 0;
    float speed_y = 0;
    float speed_delta = 0.5f;
    float MouseSensitivity = 0.25f;
    float Zoom = PI / 4;


    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void update(float delta_time)
    {
        glm::vec3 front;
        front.x = cosf(Yaw) * cosf(Pitch);
        front.y = sinf(Pitch);
        front.z = sinf(Yaw) * cosf(Pitch);
        Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));

        // process movement
        if (pressed(GLFW_KEY_W))
            speed_x = speed_x < MaxSpeed ? speed_x + speed_delta : MaxSpeed;
        else if (pressed(GLFW_KEY_S))
            speed_x = speed_x > -MaxSpeed ? speed_x - speed_delta : -MaxSpeed;
        else
            speed_x = fabs(speed_x) < 0.01f ? speed_x = 0 : speed_x - sgn(speed_x) * speed_delta;

        // process movement
        if (pressed(GLFW_KEY_D))
            speed_y = speed_y < MaxSpeed ? speed_y + speed_delta : MaxSpeed;
        else if (pressed(GLFW_KEY_A))
            speed_y = speed_y > -MaxSpeed ? speed_y - speed_delta : -MaxSpeed;
        else
            speed_y = fabs(speed_y) < 0.01f ? speed_y = 0 : speed_y - sgn(speed_y) * speed_delta;

        if (pressed(GLFW_KEY_UP))
            Pitch += Pitch > PI / 2 - 0.1f ? 0 :  2 * delta_time;
        if (pressed(GLFW_KEY_DOWN))
            Pitch -= Pitch < -PI / 2 + 0.1f ?  0 : 2 * delta_time;
        if (pressed(GLFW_KEY_LEFT))
            Yaw -= 3 * delta_time;
        if (pressed(GLFW_KEY_RIGHT))
            Yaw += 3 * delta_time;

       // std::cout << Pitch << ' ' << Yaw << '\n';

        // movement
        Position += Front * speed_x * delta_time;
        Position += Right * speed_y * delta_time;

        // FOV
        Zoom = Zoom < 1.0f ? 1.0f : Zoom;
        Zoom = Zoom > PI / 4 ? PI / 4 : Zoom;
    }

};