#ifndef SDF_MATH_HPP
#define SDF_MATH_HPP
#include <glm/glm.hpp>

namespace sdf {
	using glm::vec4;
	using glm::vec3;
	using glm::vec2;
	
	using Color = vec4;
	
	struct Ray {
		vec3 origin{};
		vec3 direction{};
		
		Ray() = default;
		Ray(vec3 orig, vec3 dir) noexcept : origin(orig), direction(glm::normalize(dir)) {}
	};
}

#endif /* SDF_MATH_HPP */
