#ifndef SDF_RAYTRACER_HPP
#define SDF_RAYTRACER_HPP
#include "object.hpp"
#include "math.hpp"

namespace sdf {
	struct PerspectiveCamera {
		float focalLength;
		int sensorWidth;
		int sensorHeight;
		
		explicit PerspectiveCamera(float focalLength = 50.0f, int sensorWidth = 32, int sensorHeight = 18) noexcept
				: focalLength(focalLength), sensorWidth(sensorWidth), sensorHeight(sensorHeight) {}
		
		[[nodiscard]]
		Ray project(float x, float y) const noexcept;
	};
	
	
	[[nodiscard]]
	Color trace(const std::shared_ptr<Object>& object, Ray ray, float bgDist = 1000.0f);
}

#endif /* SDF_RAYTRACER_HPP */
