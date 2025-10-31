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
    class RenderTargetGuard {
    public:
        RenderTargetGuard(SDL_Texture* target) : original_target_(get_render_target()) {
            set_render_target(target);
        }

        ~RenderTargetGuard() {
            set_render_target(original_target_);
        }

    private:
        SDL_Texture* original_target_;
    };

    class TextureColorGuard {
    public:
        TextureColorGuard(SDL_Texture* texture, SDL_FColor color) : texture_(texture) {
            original_color_ = get_texture_color(texture);
            set_texture_color(texture, color);
        }
        ~TextureColorGuard() {
            set_texture_color(texture_, original_color_);
        }

    private:
        SDL_Texture* texture_;
        SDL_FColor original_color_;
    };

    class RenderColorGuard {
    public:
        RenderColorGuard(SDL_FColor color) {
            SDL_GetRenderDrawColorFloat(SDL::renderer(), &original_color_.r, &original_color_.g, &original_color_.b, &original_color_.a);
            set_render_color(color);
        }
        ~RenderColorGuard() {
            set_render_color(original_color_);
        }

    private:
        SDL_FColor original_color_;
    };

    enum class TextAlign {
        Left,
        Center,
        Right
    };

    struct Color {
        static const inline SDL_FColor Black = {0.0f, 0.0f, 0.0f, 1.0f};
        static const inline SDL_FColor White = {1.0f, 1.0f, 1.0f, 1.0f};
        static const inline SDL_FColor Gray = {0.5f, 0.5f, 0.5f, 1.0f};
        static const inline SDL_FColor Red = {1.0f, 0.0f, 0.0f, 1.0f};
        static const inline SDL_FColor Green = {0.0f, 1.0f, 0.0f, 1.0f};
        static const inline SDL_FColor Blue = {0.0f, 0.0f, 1.0f, 1.0f};
        static const inline SDL_FColor Yellow = {1.0f, 1.0f, 0.0f, 1.0f};
        static const inline SDL_FColor Purple = {1.0f, 0.0f, 1.0f, 1.0f};
        static const inline SDL_FColor Cyan = {0.0f, 1.0f, 1.0f, 1.0f};
        static const inline SDL_FColor Pink = {1.f, 192.0f / 255.0f, 203.0f / 255.0f, 1.0f};
        static const inline SDL_FColor Orange = {1.f, 165.0f / 255.0f, 0.0f, 1.0f};
        static const inline SDL_FColor Transparent = {0.0f, 0.0f, 0.0f, 0.0f};
    };


    // init
    static bool init(SDL_InitFlags init_flags, const std::string& win_name, int w, int h, SDL_WindowFlags window_flags);
    static bool init_audio();
    static bool init_ttf(const fs::path& font_path, float ptsize);
    static bool init_gamepad();

    // SDL
    static void render_texture(SDL_Texture* texture, const SDL_FRect* src = nullptr, const SDL_FRect* dst = nullptr, SDL_FlipMode flip = SDL_FLIP_NONE);
    static SDL_Texture* create_texture(SDL_Surface* surface);
    static SDL_Surface* create_surface(int w, int h, SDL_FColor color = Color::Black, SDL_PixelFormat format = SDL_PIXELFORMAT_RGBA8888);
    static SDL_Texture* create_texture(int w, int h, SDL_FColor color = Color::Black, SDL_TextureAccess access = SDL_TEXTUREACCESS_STATIC, SDL_PixelFormat format = SDL_PIXELFORMAT_RGBA8888);
    static SDL_Texture* create_circle_texture(float radius, SDL_FColor color = Color::Black, float thickness = 1.f);
    static SDL_Texture* create_filled_circle_texture(float radius, SDL_FColor color = Color::Black);

    static void render_rect(const SDL_FRect* dst, SDL_FColor color, float thickness = 1);
    static void render_filled_rect(const SDL_FRect* dst, SDL_FColor color);
    static void render_circle(const SDL_FRect* dst, SDL_FColor color);
    static void render_filled_circle(const SDL_FRect* dst, SDL_FColor color);

    static const char* get_error();
    static SDL_FColor get_texture_color(SDL_Texture* texture);
    static std::pair<float, float> get_texture_size(SDL_Texture* texture);
    static SDL_FPoint get_mouse_position();
    static SDL_Texture* get_render_target();

    static void set_render_vsync(bool vsync);
    static void set_texture_color(SDL_Texture* texture, SDL_FColor color);
    static void set_texture_scalemode(SDL_Texture* texture, SDL_ScaleMode mode);
    static void set_render_color(SDL_FColor color);
    static void set_render_target(SDL_Texture* texture);
    static void set_blend_mode(SDL_Surface* surface, SDL_BlendMode mode);
    static void set_blend_mode(SDL_Texture* texture, SDL_BlendMode mode);

    static SDL_Color fcolor2color(const SDL_FColor& fcolor);

    static void render_clear();
    static void render_present();
    static void hide_cursor();
    static bool point_in_rect(const SDL_FPoint* point, const SDL_FRect* rect);

    static void set_window_position(int x, int y);
    static std::pair<int, int> get_window_size();
    static bool set_window_maximized();

    static void destroy();
    static void destroy(SDL_Texture* texture);
    static void destroy(SDL_Surface* surface);

    // SDL_image
    static SDL_Texture* load_image(const fs::path& path, int w = 0, int h = 0);

    // SDL_mixer

    // SDL_ttf
    static SDL_Surface* create_surface(const std::string& text, float ptsize, SDL_FColor color = Color::Black, int wrap_width = 0);
    static SDL_Texture* create_texture(const std::string& text, float ptsize, SDL_FColor color = Color::Black, int wrap_width = 0);

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

inline bool operator==(const SDL_FColor& lhs, const SDL_FColor& rhs) {
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

}  // namespace sdl
