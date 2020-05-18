//-----------------------------------------------------------------------------
// Copyright (c) 2016 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef STRATEGO_BOARD_H_
#define STRATEGO_BOARD_H_

#include <vector>
#include "point.h"

class Board {
public:
  struct Piece {
    // In strong order.
    enum KindPiece {
      kDummyHeadquarters = -1,
      kTaisho, kChujo, kShosho,  // shokan
      kPlane, kTank,  // plane, tank
      kTaisa, kChusa, kShosa,  // sakan
      kTaii, kChui, kShoi,  // ikan
      kEngineer, kCavaly, kSpy, kMine, kFlag,  // others
      kNumKindPieces,
      kNone = 99,
    };

    bool IsPiece() const { return (kTaisho <= piece && piece <= kFlag); }
    bool IsShokan() const { return (kTaisho <= piece && piece <= kShosho); }
    bool IsSakan() const { return (kTaisa <= piece && piece <= kShosa); }
    bool IsNotMovable() const { return (piece == kFlag || piece == kMine); }
    bool IsMovable() const {
      return (IsPiece() && piece != kFlag && piece != kMine);
    }

    KindPiece piece;
    KindPiece supposition;  // For the ai.
    int characters_id;
  };
  enum BattleResult {
    kL,  // Lose.
    kW,  // Win.
    kD,  // Draw.
  };

  static const int kNumPieces = 23;
  static const int kWidth = 6;
  static const int kHeight = 8;
  static const int kNumEntrances = 4;
  static const int kNumPlayers = 2;
  static const Point kEntrances[kNumEntrances];
  static const Point kHeadquarters[kNumPlayers][2];
  static const int kNumEachPiece[Piece::kNumKindPieces];
  static const BattleResult
      kBattleTable[Piece::kNumKindPieces - 1][Piece::kNumKindPieces - 1];

  void Initialize();
  void Battle(const Move &move);
  bool IsValid(int characters_id, std::vector<Point> *error) const;
  bool IsMoveValid(const Move &move) const;
  bool IsEnd(int *winners_id, bool *game_was_drawn) const;
  bool IsPieceHittingObstacle(const Move &move) const;
  int CountNumPieces(int characters_id) const;
  int CountNumPlaceableSquares(const Point &src) const;
  void DeterminePointRandomly(int id, Point *point) const;
  // For the ai. ->
  void SupposeBattle(int supposer_id, const Move &move);
  int MeasureDistanceToHeadquartersOf(int id, const Point &p);
  // <- For the ai.

  void Swap(const Move &move) {
    Piece src_piece = board(move.src);
    Piece dest_piece = board(move.dest);
    set_board(dest_piece, move.src);
    set_board(src_piece, move.dest);
  }
  bool ExistHeadquartersAt(const Point &point) const {
    bool y_is_in_range = (point.y == 0 || point.y == kHeight - 1);
    bool x_is_in_range = (point.x == kWidth / 2 - 1);
    return (y_is_in_range && x_is_in_range);
  }
  void Undo() {
    Log prev = log_.back();
    log_.pop_back();
    set_board(prev.src_piece, prev.move.src);
    set_board(prev.dest_piece, prev.move.dest);
  }
  bool IsDummyHeadquarters(const Point &p) const {
    return (board_[p.y][p.x].piece == Piece::kDummyHeadquarters);
  }

  void set_prev_dest(const Piece &piece) { log_.back().dest_piece = piece; }
  void set_board(const Piece &piece, const Point &dest) {
    board_[dest.y][dest.x + (IsDummyHeadquarters(dest) ? -1 : 0)] = piece;
  }
  Piece prev_src_piece() const { return log_.back().src_piece; }
  Piece prev_dest_piece() const { return log_.back().dest_piece; }
  Move prev_move() const { return log_.back().move; }
  bool prev_move_is_initialized() const { return !log_.empty(); }
  Piece board(const Point &p) const {
    return board_[p.y][p.x + (IsDummyHeadquarters(p) ? -1 : 0)];
  }

private:
  struct Log {
    Move move;
    Piece src_piece, dest_piece;
  };

  void Delete(const Point &p) {
    Piece deleted_piece;
    deleted_piece.characters_id = p.y / (kHeight / 2);
    deleted_piece.piece = Piece::kNone;
    deleted_piece.supposition = Piece::kNone;
    set_board(deleted_piece, p);
  }

  void set_prev_move(const Move &prev_move) {
    Log prev;
    prev.move = prev_move;
    log_.push_back(prev);
  }
  void add_log(const Move &prev_move) {
    Log log;
    log.move = prev_move;
    log.src_piece = board(prev_move.src);
    log.dest_piece = board(prev_move.dest);
    log_.push_back(log);
  }

  Piece board_[kHeight][kWidth];
  std::vector<Log> log_;
};

#endif  // STRATEGO_BOARD_H_