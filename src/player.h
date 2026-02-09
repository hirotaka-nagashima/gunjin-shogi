//-----------------------------------------------------------------------------
// Copyright (c) 2016 Hirotaka Nagashima. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef GUNJIN_SHOGI_PLAYER_H_
#define GUNJIN_SHOGI_PLAYER_H_

#include <string>
#include "character.h"

struct Point;
class Graphic;
class Board;
class Player : public Character {
public:
  Player(Graphic *graphic, Board *board, int id, const std::string &name)
      : Character(kPlayer, board, id, name),
        graphic_(graphic) {}

  void ReplacePieces();
  Move MovePiece();

private:
  void HighlightPlaceableSquares(const Point &src) const;
  Graphic * const graphic() const { return graphic_; }

  Graphic * const graphic_;
};

#endif  // GUNJIN_SHOGI_PLAYER_H_