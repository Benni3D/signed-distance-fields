#include "object.hpp"

namespace sdf {
	vec3 Object::normal(vec3 p) const noexcept {
		constexpr float v = 0.001f;
		const auto x = (*this)({ p.x + v, p.y, p.z }).first - (*this)({ p.x - v, p.y, p.z }).first;
		const auto y = (*this)({ p.x, p.y + v, p.z }).first - (*this)({ p.x, p.y - v, p.z }).first;
		const auto z = (*this)({ p.x, p.y, p.z + v }).first - (*this)({ p.x, p.y, p.z - v }).first;
		return glm::normalize(vec3{ x, y, z });
	}
	std::pair<vec3, vec3> Object::sampleDirectionalLight() const noexcept {
		return { { -1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }};
	}
	std::pair<float, std::shared_ptr<Shader>> Sphere::operator()(const vec3& p) const {
		return { glm::length(p) - radius, shader };
	}
	std::pair<float, std::shared_ptr<Shader>> Cube::operator()(const vec3& p) const {
		const float dx = std::abs(p.x) - a / 2, dx0 = std::max(0.0f, dx);
		const float dy = std::abs(p.y) - a / 2, dy0 = std::max(0.0f, dy);
		const float dz = std::abs(p.z) - a / 2, dz0 = std::max(0.0f, dz);
		
		const float inner = std::min(0.0f, std::max({ dx, dy, dz }));
		const float outer = std::sqrt(dx0 * dx0 + dy0 * dy0 + dz0 * dz0);
		return { inner + outer, shader };
	}
	vec3 Object::center() const noexcept { return { 0.0f, 0.0f, 0.0f }; }
	vec3 Translation::center() const noexcept { return object->center() + translation; }
	vec3 RotationX::center() const noexcept { return object->center(); }
	vec3 RotationY::center() const noexcept { return object->center(); }
	vec3 Union::center() const noexcept { return (obj1->center() + obj2->center()) / 2.0f; }
	vec3 Intersection::center() const noexcept { return (obj1->center() + obj2->center()) / 2.0f; }
	vec3 Subtraction::center() const noexcept { return (obj1->center() + obj2->center()) / 2.0f; }
	
	std::pair<float, std::shared_ptr<Shader>> Translation::operator()(const vec3& p) const {
		return (*object)(p - translation);
	}
	std::pair<float, std::shared_ptr<Shader>> RotationX::operator()(const vec3& p) const {
		const float y = cosr * p.y - sinr * p.z;
		const float z = sinr * p.y + cosr * p.z;
		return (*object)(vec3{ p.x, y, z});
	}
	std::pair<float, std::shared_ptr<Shader>> RotationY::operator()(const vec3& p) const {
		const float x = cosr * p.x - sinr * p.z;
		const float z = sinr * p.x + cosr * p.z;
		return (*object)(vec3{ x, p.y, z});
	}
	std::pair<float, std::shared_ptr<Shader>> Union::operator()(const vec3& p) const {
		const auto a = (*obj1)(p);
		const auto b = (*obj2)(p);
		return a.first < b.first ? a : b;
	}
	std::pair<float, std::shared_ptr<Shader>> Intersection::operator()(const vec3& p) const {
		const auto a = (*obj1)(p);
		const auto b = (*obj2)(p);
		return a.first > b.first ? a : b;
	}
	std::pair<float, std::shared_ptr<Shader>> Subtraction::operator()(const vec3& p) const {
		const auto a = (*obj1)(p);
		auto b = (*obj2)(p);
		b.first = -b.first;
		return a.first > b.first ? a : b;
	}
	
	void RotationX::update(float r) {
		rotation = r;
		sinr = std::sin(r);
		cosr = std::cos(r);
	}
	void RotationY::update(float r) {
		rotation = r;
		sinr = std::sin(r);
		cosr = std::cos(r);
	}
}