#ifndef SDF_OBJECT_HPP
#define SDF_OBJECT_HPP
#include <utility>
#include <memory>
#include "shader.hpp"
#include "math.hpp"

namespace sdf {
	class Object {
	public:
		virtual ~Object() = default;
		virtual std::pair<float, std::shared_ptr<Shader>> operator()(const vec3& p) const = 0;
		[[nodiscard]]
		virtual vec3 normal(vec3 p) const noexcept;
		[[nodiscard]]
		virtual std::pair<vec3, vec3> sampleDirectionalLight() const noexcept;
		[[nodiscard]]
		virtual vec3 center() const noexcept;
	};
	
	
	class Sphere : public Object {
	public:
		float radius;
		std::shared_ptr<Shader> shader;
		Sphere(float radius, const std::shared_ptr<Shader>& shader) : radius(radius), shader(shader) {}
		
		[[nodiscard]]
		std::pair<float, std::shared_ptr<Shader>> operator()(const vec3& p) const override;
	};
	class Cube : public Object {
	public:
		float a;
		std::shared_ptr<Shader> shader;
		Cube(float a, const std::shared_ptr<Shader>& shader) : a(a), shader(shader) {}
		
		[[nodiscard]]
		std::pair<float, std::shared_ptr<Shader>> operator()(const vec3& p) const override;
	};
	
	
	class Translation : public Object {
	public:
		std::shared_ptr<Object> object;
		vec3 translation;
		
		Translation() = default;
		Translation(const std::shared_ptr<Object>& sdf, vec3 translation)
				: object(sdf), translation(translation) {}
		
		[[nodiscard]]
		std::pair<float, std::shared_ptr<Shader>> operator()(const vec3& p) const override;
		[[nodiscard]]
		vec3 center() const noexcept override;
	};
	class RotationX : public Object {
	private:
		float sinr, cosr;
	public:
		std::shared_ptr<Object> object;
		float rotation;
		
		RotationX(const std::shared_ptr<Object>& obj, float rotation)
			: object(obj), rotation(rotation), sinr(std::sin(rotation)), cosr(std::cos(rotation)) {}
		
		[[nodiscard]]
		std::pair<float, std::shared_ptr<Shader>> operator()(const vec3& p) const override;
		[[nodiscard]]
		vec3 center() const noexcept override;
		void update(float rotation);
	};
	class RotationY : public Object {
	private:
		float sinr, cosr;
	public:
		std::shared_ptr<Object> object;
		float rotation;
		
		RotationY(const std::shared_ptr<Object>& obj, float rotation)
				: object(obj), rotation(rotation), sinr(std::sin(rotation)), cosr(std::cos(rotation)) {}
		
		[[nodiscard]]
		std::pair<float, std::shared_ptr<Shader>> operator()(const vec3& p) const override;
		[[nodiscard]]
		vec3 center() const noexcept override;
		void update(float rotation);
	};
	
	class Union : public Object {
	public:
		std::shared_ptr<Object> obj1, obj2;
		
		Union(const std::shared_ptr<Object>& obj1, const std::shared_ptr<Object>& obj2)
			: obj1(obj1), obj2(obj2) {}
		
		[[nodiscard]]
		std::pair<float, std::shared_ptr<Shader>> operator()(const vec3& p) const override;
		[[nodiscard]]
		vec3 center() const noexcept override;
	};
	class Intersection : public Object {
	public:
		std::shared_ptr<Object> obj1, obj2;
		
		Intersection(const std::shared_ptr<Object>& obj1, const std::shared_ptr<Object>& obj2)
				: obj1(obj1), obj2(obj2) {}
		
		[[nodiscard]]
		std::pair<float, std::shared_ptr<Shader>> operator()(const vec3& p) const override;
		[[nodiscard]]
		vec3 center() const noexcept override;
	};
	class Subtraction : public Object {
	public:
		std::shared_ptr<Object> obj1, obj2;
		
		Subtraction(const std::shared_ptr<Object>& obj1, const std::shared_ptr<Object>& obj2)
				: obj1(obj1), obj2(obj2) {}
		
		[[nodiscard]]
		std::pair<float, std::shared_ptr<Shader>> operator()(const vec3& p) const override;
		[[nodiscard]]
		vec3 center() const noexcept override;
	};
}

#endif /* SDF_OBJECT_HPP */
