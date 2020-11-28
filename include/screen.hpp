#ifndef SDF_SCREEN_HPP
#define SDF_SCREEN_HPP
#include <utility>
#include <atomic>
#include "math.hpp"

struct SDL_Surface;

namespace sdf {
	extern vec3 camera_pos;
	extern vec2 camera_rotation;
	extern std::atomic_bool quit_requested;
	struct PerspectiveCamera;
	class Object;
	void init_screen(int width, int height, int scale);
	bool screen_initialized() noexcept;
	std::pair<int, int> surfaceSize() noexcept;
	
	SDL_Surface* createSubSurface(int ysize);
	void draw(int x, int y, Color color);
	void blit(SDL_Surface* surface, int x, int y);
	void renderSurface(SDL_Surface* surface, const PerspectiveCamera& camera, const std::shared_ptr<Object>& obj, int yoff = 0, int ysize = INT32_MAX, int xoff = 0, int xsize = INT32_MAX);
	void render(const PerspectiveCamera& camera, const std::shared_ptr<Object>& obj, int yoff = 0, int ysize = INT32_MAX, int xoff = 0, int xsize = INT32_MAX);
	void set_fps(float fps);
}

#endif /* SDF_SCREEN_HPP */
