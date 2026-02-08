//-----------------------------------------------------------------------------
// Copyright (c) 2016 Hirotaka Nagashima. All rights reserved.
//-----------------------------------------------------------------------------
// This class is used for gui program based on sdl.
// You shouldn't edit this file.
//-----------------------------------------------------------------------------

#ifndef SDL_WINDOW_H_
#define SDL_WINDOW_H_

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

struct Point;
class Window {
public:
  Window(int width, int height, std::string title)
      : kWidth(width),
        kHeight(height),
        kTitleName(title) {}

  void Initialize();
  void Terminate();
  void ClearScreen();
  void DrawSingleImage(SDL_Surface *image, int dest_x, int dest_y);
  void DrawImage(SDL_Surface *image, int dest_x, int dest_y,
                 int id, int width, int height);
  void DrawString(const char *text, int dest_x, int dest_y, TTF_Font *font,
                  const SDL_Color &color = {0xff, 0xff, 0xff});
  void DrawStringCenter(const char *text, int dest_y, TTF_Font *font,
                        const SDL_Color &color = {0xff, 0xff, 0xff});
  void Sleep(int duration) const;
  // Terminate the process if the window is closed.
  void CheckClose() const;
  // Returns clicked coordinate.
  Point WaitClick(bool distinguish_mouse_down);
  // Wait till enter key is pressed.
  void WaitEnterKey() const;

  SDL_Surface *video_surface() const { return video_surface_; }

private:
  const int kWidth;
  const int kHeight;
  const std::string kTitleName;
  SDL_Surface *video_surface_;
};

#endif  // SDL_WINDOW_H_