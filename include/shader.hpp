#ifndef SDF_SHADER_HPP
#define SDF_SHADER_HPP
#include "math.hpp"

namespace sdf {
	struct RayTracer;
	class Object;
	class Shader {
	public:
		virtual ~Shader() = default;
		
		[[nodiscard]]
		virtual Color shade(const Ray& ray, const std::shared_ptr<Object>& object) const = 0;
	};
	
	class ShaderConstantColor : public Shader {
	public:
		Color color;
		explicit ShaderConstantColor(Color color) noexcept : color(color) {}
		
		[[nodiscard]]
		Color shade(const Ray& ray, const std::shared_ptr<Object>& object) const noexcept override;
	};
	
	class ShaderLambertian : public Shader {
	public:
		Color color;
		explicit ShaderLambertian(Color color) noexcept : color(color) {}
		
		[[nodiscard]]
		Color shade(const Ray& ray, const std::shared_ptr<Object>& object) const noexcept override;
	};
}

#endif /* SDF_SHADER_HPP */
