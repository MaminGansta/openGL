#pragma once

#include "GLFW/glfw3.h"
#include "camera/camera.h"
#include "window/input.h"
#include "window/window.h"

void ProcessCameraMovement(gl::Window& window, gl::Camera& camera, float delta_time);