#include <sdl/sdl.hpp>

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

void SDL::set_window_position(int x, int y) {
    SDL_SetWindowPosition(window_, x, y);
}

void SDL::set_render_vsync(bool vsync) {
    SDL_SetRenderVSync(renderer_, vsync);
}

void SDL::set_color(SDL_Color color) {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
}

SDL_Texture* SDL::load_image(const std::filesystem::path& path, int w, int h) {
    auto* texture = IMG_LoadTexture(renderer_, path.c_str());
    if (w && h) {
        auto* scaled_texture = create_texture(w, h, BLACK, SDL_TEXTUREACCESS_TARGET);
        set_render_target(scaled_texture);
        render_texture(texture, nullptr, nullptr);
        destroy(texture);
        return scaled_texture;
    }
    return texture;
}

void SDL::render_texture(SDL_Texture* texture, const SDL_FRect* src, const SDL_FRect* dst) {
    SDL_RenderTexture(renderer_, texture, src, dst);
}

SDL_Surface* SDL::create_surface(int w, int h, SDL_Color color, SDL_PixelFormat format) {
    SDL_Surface* surface = SDL_CreateSurface(w, h, format);
    if (color != BLACK) {
        SDL_FillSurfaceRect(surface, nullptr, SDL_MapSurfaceRGBA(surface, color.r, color.g, color.b, color.a));
    }
    return surface;
}

SDL_Surface* SDL::create_surface(const std::string& text, float ptsize, SDL_Color color, int wrap_width) {
    set_font_size(ptsize);
    if (wrap_width) {
        return TTF_RenderText_Blended_Wrapped(font_, text.c_str(), 0, color, wrap_width);
    } else {
        return TTF_RenderText_Blended(font_, text.c_str(), 0, color);
    }
}

SDL_Texture* SDL::create_texture(SDL_Surface* surface) {
    return SDL_CreateTextureFromSurface(renderer_, surface);
}

SDL_Texture* SDL::create_texture(const std::string& text, float ptsize, SDL_Color color, int wrap_len) {
    SDL_Surface* surface = create_surface(text, ptsize, color, wrap_len);
    SDL_Texture* texture = create_texture(surface);
    destroy(surface);
    return texture;
}

void SDL::set_font_size(float ptsize) {
    TTF_SetFontSize(font_, ptsize);
}

SDL_Texture* SDL::create_texture(int w, int h, SDL_Color color, SDL_TextureAccess access, SDL_PixelFormat format) {
    access = static_cast<SDL_TextureAccess>(access | SDL_TEXTUREACCESS_TARGET);
    SDL_Texture* texture = SDL_CreateTexture(renderer_, format, access, w, h);
    set_render_target(texture);
    set_color(color);
    render_clear();
    set_render_target(nullptr);

    return texture;
}

void SDL::render_fill_rect(const SDL_FRect* dst, SDL_Color color) {
    set_color(color);
    SDL_RenderFillRect(renderer_, dst);
}

void SDL::render_rect(const SDL_FRect* dst, SDL_Color color) {
    set_color(color);
    SDL_RenderRect(renderer_, dst);
}

void SDL::set_blend_mode(SDL_Surface* surface, SDL_BlendMode mode) {
    SDL_SetSurfaceBlendMode(surface, mode);
}

void SDL::set_blend_mode(SDL_Texture* texture, SDL_BlendMode mode) {
    SDL_SetTextureBlendMode(texture, mode);
}

void SDL::set_render_target(SDL_Texture* texture) {
    SDL_SetRenderTarget(renderer_, texture);
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

void SDL::render_clear() {
    SDL_RenderClear(renderer_);
}

void SDL::render_present() {
    SDL_RenderPresent(renderer_);
}

}  // namespace sdl
