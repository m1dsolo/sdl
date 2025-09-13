// Simple SDL cpp wrapper with SDL3, SDL_image, SDL_mixer, SDL_ttf
// TODO: RAII
#pragma once

#include <filesystem>
#include <utility>
#include <string>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace fs = std::filesystem;

namespace sdl {

class SDL {
public:
    class TargetGuard {
    public:
        TargetGuard(SDL_Texture* target) : original_target_(get_render_target()) {
            set_render_target(target);
        }

        ~TargetGuard() {
            set_render_target(original_target_);
        }

    private:
        SDL_Texture* original_target_;
    };

    static const inline SDL_Color BLACK = {0, 0, 0, 255};
    static const inline SDL_Color WHITE = {255, 255, 255, 255};
    static const inline SDL_Color GRAY = {128, 128, 128, 255};
    static const inline SDL_Color RED = {255, 0, 0, 255};
    static const inline SDL_Color GREEN = {0, 255, 0, 255};
    static const inline SDL_Color BLUE = {0, 0, 255, 255};
    static const inline SDL_Color YELLOW = {255, 255, 0, 255};
    static const inline SDL_Color PURPLE = {255, 0, 255, 255};
    static const inline SDL_Color CYAN = {0, 255, 255, 255};
    static const inline SDL_Color PINK = {255, 192, 203, 255};
    static const inline SDL_Color ORANGE = {255, 165, 0, 255};
    static const inline SDL_Color TRANSPARENT = {0, 0, 0, 0};

    // init
    static bool init(SDL_InitFlags init_flags, const std::string& win_name, int w, int h, SDL_WindowFlags window_flags);
    static bool init_audio();
    static bool init_ttf(const fs::path& font_path, float ptsize);
    static bool init_gamepad();

    // SDL
    static void render_texture(SDL_Texture* texture, const SDL_FRect* src = nullptr, const SDL_FRect* dst = nullptr);
    static SDL_Texture* create_texture(SDL_Surface* surface);
    static SDL_Surface* create_surface(int w, int h, SDL_Color color = BLACK, SDL_PixelFormat format = SDL_PIXELFORMAT_RGBA8888);
    static SDL_Texture* create_texture(int w, int h, SDL_Color color = BLACK, SDL_TextureAccess access = SDL_TEXTUREACCESS_STATIC, SDL_PixelFormat format = SDL_PIXELFORMAT_RGBA8888);
    static SDL_Texture* create_circle_texture(float radius, SDL_Color color = BLACK);
    static SDL_Texture* create_filled_circle_texture(float radius, SDL_Color color = BLACK);

    static void render_rect(const SDL_FRect* dst, SDL_Color color);
    static void render_filled_rect(const SDL_FRect* dst, SDL_Color color);
    static void render_circle(const SDL_FRect* dst, SDL_Color color);
    static void render_filled_circle(const SDL_FRect* dst, SDL_Color color);

    static const char* get_error();
    static std::pair<float, float> get_texture_size(SDL_Texture* texture);
    static SDL_FPoint get_mouse_position();
    static SDL_Texture* get_render_target();

    static void set_window_position(int x, int y);
    static void set_render_vsync(bool vsync);
    static void set_color(SDL_Color color);
    static void set_render_target(SDL_Texture* texture);
    static void set_blend_mode(SDL_Surface* surface, SDL_BlendMode mode);
    static void set_blend_mode(SDL_Texture* texture, SDL_BlendMode mode);

    static void render_clear();
    static void render_present();
    static void hide_cursor();
    static bool point_in_rect(const SDL_FPoint* point, const SDL_FRect* rect);

    static void destroy();
    static void destroy(SDL_Texture* texture);
    static void destroy(SDL_Surface* surface);

    // SDL_image
    static SDL_Texture* load_image(const fs::path& path, int w = 0, int h = 0);

    // SDL_mixer

    // SDL_ttf
    static SDL_Surface* create_surface(const std::string& text, float ptsize, SDL_Color color = BLACK, int wrap_width = 0);
    static SDL_Texture* create_texture(const std::string& text, float ptsize, SDL_Color color = BLACK, int wrap_width = 0);

    static void set_font_size(float ptsize);

    static SDL_Window* window() { return window_; }
    static SDL_Renderer* renderer() { return renderer_; }
    static TTF_Font* font() { return font_; }

private:
    static inline SDL_Window* window_ = nullptr;
    static inline SDL_Renderer* renderer_ = nullptr;
    static inline TTF_Font* font_ = nullptr;
    static inline SDL_Gamepad* gamepad_ = nullptr;
};

inline bool operator==(const SDL_Color& lhs, const SDL_Color& rhs) {
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

}  // namespace sdl
