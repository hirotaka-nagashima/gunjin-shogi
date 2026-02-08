//-----------------------------------------------------------------------------
// Copyright (c) 2016 Hirotaka Nagashima. All rights reserved.
//-----------------------------------------------------------------------------
// Information on this class is described in "graphic.h".
//-----------------------------------------------------------------------------

#include "graphic.h"
#include <algorithm>
#include "character.h"
#include "point.h"
#include "board.h"

void Graphic::Initialize() {
  window_->Initialize();

  // Load images.
  image_board_ = IMG_Load("resources/board.png");
  image_piece_ = IMG_Load("resources/piece.png");
  image_label_ = IMG_Load("resources/label.png");
  image_supposition_label_ = IMG_Load("resources/supposition_label.png");
  image_overlay_ = IMG_Load("resources/overlay.png");
  image_hilight_ = IMG_Load("resources/hilight.png");
  image_determining_button_ = IMG_Load("resources/determining_button.png");
  image_cross_ = IMG_Load("resources/cross.png");
  if (!image_board_ || !image_piece_ || !image_label_ ||
      !image_supposition_label_ || !image_overlay_ || !image_hilight_ ||
      !image_determining_button_ || !image_cross_) {
    fprintf(stderr, "ERROR: %s\n", IMG_GetError());
    exit(-1);
  }
  
  // Load fonts.
  font_ = TTF_OpenFont("resources/font.ttf", 50);
  small_font_ = TTF_OpenFont("resources/font.ttf", 25);
  smaller_font_ = TTF_OpenFont("resources/font.ttf", 20);
  if (!font_ || !small_font_) {
    fprintf(stderr, "ERROR: %s\n", TTF_GetError());
    exit(-1);
  }
}

void Graphic::Terminate() {
  // Release resources.
  SDL_FreeSurface(image_board_);
  SDL_FreeSurface(image_piece_);
  SDL_FreeSurface(image_label_);
  SDL_FreeSurface(image_supposition_label_);
  SDL_FreeSurface(image_overlay_);
  SDL_FreeSurface(image_hilight_);
  SDL_FreeSurface(image_determining_button_);
  SDL_FreeSurface(image_cross_);
  TTF_CloseFont(font_);
  TTF_CloseFont(small_font_);
  TTF_CloseFont(smaller_font_);

  window_->Terminate();
}

Point Graphic::GetClickedPosition(int characters_id) {
  // Wait for a mouse event.
  SDL_Event event;
  SDL_WaitEvent(&event);

  // Get the clicked coordinates.
  Point clicked_coordinates = window()->WaitClick(true);

  // Calculate clicked id of board.
  Point clicked_position;
  clicked_position.y = clicked_coordinates.y / 50 - 1;
  clicked_position.x = clicked_coordinates.x / 50;

  // Rotate the clicked position 180 degrees if a board is rotated.
  if (characters_id == 0) {
    clicked_position.y = Board::kHeight - 1 - clicked_position.y;
    clicked_position.x = Board::kWidth - 1 - clicked_position.x;
  }

  // If outside of the board was clicked,
  // set the nearest valid piece as clicked coordinates.
  clicked_position.y = std::min(Board::kHeight - 1,
                                std::max(clicked_position.y, 0));
  clicked_position.x = std::min(Board::kWidth - 1,
                                std::max(clicked_position.x, 0));

  return clicked_position;
}

void Graphic::DisplayResult(const Board &board,
                            const std::string &winners_name,
                            bool game_was_drawn) {
  // Overlay.
  window_->DrawSingleImage(image_overlay_, 0, 0);
  hilighted_squares().clear();

  // Draw winner's name.
  if (game_was_drawn) {
    window_->DrawStringCenter("Draw! :(", 200, font_);
  } else {
    window_->DrawStringCenter(winners_name.c_str(), 160, font_);
    window_->DrawStringCenter("win! :)", 230, font_);
  }

  // Display.
  SDL_Flip(window_->video_surface());

  // Wait.
  window_->WaitClick(false);
}

void Graphic::DisplayBoard(const Board &board, const Character &character) {
  // Draw a background.
  window_->DrawSingleImage(image_board_, 0, 0);
  hilighted_squares().clear();

  // Draw a player's name at the top of the screen.
  window_->DrawString(character.name().c_str(), 10, -5, font_);

  // Draw a player's score.
  char score[10];
  sprintf_s(score, "%+2d", character.score());
  window_->DrawString(score, 305, 10, smaller_font_);

  // Draw pieces.
  Point point;
  for (point.y = 0; point.y < Board::kHeight; ++point.y) {
    for (point.x = 0; point.x < Board::kWidth; ++point.x) {
      // Skip a invisible piece.
      Board::Piece piece = board.board(point);
      if (!piece.IsPiece() || board.IsDummyHeadquarters(point))
        continue;

      // Calculate destination to draw the piece.
      Point dest;
      if (board.ExistHeadquartersAt(point)) {
        if (character.id() == 0)
          dest.y = 50 * (Board::kHeight - 1 - point.y) + 50;
        else
          dest.y = 50 * point.y + 50;
        dest.x = 50 * Board::kWidth / 2 - 25;
      } else {
        if (character.id() == 0) {
          // If a board was displayed with rotated 180 degrees,
          // rotate coordinates 180 degrees.
          dest.y = 50 * (Board::kHeight - 1 - point.y) + 50;
          dest.x = 50 * (Board::kWidth - 1 - point.x);
        } else {
          dest.y = 50 * point.y + 50;
          dest.x = 50 * point.x;
        }
      }

      bool piece_is_current_characters =
          (piece.characters_id == character.id());
      DrawPiece(piece_is_current_characters, dest.x, dest.y, piece);
    }
  }

  SDL_Flip(window_->video_surface());
}

void Graphic::WaitNextPlayer(const std::string &name) {
  window()->ClearScreen();
  hilighted_squares().clear();

  // Draw next player's name.
  std::string temp = name + "?";
  window_->DrawStringCenter("Are you", 140, small_font_);
  window_->DrawStringCenter(temp.c_str(), 170, font_);
  window_->DrawStringCenter("If so, click anywhere.", 260, small_font_);

  // Display.
  SDL_Flip(window_->video_surface());

  // Wait.
  window()->WaitClick(false);
}

void Graphic::HilightSquare(const Point &point, int characters_id) {
  hilighted_squares().push_back(point);

  // Calculate destination to draw the piece.
  Point dest;
  if (characters_id == 0) {
    // If a board was displayed with rotated 180 degrees,
    // rotate coordinates 180 degrees.
    dest.y = 50 * (Board::kHeight - 1 - point.y) + 50;
    dest.x = 50 * (Board::kWidth - 1 - point.x);
  } else {
    dest.y = 50 * point.y + 50;
    dest.x = 50 * point.x;
  }

  // Draw square overlay.
  window_->DrawSingleImage(image_hilight_, dest.x, dest.y);
  
  // Display.
  SDL_Flip(window_->video_surface());
}

void Graphic::UnhilightSquares(const Board &board, int character_id) {
  // Draw a square over the hilighted square.
  for (int i = 0; i < static_cast<int>(hilighted_squares().size()); ++i)
    DisplayPiece(board, hilighted_squares().at(i), character_id);

  hilighted_squares().clear();
}

void Graphic::CrossCell(const Point &point, int characters_id) {
  hilighted_squares().push_back(point);

  // Calculate destination to draw the piece.
  Point dest;
  if (characters_id == 0) {
    // If a board was displayed with rotated 180 degrees,
    // rotate coordinates 180 degrees.
    dest.y = 50 * (Board::kHeight - 1 - point.y) + 50;
    dest.x = 50 * (Board::kWidth - 1 - point.x);
  } else {
    dest.y = 50 * point.y + 50;
    dest.x = 50 * point.x;
  }

  // Draw square overlay.
  window_->DrawSingleImage(image_cross_, dest.x, dest.y);

  // Display.
  SDL_Flip(window_->video_surface());
}

void Graphic::DisplayDeterminingButton() {
  window_->DrawSingleImage(image_determining_button_, 0, 50);
  SDL_Flip(window_->video_surface());
}

void Graphic::DisplayPiece(const Board &board, const Point &point,
                           int characters_id) {
  Board::Piece piece = board.board(point);
  Board::Piece::KindPiece kind_piece = piece.piece;
  bool piece_is_current_characters = (characters_id == piece.characters_id);

  // Draw the piece of background and the piece.
  if (board.IsDummyHeadquarters(point) || board.ExistHeadquartersAt(point)) {
    // If the size of squares is twice as large as. ->
    // Calculate destination to draw the piece.
    // If a board was displayed with rotated 180 degrees,
    // rotate coordinates 180 degrees.
    Point dest;
    dest.y = 50 + 50 * ((characters_id == 0) ?
                        (Board::kHeight - 1 - point.y) : point.y);
    dest.x = 50 * (Board::kWidth / 2 - 1);

    // Calculate id.
    int y = dest.y / 50;
    int x = dest.x / 50;
    int id = y * (Board::kWidth + 1) + x;

    // Draw the part of the background and the piece.
    window_->DrawImage(image_board_, dest.x, dest.y, id, 50, 50);
    window_->DrawImage(image_board_, dest.x + 50, dest.y, id + 1, 50, 50);
    DrawPiece(piece_is_current_characters, dest.x + 25, dest.y,
              board.board(point));
  } else {
    // Calculate destination to draw the piece.
    Point dest;
    if (characters_id == 0) {
      // If a board was displayed with rotated 180 degrees,
      // rotate coordinates 180 degrees.
      dest.y = 50 * (Board::kHeight - 1 - point.y) + 50;
      dest.x = 50 * (Board::kWidth - 1 - point.x);
    } else {
      dest.y = 50 * point.y + 50;
      dest.x = 50 * point.x;
    }

    // Calculate id.
    int y = dest.y / 50;
    int x = dest.x / 50;
    int id = y * (Board::kWidth + 1) + x;

    // Draw the part of the background and the piece.
    window_->DrawImage(image_board_, dest.x, dest.y, id, 50, 50);
    DrawPiece(piece_is_current_characters, dest.x, dest.y, piece);
  }

  // Display.
  SDL_Flip(window_->video_surface());
}

Board::Piece::KindPiece Graphic::GetSupposition() {
  DisplaySuppositionMenu();

  // Wait for a mouse event.
  SDL_Event event;
  SDL_WaitEvent(&event);

  // Get the clicked coordinates.
  Point clicked_coordinates = window()->WaitClick(true);

  // Calculate clicked id of board.
  Point clicked_position;
  clicked_position.y = (clicked_coordinates.y - 125) / 50;
  clicked_position.y = (clicked_coordinates.y < 125) ? -1 : clicked_position.y;
  clicked_position.x = (clicked_coordinates.x - 75) / 50;
  clicked_position.x = (clicked_coordinates.x < 75) ? -1 : clicked_position.x;
  Point prev_clicked_position = clicked_position;

  // If no piece was clicked,
  // set the nearest valid piece as clicked coordinates.
  int num_row_pieces = 4;
  int num_column_pieces = Board::Piece::kNumKindPieces / num_row_pieces;
  clicked_position.y = std::min(num_column_pieces - 1,
                                std::max(clicked_position.y, 0));
  clicked_position.x = std::min(num_row_pieces - 1,
                                std::max(clicked_position.x, 0));

  // If any piece was clicked.
  if (prev_clicked_position.Equals(clicked_position)) {
    int kind_piece = clicked_position.y * num_row_pieces + clicked_position.x;
    return static_cast<Board::Piece::KindPiece>(kind_piece);
  }

  return Board::Piece::kNone;
}

void Graphic::DisplaySuppositionMenu() {
  window()->DrawSingleImage(image_overlay_, 0, 0);

  // Draw all kinds of pieces.
  for (int i = 0; i < Board::Piece::kNumKindPieces; ++i) {
    Point dest;
    dest.y = (i / 4) * 50 + 125;
    dest.x = (i % 4) * 50 + 75;
    window()->DrawImage(image_label_, dest.x, dest.y, i, 50, 50);
  }

  // Display.
  SDL_Flip(window_->video_surface());
}

void Graphic::DrawPiece(bool piece_is_current_characters,
                        int dest_x, int dest_y, const Board::Piece &piece) {
  if (!piece.IsPiece())
    return;
  if (piece_is_current_characters) {
    window_->DrawImage(image_piece_, dest_x, dest_y, 0, 50, 50);
    window_->DrawImage(image_label_, dest_x, dest_y, piece.piece, 50, 50);
  } else {
    window_->DrawImage(image_piece_, dest_x, dest_y, 1, 50, 50);
    if (piece.supposition != Board::Piece::kNone) {
      window_->DrawImage(image_supposition_label_, dest_x, dest_y,
                         piece.supposition, 50, 50);
    }
  }
}