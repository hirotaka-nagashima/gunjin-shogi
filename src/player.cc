//-----------------------------------------------------------------------------
// Copyright (c) 2016 Hirotaka Nagashima. All rights reserved.
//-----------------------------------------------------------------------------
// Information on this class is described in "player.h".
//-----------------------------------------------------------------------------

#include "player.h"
#include "board.h"
#include "graphic.h"
#include "point.h"

void Player::ReplacePieces() {
  // Display a current board.
  graphic()->DisplayBoard(*board(), *this);
  graphic()->DisplayDeterminingButton();

  // Replace pieces.
  while (true) {
    graphic()->UnhilightSquares(*board(), id());

    // Determine a source and hilight one.
    Move move;
    move.src = graphic()->GetClickedPosition(id());

    // If outside of my board was clicked, end replacing phase.
    if (board()->board(move.src).characters_id != id()) {
      // Skip to read mouse up.
      graphic()->window()->WaitClick(true);

      // End replacing phase.
      std::vector<Point> error;
      if (board()->IsValid(id(), &error))
        break;

      // If there is an unavailable position, display one.
      for (int i = 0; i < static_cast<int>(error.size()); ++i)
        graphic()->CrossCell(error[i], id());
      graphic()->window()->Sleep(300);

      continue;
    }

    // Hilight the piece at the source.
    graphic()->HilightSquare(move.src, id());

    // Determine a destination and hilight one.
    move.dest = graphic()->GetClickedPosition(id());
    if (board()->board(move.dest).characters_id != id())
      continue;
    graphic()->HilightSquare(move.dest, id());
    graphic()->window()->Sleep(50);

    // Swap them and display current state of the board.
    board()->Swap(move);
  }
}

Move Player::MovePiece() {
  // Display first state of the board.
  graphic()->DisplayBoard(*board(), *this);

  // Determine how to move.
  Move move;
  while (true) {
    graphic()->UnhilightSquares(*board(), id());

    // Determine a source.
    move.src = graphic()->GetClickedPosition(id());
    
    // If an opponent's piece was clicked, show supposition menu.
    Board::Piece src_piece = board()->board(move.src);
    if (src_piece.characters_id != id() && src_piece.IsPiece()) {
      Board::Piece supposition = board()->board(move.src);
      supposition.supposition = graphic()->GetSupposition();
      board()->set_board(supposition, move.src);
      graphic()->DisplayBoard(*board(), *this);
      continue;
    }

    // Count the number of the squares the piece can place and hilight them.
    HighlightPlaceableSquares(move.src);

    // If the piece can place nowhere, continue.
    if (graphic()->hilighted_squares().empty())
      continue;

    graphic()->HilightSquare(move.src, id());

    // Determine a destination.
    move.dest = graphic()->GetClickedPosition(id());
    if (board()->IsMoveValid(move))
      break;
  }

  board()->Battle(move);
  return move;
}

void Player::HighlightPlaceableSquares(const Point &src) const {
  Move move;
  move.src = src;
  for (move.dest.y = 0; move.dest.y < Board::kHeight; ++move.dest.y) {
    for (move.dest.x = 0; move.dest.x < Board::kWidth; ++move.dest.x) {
      // Hilight a square the piece can move to.
      if (board()->IsMoveValid(move))
        graphic()->HilightSquare(move.dest, id());
    }
  }
}