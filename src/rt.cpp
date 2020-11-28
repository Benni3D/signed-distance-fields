#include <spdlog/spdlog.h>
#include "raytracer.hpp"

namespace sdf {
	[[nodiscard]]
	static Color project_background(const Ray& ray) noexcept {
		const auto dir = ray.direction;
		if (dir.y < 0.0f) return { 0.1f, 0.4f, 0.1f, 1.0f };
		else return { 2.0f * dir.y, 4.0f * dir.y, 1.0f, 1.0f };
	}
	
	Ray PerspectiveCamera::project(float x, float y) const noexcept {
		static constexpr vec3 pos{ 0.0f, 0.0f, 0.0f };
		return Ray{ pos, { (x - 0.5f) * float(sensorWidth), -(y - 0.5f) * float(sensorHeight), -focalLength } };
	}

	Color trace(const std::shared_ptr<Object>& object, Ray ray, float bgDist) {
		auto ds = (*object)(ray.origin);
		while (std::abs(ds.first) > 0.0001) {
			ray.origin += ray.direction * ds.first;
			ds = (*object)(ray.origin);
			if (glm::length(ray.origin) >= bgDist)
				return project_background(ray);
		}
		return ds.second->shade(ray, object);
	}
}