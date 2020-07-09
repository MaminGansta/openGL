
#include "process_input.h"


void ProcessCameraMovement(gl::Window& window, gl::Camera& camera, float deltaTime)
{
    // speed x
    if (gl::pressed(GLFW_KEY_W))
        camera.m_SpeedX = camera.m_SpeedX < camera.m_MaxSpeed ? camera.m_SpeedX + camera.m_DeltaSpeed : camera.m_MaxSpeed;
    else if (gl::pressed(GLFW_KEY_S))
        camera.m_SpeedX = camera.m_SpeedX > -camera.m_MaxSpeed ? camera.m_SpeedX - camera.m_DeltaSpeed : -camera.m_MaxSpeed;
    else
        camera.m_SpeedX = fabs(camera.m_SpeedX) < 0.01f ? camera.m_SpeedX = 0 : camera.m_SpeedX - gl::sgn(camera.m_SpeedX) * camera.m_DeltaSpeed;

    // speed y
    if (gl::pressed(GLFW_KEY_D))
        camera.m_SpeedY = camera.m_SpeedY < camera.m_MaxSpeed ? camera.m_SpeedY + camera.m_DeltaSpeed: camera.m_MaxSpeed;
    else if (gl::pressed(GLFW_KEY_A))
        camera.m_SpeedY = camera.m_SpeedY > -camera.m_MaxSpeed ? camera.m_SpeedY - camera.m_DeltaSpeed : -camera.m_MaxSpeed;
    else
        camera.m_SpeedY = fabs(camera.m_SpeedY) < 0.01f ? camera.m_SpeedY = 0 : camera.m_SpeedY - gl::sgn(camera.m_SpeedY) * camera.m_DeltaSpeed;

    // clamp
    if (fabs(camera.m_SpeedX) > camera.m_MaxSpeed) camera.m_SpeedX = gl::sgn(camera.m_SpeedX) * camera.m_MaxSpeed;
    if (fabs(camera.m_SpeedY) > camera.m_MaxSpeed) camera.m_SpeedY = gl::sgn(camera.m_SpeedY) * camera.m_MaxSpeed;

    // move
    camera.m_Position += camera.m_Front * camera.m_SpeedX * deltaTime;
    camera.m_Position += camera.m_Right * camera.m_SpeedY * deltaTime;

    camera.UpdateCameraVectors();

	// catch coursor
	if (gl::mouse_pressed(GLFW_MOUSE_BUTTON_RIGHT))
	{
		window.CatchCursor(true);
		camera.ProcessMouseMovementShift(gl::mouse_offsetX(), gl::mouse_offsetY());
		camera.ProcessMouseScroll(gl::fov_offset());
	}
	else
		window.CatchCursor(false);
}