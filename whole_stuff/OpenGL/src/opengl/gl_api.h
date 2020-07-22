#pragma once

#define GLEW_STATIC
#include "GL/glew.h"

#include "framebuffer/depthbuffer.h"
#include "framebuffer/framebuffer.h"
#include "cubemap/cubemap.h"
#include "cubemap/skybox.h"
#include "light/light.h"
#include "window/timer.h"
#include "camera/camera.h"
#include "model/model_loader.h"
#include "model/model.h"
#include "shader/shader.h"
#include "window/window.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"