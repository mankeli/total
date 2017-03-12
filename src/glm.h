#pragma once 
#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE 
#define GLM_SWIZZLE_XYZW 

#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_precision.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"

using glm::vec2;
using glm::mat2;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

using glm::u8vec3;
using glm::i32vec2;
using glm::i32vec3;
using glm::ivec2;
using glm::ivec3;

using glm::clamp;
using glm::linearRand;
using glm::normalize;
using glm::dot;

using glm::mix;

using glm::min;
using glm::max;
using glm::floor;
using glm::fract;
using glm::smoothstep;
