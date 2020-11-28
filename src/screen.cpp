#include <spdlog/spdlog.h>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <chrono>
#include "raytracer.hpp"
#include "screen.hpp"

namespace sdf {
	using namespace std::chrono_literals;
	vec3 camera_pos{};
	vec2 camera_rotation{};
	std::atomic_bool quit_requested;
	static SDL_Window* window = nullptr;
	static SDL_Surface* surface = nullptr;
	static SDL_Texture* texture = nullptr;
	static SDL_Renderer* windowRenderer = nullptr;
	static std::atomic_bool initialized = false;
	
	bool screen_initialized() noexcept { return initialized; }
	std::pair<int, int> surfaceSize() noexcept { return { surface->w, surface->h }; }
	static bool quit() {
		spdlog::debug("Quitting...");
		quit_requested = true;
		while (quit_requested);
		return true;
	}
	
	void init_screen(int width, int height, int scale) {
		if (window || surface) return;
		spdlog::set_level(spdlog::level::debug);
		if (SDL_Init(SDL_INIT_VIDEO)) {
			spdlog::critical("Failed to initialize SDL2: {}", SDL_GetError());
			std::exit(1);
		}
		spdlog::debug("Initialized SDL2");
		window = SDL_CreateWindow(
				"Signed Distance Fields Demo",
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				width * scale, height * scale,
				SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
		);
		spdlog::debug("Created window with size [{},{}]", width, height);
		if (!window) {
			spdlog::critical("Failed to create window: {}", SDL_GetError());
			std::exit(1);
		}
		windowRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (!windowRenderer) {
			spdlog::critical("Failed to create renderer: {}", SDL_GetError());
			std::exit(1);
		}
		surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 24, SDL_PIXELFORMAT_RGB888);
		if (!surface) {
			spdlog::critical("Failed to create surface: {}", SDL_GetError());
			std::exit(1);
		}
		texture = SDL_CreateTextureFromSurface(windowRenderer, surface);
		if (!texture) {
			spdlog::critical("Failed to create Texture: {}", SDL_GetError());
			std:exit(1);
		}
		spdlog::debug("Created Surface with size [{},{}]", surface->w, surface->h);
		
		initialized = true;
		
		const auto now = []{ return std::chrono::high_resolution_clock::now(); };
		auto lastFrame = now();
		
		constexpr float speed = 4.0f, rotate_speed = 200.0f;
		bool up{}, down{}, forward{}, backward{}, left{}, right{};
		while (true) {
			const auto curFrame = now();
			const auto delta = std::chrono::duration_cast<std::chrono::microseconds>(curFrame - lastFrame).count() / 1000000.0f;
			SDL_GetWindowSize(window, &width, &height);
			
			float dx = 0, dy = 0;
			if (SDL_Event e; SDL_PollEvent(&e)) {
				switch (e.type) {
				case SDL_QUIT: if (quit()) goto end; break;
				case SDL_KEYDOWN:
				case SDL_KEYUP: {
					const bool state = e.key.state;
					switch (e.key.keysym.sym) {
					case SDLK_w: forward = state; break;
					case SDLK_s: backward = state; break;
					case SDLK_a: left = state; break;
					case SDLK_d: right = state; break;
					case SDLK_SPACE: up = state; break;
					case SDLK_LSHIFT: down = state; break;
					case SDLK_F11: SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP); break;
					}
					break;
				}
				case SDL_MOUSEMOTION:
					dx = float(e.motion.x - float(width) / 2.0f) / float(width);
					dy = float(e.motion.y - float(height) / 2.0f) / float(height);
					break;
				}
			}
			
			const int dir_z = (forward && !backward) - (!forward && backward);
			const int dir_y = (up && !down) - (!up && down);
			const int dir_x = (left && !right) - (!left && right);
			camera_pos.z += dir_z * speed * std::cos(camera_rotation.y) * delta;
			camera_pos.x -= dir_z * speed * std::sin(camera_rotation.y) * delta;
			camera_pos.z += dir_x * speed * std::sin(camera_rotation.y) * delta;
			camera_pos.x += dir_x * speed * std::cos(camera_rotation.y) * delta;
			camera_pos.y -= dir_y * speed * delta;
			
			camera_rotation.y += dx * rotate_speed * delta;
			camera_rotation.x -= dy * rotate_speed * delta;
			
			SDL_WarpMouseInWindow(window, width / 2, height / 2);
			
			//SDL_BlitScaled(surface, nullptr, SDL_GetWindowSurface(window), nullptr);
			//SDL_UpdateWindowSurface(window);
			
			SDL_UpdateTexture(texture, nullptr, surface->pixels, surface->pitch);
			SDL_RenderClear(windowRenderer);
			SDL_RenderCopy(windowRenderer, texture, nullptr, nullptr);
			SDL_RenderPresent(windowRenderer);
			
			lastFrame = curFrame;
			std::this_thread::sleep_for(1ms);
		}
	end:;
		SDL_FreeSurface(surface);
		SDL_DestroyWindow(window);
		SDL_Quit();
		std::exit(0);
	}
	
	void draw(int x, int y, Color color) {
		const SDL_Rect rect = { x, y, 1, 1 };
		const std::uint8_t r = uint8_t(color.x * 255.0f);
		const std::uint8_t g = uint8_t(color.y * 255.0f);
		const std::uint8_t b = uint8_t(color.z * 255.0f);
		const std::uint32_t rgb = (r << 16) | (g << 8) | b;
		SDL_FillRect(surface, &rect, rgb);
	}
	constexpr Color alpha_blend(Color c1, Color c2) {
		return (1.0f - c2.w) * c1 + c2.w * c2;
	}
	void render(const PerspectiveCamera& camera, const std::shared_ptr<Object>& obj, int yoff, int ysize, int xoff, int xsize) {
		const int width = surface->w, height = surface->h;
		yoff = std::clamp(yoff, 0, height);
		ysize = std::clamp(ysize, 0, height - yoff);
		xoff = std::clamp(xoff, 0, width);
		xsize = std::clamp(xsize, 0, width - xoff);
		for (int y = yoff; y < yoff + ysize; ++y) {
			for (int x = xoff; x < xoff + xsize; ++x) {
				const auto ray = camera.project((float(x) + 0.5f) / float(width), (float(y) + 0.5f) / float(height));
				draw(x, y, trace(obj, ray));
			}
		}
	}
	void set_fps(float fps) {
		std::string tmp = "Signed Distance Fields Demo | FPS:" + std::to_string(fps);
		SDL_SetWindowTitle(window, tmp.c_str());
	}
}