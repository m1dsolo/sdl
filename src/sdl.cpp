#include "SDL3/SDL_render.h"
#include <sdl/sdl.hpp>

#include <numbers>
#include <cmath>

namespace sdl {

bool SDL::init(SDL_InitFlags init_flags, const std::string& win_name, int w, int h, SDL_WindowFlags window_flags) {
    bool res = SDL_Init(init_flags);
    window_ = SDL_CreateWindow(win_name.c_str(), w, h, window_flags);
    renderer_ = SDL_CreateRenderer(window_, nullptr);

    return res && window_ && renderer_;
}

// TODO
bool SDL::init_audio() {
    bool res = MIX_Init();

    return res;
}

bool SDL::init_ttf(const std::filesystem::path& font_path, float font_size) {
    bool res = TTF_Init();
    font_ = TTF_OpenFont(font_path.c_str(), font_size);
    return res && font_;
}

bool SDL::init_gamepad() {
    SDL_JoystickID* joystick_id = SDL_GetGamepads(nullptr);
    if (!joystick_id) {
        return false;
    }
    gamepad_ = SDL_OpenGamepad(*joystick_id);

    return true;
}

void SDL::destroy() {
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    TTF_CloseFont(font_);

    TTF_Quit();
    MIX_Quit();
    SDL_Quit();
}

void SDL::hide_cursor() {
    SDL_HideCursor();
}

void SDL::set_render_vsync(bool vsync) {
    SDL_SetRenderVSync(renderer_, vsync);
}

void SDL::set_texture_color(SDL_Texture* texture, SDL_FColor color) {
    SDL_SetTextureColorModFloat(texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaModFloat(texture, color.a);
}

void SDL::set_texture_scalemode(SDL_Texture* texture, SDL_ScaleMode mode) {
    SDL_SetTextureScaleMode(texture, mode);
}

void SDL::set_render_color(SDL_FColor color) {
    SDL_SetRenderDrawColorFloat(renderer_, color.r, color.g, color.b, color.a);
}

SDL_Texture* SDL::load_image(const std::filesystem::path& path, int w, int h) {
    auto* texture = IMG_LoadTexture(renderer_, path.c_str());
    if (w && h) {
        auto* scaled_texture = create_texture(w, h, Color::Black, SDL_TEXTUREACCESS_TARGET);
        set_render_target(scaled_texture);
        render_texture(texture, nullptr, nullptr);
        destroy(texture);
        return scaled_texture;
    }
    return texture;
}

void SDL::render_texture(SDL_Texture* texture, const SDL_FRect* src, const SDL_FRect* dst, SDL_FlipMode flip) {
    if (flip != SDL_FLIP_NONE) {
        SDL_RenderTextureRotated(renderer_, texture, src, dst, 0.0, nullptr, flip);
    } else {
        SDL_RenderTexture(renderer_, texture, src, dst);
    }
}

SDL_Surface* SDL::create_surface(int w, int h, SDL_FColor color, SDL_PixelFormat format) {
    SDL_Surface* surface = SDL_CreateSurface(w, h, format);
    if (color != Color::Black) {
        SDL_Color color_uint8 = fcolor2color(color);
        SDL_FillSurfaceRect(surface, nullptr, SDL_MapSurfaceRGBA(surface, color_uint8.r, color_uint8.g, color_uint8.b, color_uint8.a));
    }
    return surface;
}

SDL_Surface* SDL::create_surface(const std::string& text, float ptsize, SDL_FColor color, int wrap_width) {
    set_font_size(ptsize);
    SDL_Color color_uint8 = fcolor2color(color);
    if (wrap_width) {
        return TTF_RenderText_Blended_Wrapped(font_, text.c_str(), 0, color_uint8, wrap_width);
    } else {
        return TTF_RenderText_Blended(font_, text.c_str(), 0, color_uint8);
    }
}

SDL_Texture* SDL::create_texture(SDL_Surface* surface) {
    return SDL_CreateTextureFromSurface(renderer_, surface);
}

SDL_Texture* SDL::create_texture(const std::string& text, float ptsize, SDL_FColor color, int wrap_len) {
    SDL_Surface* surface = create_surface(text, ptsize, color, wrap_len);
    SDL_Texture* texture = create_texture(surface);
    destroy(surface);
    return texture;
}

void SDL::render_text(SDL_Texture* texture, float x, float y, const std::string& text, float ptsize, SDL_FColor color, int wrap_width) {
    auto tmp_texture = create_texture(text, ptsize, color, wrap_width);
    RenderTargetGuard guard{texture};
    auto [w, h] = get_texture_size(tmp_texture);
    SDL_FRect dst = {x, y, w, h};
    render_texture(tmp_texture, nullptr, &dst);
    destroy(tmp_texture);
}

void SDL::set_font_size(float ptsize) {
    TTF_SetFontSize(font_, ptsize);
}

SDL_Texture* SDL::create_texture(int w, int h, SDL_FColor color, SDL_TextureAccess access, SDL_PixelFormat format) {
    access = static_cast<SDL_TextureAccess>(access | SDL_TEXTUREACCESS_TARGET);
    SDL_Texture* texture = SDL_CreateTexture(renderer_, format, access, w, h);
    RenderTargetGuard guard{texture};
    set_render_color(color);
    render_clear();

    return texture;
}

SDL_Texture* SDL::create_circle_texture(float radius, SDL_FColor color, float thichness) {
    auto size = radius * 2;
    auto texture = create_texture(size, size, Color::Transparent, SDL_TEXTUREACCESS_TARGET);
    set_blend_mode(texture, SDL_BLENDMODE_BLEND);
    auto target = RenderTargetGuard{texture};
    SDL_FRect dst = {0.f, 0.f, size - 1.f, size - 1.f};
    render_filled_circle(&dst, color);
    dst = {thichness, thichness, size - 1.f - thichness * 2.f, size - 1.f - thichness * 2.f};
    render_filled_circle(&dst, Color::Transparent);
    return texture;
}

SDL_Texture* SDL::create_filled_circle_texture(float radius, SDL_FColor color) {
    auto size = radius * 2;
    auto texture = create_texture(size, size, Color::Transparent, SDL_TEXTUREACCESS_TARGET);
    set_blend_mode(texture, SDL_BLENDMODE_BLEND);
    auto target = RenderTargetGuard{texture};
    SDL_FRect dst = {0.f, 0.f, size - 1.f, size - 1.f};
    render_filled_circle(&dst, color);
    return texture;
}

void SDL::render_rect(const SDL_FRect* dst, SDL_FColor color, float thickness) {
    if (thickness == 1) {
        set_render_color(color);
        SDL_RenderRect(renderer_, dst);
    } else {
        SDL_FRect top = {dst->x, dst->y, dst->w, thickness};
        render_filled_rect(&top, color);

        SDL_FRect bottom = {dst->x, dst->y + dst->h - thickness, dst->w, thickness};
        render_filled_rect(&bottom, color);

        SDL_FRect left = {dst->x, dst->y + thickness, thickness, dst->h - 2*thickness};
        render_filled_rect(&left, color);

        SDL_FRect right = {dst->x + dst->w - thickness, dst->y + thickness, thickness, dst->h - 2*thickness};
        render_filled_rect(&right, color);
    }
}

void SDL::render_filled_rect(const SDL_FRect* dst, SDL_FColor color) {
    set_render_color(color);
    SDL_RenderFillRect(renderer_, dst);
}

void SDL::render_circle(const SDL_FRect* dst, SDL_FColor color) {
    set_render_color(color);
    
    float center_x = dst->x + dst->w / 2.0f;
    float center_y = dst->y + dst->h / 2.0f;
    float radius = std::min(dst->w, dst->h) / 2.0f;
    
    const int segments = 64;
    SDL_FPoint points[segments + 1];
    
    for (int i = 0; i <= segments; ++i) {
        float angle = (i * 2.0f * std::numbers::pi_v<float>) / segments;
        points[i].x = center_x + std::cos(angle) * radius;
        points[i].y = center_y + std::sin(angle) * radius;
    }
    
    SDL_RenderLines(renderer_, points, segments + 1);
}

void SDL::render_filled_circle(const SDL_FRect* dst, SDL_FColor color) {
    set_render_color(color);

    const float center_x = dst->x + dst->w / 2.0f;
    const float center_y = dst->y + dst->h / 2.0f;
    const float radius = std::min(dst->w, dst->h) / 2.0f;

    const int segments = 64;
    
    SDL_Vertex vertices[segments + 1];
    
    vertices[0] = {center_x, center_y};

    for (int i = 0; i < segments; ++i) {
        const float angle = (i * 2.0f * std::numbers::pi_v<float>) / segments;
        vertices[i + 1].position.x = center_x + std::cos(angle) * radius;
        vertices[i + 1].position.y = center_y + std::sin(angle) * radius;
        vertices[i + 1].color = color;
        vertices[i + 1].tex_coord = {0.f, 0.f};
    }

    int indexs[segments * 3];
    for (int i = 0; i < segments; ++i) {
        indexs[i * 3] = 0;
        indexs[i * 3 + 1] = i + 1;
        indexs[i * 3 + 2] = (i + 1) % segments + 1;
    }

    SDL_RenderGeometry(
        renderer_,
        nullptr,
        vertices,
        segments + 1,
        indexs,
        segments * 3
    );
}

void SDL::set_blend_mode(SDL_Surface* surface, SDL_BlendMode mode) {
    SDL_SetSurfaceBlendMode(surface, mode);
}

void SDL::set_blend_mode(SDL_Texture* texture, SDL_BlendMode mode) {
    SDL_SetTextureBlendMode(texture, mode);
}

SDL_Color SDL::fcolor2color(const SDL_FColor& fcolor) {
    return SDL_Color{
        static_cast<Uint8>(fcolor.r * 255.0f + 0.5f),
        static_cast<Uint8>(fcolor.g * 255.0f + 0.5f),
        static_cast<Uint8>(fcolor.b * 255.0f + 0.5f),
        static_cast<Uint8>(fcolor.a * 255.0f + 0.5f)
    };
}

void SDL::set_render_target(SDL_Texture* texture) {
    SDL_SetRenderTarget(renderer_, texture);
}

SDL_FColor SDL::get_texture_color(SDL_Texture* texture) {
    SDL_FColor color;
    SDL_GetTextureColorModFloat(texture, &color.r, &color.g, &color.b);
    SDL_GetTextureAlphaModFloat(texture, &color.a);
    return color;
}

std::pair<float, float> SDL::get_texture_size(SDL_Texture* texture) {
    float w, h;
    SDL_GetTextureSize(texture, &w, &h);
    return {w, h};
}

SDL_FPoint SDL::get_mouse_position() {
    float x, y;
    SDL_GetMouseState(&x, &y);
    return {x, y};
}

const char* SDL::get_error() {
    return SDL_GetError();
}

SDL_Texture* SDL::get_render_target() {
    return SDL_GetRenderTarget(renderer_);
}

void SDL::destroy(SDL_Texture* texture) {
    SDL_DestroyTexture(texture);
}

void SDL::destroy(SDL_Surface* surface) {
    SDL_DestroySurface(surface);
}

bool SDL::point_in_rect(const SDL_FPoint* point, const SDL_FRect* rect) {
    return SDL_PointInRectFloat(point, rect);
}

void SDL::set_window_position(int x, int y) {
    SDL_SetWindowPosition(window_, x, y);
}

std::pair<int, int> SDL::get_window_size() {
    int w, h;
    SDL_GetWindowSize(window_, &w, &h);
    return {w, h};
}

bool SDL::set_window_maximized() {
    return SDL_MaximizeWindow(window_);
}

void SDL::render_clear() {
    SDL_RenderClear(renderer_);
}

void SDL::render_present() {
    SDL_RenderPresent(renderer_);
}

}  // namespace sdl
