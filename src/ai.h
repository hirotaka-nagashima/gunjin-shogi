//-----------------------------------------------------------------------------
// Copyright (c) 2016 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef STRATEGO_AI_H_
#define STRATEGO_AI_H_

#include <string>
#include <vector>
#include "character.h"

struct Point;
class Board;
class Ai : public Character {
public:
  Ai(Board *board, int id, const std::string &name)
      : Character(kAi, board, id, name) {}

  void ReplacePieces();
  Move MovePiece();

protected:
  static const int kMaxTimesSwapPiecesRandomly;
  static const char *kFormationFileUrl;

  int EvaluateBoard() const;
  void SupposeOpponentsFormation(const Board::Piece &ais_piece);
  void LoadFormationRandomly();
  void ReplaceSomePiecesRandomly();
};

#endif  // STRATEGO_AI_H_