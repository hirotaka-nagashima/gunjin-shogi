//-----------------------------------------------------------------------------
// Copyright (c) 2016 Hirotaka Nagashima. All rights reserved.
//-----------------------------------------------------------------------------
// This class is used for drawing image and text to a window.
//-----------------------------------------------------------------------------

#ifndef GUNJIN_SHOGI_CHARACTOR_H_
#define GUNJIN_SHOGI_CHARACTOR_H_

#include <string>
#include "board.h"

struct Point;
class Character {
public:
  enum CharacterType {
    kPlayer,
    kAi,
  };

  Character(CharacterType type, Board *board, int id, const std::string &name)
      : type_(type),
        board_(board),
        kId(id),
        kOpponentsId(1 - id),
        kName(name),
        score_(0) {}
  virtual ~Character() {}

  virtual Move MovePiece() = 0;
  virtual void ReplacePieces() = 0;
  void UpdateScore() {
    int num_my_pieces = board()->CountNumPieces(id());
    int num_opponents_pieces = board()->CountNumPieces(opponents_id());
    int difference = num_my_pieces - num_opponents_pieces;
    score_ = difference;
  }

  int id() const { return kId; }
  int opponents_id() const { return kOpponentsId; }
  std::string name() const { return kName; }
  int score() const { return score_; }
  CharacterType type() const { return type_; }

protected:
  Board *board() const { return board_; }

  const int kId;
  const int kOpponentsId;
  const std::string kName;
  Board * const board_;
  int score_;
  CharacterType type_;
};

#endif  // GUNJIN_SHOGI_CHARACTOR_H_