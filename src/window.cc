//-----------------------------------------------------------------------------
// Copyright (c) 2016 Hirotaka Nagashima. All rights reserved.
//-----------------------------------------------------------------------------
// Information on this class is described in "window.h".
//-----------------------------------------------------------------------------

#include "window.h"
#include <iostream>
#include <cassert>
#include "point.h"

void Window::Initialize() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "ERROR: " << SDL_GetError() << std::endl;
    exit(-1);
  }
  if (TTF_Init() < 0) {
    std::cerr << "ERROR: " << TTF_GetError() << std::endl;
    exit(-1);
  }
  window_ = SDL_CreateWindow(kTitleName.c_str(), SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, kWidth, kHeight, 0);
  if (!window_) {
    std::cerr << "ERROR: " << SDL_GetError() << std::endl;
    exit(-1);
  }
  video_surface_ = SDL_GetWindowSurface(window_);
}

void Window::Terminate() {
  SDL_DestroyWindow(window_);
  window_ = nullptr;
  video_surface_ = nullptr;
  TTF_Quit();
  SDL_Quit();
}

void Window::UpdateSurface() const {
  SDL_UpdateWindowSurface(window_);
}

void Window::ClearScreen() {
  SDL_FillRect(video_surface_, nullptr, 0);
}

void Window::DrawSingleImage(SDL_Surface *image, int dest_x, int dest_y) {
  SDL_Rect src, dest;
  src.y = src.x = 0;
  src.w = image->w;
  src.h = image->h;
  dest.x = dest_x;
  dest.y = dest_y;
  SDL_BlitSurface(image, &src, video_surface_, &dest);
}

void Window::DrawImage(SDL_Surface *image, int dest_x, int dest_y,
                       int id, int width, int height) {
  SDL_Rect src, dest;
  src.x = (id % (image->w / width)) * width;
  src.y = (id / (image->w / width)) * height;
  src.w = width;
  src.h = height;
  dest.x = dest_x;
  dest.y = dest_y;
  SDL_BlitSurface(image, &src, video_surface_, &dest);
}

void Window::DrawString(const char *text, int dest_x, int dest_y,
                        TTF_Font *font, const SDL_Color &color) {
  SDL_Surface *temp_text = TTF_RenderUTF8_Blended(font, text, color);
  SDL_Rect src, dest;
  src.x = 0;
  src.y = 0;
  src.w = temp_text->w;
  src.h = temp_text->h;
  dest.x = dest_x;
  dest.y = dest_y;
  SDL_BlitSurface(temp_text, &src, video_surface_, &dest);
  SDL_FreeSurface(temp_text);
}

void Window::DrawStringCenter(const char *text, int dest_y,
                              TTF_Font *font, const SDL_Color &color) {
  SDL_Surface *temp_text = TTF_RenderUTF8_Blended(font, text, color);
  SDL_Rect src, dest;
  src.x = 0;
  src.y = 0;
  src.w = temp_text->w;
  src.h = temp_text->h;
  dest.x = (kWidth - temp_text->w) / 2;
  dest.y = dest_y;
  SDL_BlitSurface(temp_text, &src, video_surface_, &dest);
  SDL_FreeSurface(temp_text);
}

void Window::Sleep(int duration) const {
  // Divide duration by 10 for lighter processing.
  for (int i = 0; i < duration / 10; ++i) {
    SDL_Delay(10);
    CheckClose();
  }
}

void Window::CheckClose() const {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT)
      exit(0);
  }
}

Point Window::WaitClick(bool distinguish_mouse_down) {
  // Get the current state of the mouse button.
  while (true) {
    SDL_Event event;
    SDL_WaitEvent(&event);
    if (event.type == SDL_QUIT)
      exit(0);
    if ((event.type == SDL_MOUSEBUTTONDOWN && distinguish_mouse_down) ||
        event.type == SDL_MOUSEBUTTONUP) {
      // Return clicked coordinates.
      Point point;
      point.y = event.button.y;
      point.x = event.button.x;
      return point;
    }
  }
}

void Window::WaitEnterKey() const {
  while (true) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        exit(0);
      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN)
        return;
    }
    SDL_Delay(10);
  }
}