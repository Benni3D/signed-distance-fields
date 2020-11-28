#include "raytracer.hpp"
#include "shader.hpp"

namespace sdf {
	Color ShaderConstantColor::shade(const Ray& ray, const std::shared_ptr<Object>& object) const noexcept {
		return color;
	}
	Color ShaderLambertian::shade(const Ray& ray, const std::shared_ptr<Object>& object) const noexcept {
		const auto [lightDir, lightColor] = object->sampleDirectionalLight();
		const auto normal = object->normal(ray.origin);
		
		const vec3 tmp = (vec3{color.x, color.y, color.z} * lightColor) * std::clamp(glm::dot(normal, lightDir), 0.0f, 1.0f);
		return { tmp.x, tmp.y, tmp.z, color.w };
	}
}