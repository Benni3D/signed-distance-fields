#include <numeric>
#include <thread>
#include <array>
#include "object.hpp"
#include "shader.hpp"
#include "raytracer.hpp"
#include "screen.hpp"

#define MULTI 0

using namespace sdf;
using namespace std::chrono_literals;

static PerspectiveCamera camera{};
static std::shared_ptr<Object> scene{};

static auto now() { return std::chrono::high_resolution_clock::now(); }

static void do_render(volatile float* fps = nullptr, int yoff = 0, int ysize = INT32_MAX) {
	auto lastFrame = now();
	const auto translation = std::make_shared<Translation>(scene, camera_pos);
	const auto rotationY = std::make_shared<RotationY>(translation, camera_rotation.y);
	const auto rotation = std::make_shared<RotationX>(rotationY, camera_rotation.x);
	while (!quit_requested) {
		const auto curFrame = now();
		const float diff = std::chrono::duration_cast<std::chrono::microseconds>(curFrame - lastFrame).count() / 1'000'000.f;
		if (fps) *fps = 1.0f / diff; else set_fps(1.0f / diff);
		
		translation->translation = camera_pos;
		rotationY->update(camera_rotation.y);
		rotation->update(camera_rotation.x);
		
		render(camera, rotation, yoff, ysize);
		
		lastFrame = curFrame;
	}
}


int main() {
	constexpr int div = 4;
	constexpr int w = 640 / div, h = 360 / div;
	std::thread screen_thread(init_screen, w, h, 5);
	auto shader = std::make_shared<sdf::ShaderLambertian>(Color{ 0.1f, 0.1f, 0.9f, 1.0f });
	auto cube = std::make_shared<sdf::Cube>(1.0f, shader);
	auto obj = std::make_shared<sdf::Translation>(cube, vec3{ 1.8f, 0.0f, -10.0f });
	auto obj2 = std::make_shared<sdf::Translation>(cube, vec3{ 0.0f, 0.0f, -10.0f });
	auto obj3 = std::make_shared<sdf::Translation>(cube, vec3{ 1.8f, 1.0f, -10.0f });
	auto obj4 = std::make_shared<sdf::Translation>(cube, vec3{ 0.0f, 1.0f, -10.0f });
	scene = std::make_shared<Union>(std::make_shared<Union>(obj, obj2), std::make_shared<Union>(obj3, obj4));
	while (!screen_initialized());

#if MULTI
	constexpr size_t num_threads = 16;
	std::array<std::thread, num_threads> renderer{};
	std::array<volatile float, num_threads> fps{};
	
	for (size_t i = 0; i < renderer.size(); ++i) {
		const float ysize = ((float)h / (float)renderer.size());
		const float yoff = ysize * (float)i;
		renderer[i] = std::thread(do_render, &fps[i], (int)yoff, (int)std::ceil(ysize));
	}
	
	while (!quit_requested) {
		set_fps(std::accumulate(fps.begin(), fps.end(), 0.0f) / fps.size());
		std::this_thread::sleep_for(10ms);
	}
	
	for (auto& r : renderer) {
		r.join();
	}
#else
	do_render();
#endif
	
	quit_requested = false;
	screen_thread.join();
	return 0;
}