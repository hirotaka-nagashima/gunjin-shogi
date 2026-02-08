//-----------------------------------------------------------------------------
// Copyright (c) 2016 Hirotaka Nagashima. All rights reserved.
//-----------------------------------------------------------------------------
// This class is used for drawing image and text to a window.
//-----------------------------------------------------------------------------

#ifndef SDL_GRAPHIC_H_
#define SDL_GRAPHIC_H_

#include <vector>
#include "window.h"
#include "board.h"

struct Point;
class Character;
class Board;
class Graphic {
public:
  Graphic() { window_ = new Window(350, 450, "Stratego"); }
  ~Graphic() { delete window_; }

  void Initialize();
  void Terminate();
  Point GetClickedPosition(int characters_id);
  void DisplayResult(const Board &board, const std::string &winners_name,
                     bool game_was_drawn);
  void DisplayBoard(const Board &board, const Character &character);
  void WaitNextPlayer(const std::string &name);
  void HilightSquare(const Point &p, int characters_id);
  void UnhilightSquares(const Board &board, int character_id);
  void CrossCell(const Point &p, int characters_id);
  void DisplayDeterminingButton();
  // Use this instead of "DisplayBoard()" for lighter processing.
  void DisplayPiece(const Board &board, const Point &point, int characters_id);
  Board::Piece::KindPiece GetSupposition();

  Window *window() { return window_; }
  std::vector<Point> &hilighted_squares() { return hilighted_squares_; }

private:
  void DisplaySuppositionMenu();
  void DrawPiece(bool piece_is_current_characters,
                 int dest_x, int dest_y, const Board::Piece &piece);

  std::vector<Point> hilighted_squares_;
  Window *window_;

  // Resources.
  TTF_Font *font_;
  TTF_Font *small_font_;
  TTF_Font *smaller_font_;
  SDL_Surface *image_board_;
  SDL_Surface *image_piece_;
  SDL_Surface *image_label_;
  SDL_Surface *image_supposition_label_;
  SDL_Surface *image_overlay_;
  SDL_Surface *image_hilight_;
  SDL_Surface *image_determining_button_;
  SDL_Surface *image_cross_;
};

#endif  // SDL_GRAPHIC_H_