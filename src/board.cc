//-----------------------------------------------------------------------------
// Copyright (c) 2016 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------
// Information on this class is described in "board.h".
//-----------------------------------------------------------------------------

#include "board.h"
#include <cmath>
#include <cassert>

const Point Board::kEntrances[kNumEntrances] = {
    {3, 1}, {3, 4}, {4, 1}, {4, 4}};
const Point Board::kHeadquarters[kNumPlayers][2] = {
    {{0, kWidth / 2 - 1}, {0, kWidth / 2}},
    {{kHeight - 1, kWidth / 2 - 1}, {kHeight - 1, kWidth / 2}}};
const int Board::kNumEachPiece[Piece::kNumKindPieces] = {
    1, 1, 1, 2, 2, 1, 1, 1, 2, 2, 2, 2, 1, 1, 2, 1};
const Board::BattleResult Board::kBattleTable
    [Piece::kNumKindPieces - 1][Piece::kNumKindPieces - 1] = {
    {kD, kW, kW, kW, kW, kW, kW, kW, kW, kW, kW, kW, kW, kL, kL},
    {kL, kD, kW, kW, kW, kW, kW, kW, kW, kW, kW, kW, kW, kW, kL},
    {kL, kL, kD, kW, kW, kW, kW, kW, kW, kW, kW, kW, kW, kW, kL},
    {kL, kL, kL, kD, kW, kW, kW, kW, kW, kW, kW, kW, kW, kW, kW},
    {kL, kL, kL, kL, kD, kW, kW, kW, kW, kW, kW, kW, kW, kW, kL},
    {kL, kL, kL, kL, kL, kD, kW, kW, kW, kW, kW, kW, kW, kW, kL},
    {kL, kL, kL, kL, kL, kL, kD, kW, kW, kW, kW, kW, kW, kW, kL},
    {kL, kL, kL, kL, kL, kL, kL, kD, kW, kW, kW, kW, kW, kW, kL},
    {kL, kL, kL, kL, kL, kL, kL, kL, kD, kW, kW, kW, kW, kW, kL},
    {kL, kL, kL, kL, kL, kL, kL, kL, kL, kD, kW, kW, kW, kW, kL},
    {kL, kL, kL, kL, kL, kL, kL, kL, kL, kL, kD, kW, kW, kW, kL},
    {kL, kL, kL, kL, kL, kL, kL, kL, kL, kL, kL, kD, kL, kW, kW},
    {kL, kL, kL, kL, kL, kL, kL, kL, kL, kL, kL, kW, kD, kW, kL},
    {kW, kL, kL, kL, kL, kL, kL, kL, kL, kL, kL, kL, kL, kD, kL},
    {kW, kW, kW, kL, kW, kW, kW, kW, kW, kW, kW, kL, kW, kW, kD}};

void Board::Initialize() {
  // Sequence pieces.
  int count_each_piece[Piece::kNumKindPieces];  // For mapping.
  for (int y = 0; y < kHeight; ++y) {
    // Initialize map.
    if (y % (kHeight / 2) == 0)
      memset(count_each_piece, 0, sizeof(count_each_piece));

    for (int x = 0; x < kWidth; ++x) {
      // Create a piece randomly.
      Piece piece;
      piece.characters_id = y / (kHeight / 2);
      piece.supposition = Piece::kNone;
      
      // Set a kind of the piece.
      bool y_is_in_range = (y == 0 || y == kHeight - 1);
      bool x_is_in_range = (x == kWidth / 2);
      bool is_dummy_headquarters = (y_is_in_range && x_is_in_range);
      if (is_dummy_headquarters) {
        piece.piece = Piece::kDummyHeadquarters;
      } else {
        // Create a random kind of piece.
        do {
          piece.piece = static_cast<Piece::KindPiece>(
              rand() % Piece::kNumKindPieces);
        } while (kNumEachPiece[piece.piece] <=
                 count_each_piece[piece.piece]);
        ++count_each_piece[piece.piece];
      }

      // Place the piece.
      board_[y][x] = piece;
    }
  }

  // If the board is invalid.
  for (int id = 0; id < kNumPlayers; ++id) {
    std::vector<Point> error;
    while (!IsValid(id, &error)) {
      for (int i = 0; i < static_cast<int>(error.size()); ++i) {
        Move move;
        move.src = error[i];
        DeterminePointRandomly(id, &move.dest);
        Swap(move);
      }
    }
  }
}

void Board::Battle(const Move &move) {
  const Piece kSrcPiece = board(move.src);
  const Piece kDestPiece = board(move.dest);

  // Log.
  add_log(move);

  // Check which piece is stronger.
  BattleResult result = kW;
  switch (kDestPiece.piece) {
    // If the strong of the piece is depend on one at the back of.
  case Piece::kFlag: {
    // Check a piece at the back of the flag.
    bool back_flag_is_existed = false;
    Piece back_flag;
    if (kDestPiece.characters_id == 0 && 0 < move.dest.y) {
      back_flag = board({move.dest.y - 1, move.dest.x});
      back_flag_is_existed = true;
    } else if (kDestPiece.characters_id == 1 && move.dest.y < kHeight - 1) {
      back_flag = board({move.dest.y + 1, move.dest.x});
      back_flag_is_existed = true;
    }

    // Check the result of the battle.
    if (back_flag_is_existed && back_flag.IsPiece() &&
        back_flag.characters_id == kDestPiece.characters_id) {
      result = kBattleTable[kSrcPiece.piece][back_flag.piece];
      if (back_flag.piece == Piece::kMine)
        Delete(move.dest);
    }
    break;
  }
  case Piece::kNone: {
    result = kW;
    break;
  }
  default: {
    result = kBattleTable[kSrcPiece.piece][kDestPiece.piece];
    break;
  }
  }

  // Delete pieces.
  Delete(move.src);
  switch (result) {
  case kL: if (kDestPiece.piece == Piece::kMine) Delete(move.dest); break;
  case kW: set_board(kSrcPiece, move.dest); break;
  case kD: Delete(move.dest); break;
  default: assert(true);
  }
}

bool Board::IsValid(int characters_id, std::vector<Point> *error) const {
  bool is_available = true;

  // Check whether both mines and a flag aren't placed at entrances.
  int first_entrance = (characters_id == 0) ? 0 : kNumEntrances / 2;
  int last_entrance = (characters_id == 0) ? kNumEntrances / 2 : kNumEntrances;
  for (int i = first_entrance; i < last_entrance; ++i) {
    Piece piece = board(kEntrances[i]);
    if (piece.IsNotMovable()) {
      error->push_back(kEntrances[i]);
      is_available = false;
    }
  }

  // NOTE: A rule is unclearly about one below.
  // Check whether a mine is placed at headquarters.
  //Point headquarters;
  //headquarters.y = (characters_id == 0) ? 0 : kHeight - 1;
  //headquarters.x = kWidth / 2;
  //if (Piece::kMine == board(headquarters).piece) {
  //  error->push_back(headquarters);
  //  is_available = false;
  //}

  return is_available;
}

bool Board::IsMoveValid(const Move &move) const {
  const Piece kSrcPiece = board(move.src);
  const Piece kDestPiece = board(move.dest);

  // Check whether the piece at source is movable.
  if (!kSrcPiece.IsMovable())
    return false;

  // Check whether pieces at destination and source
  // belong to same characters.
  if (kSrcPiece.characters_id == kDestPiece.characters_id &&
      kDestPiece.IsPiece()) {
    return false;
  }

  // Calculate differencial vector.
  Point difference = move.dest.Subtract(move.src);
  // If a board was rotated 180 degrees.
  if (kSrcPiece.characters_id == 0)
    difference = difference.Inverse();

  // If the piece moves diagonally or doesn't move.
  if ((difference.y != 0 && difference.x != 0) ||
      (difference.y == 0 && difference.x == 0)) {
    return false;
  }

  bool piece_hits_obstacle = IsPieceHittingObstacle(move);
  bool y_is_in_range = (abs(difference.y) == 1);
  bool x_is_in_range = (abs(difference.x) == 1);
  switch (kSrcPiece.piece) {
  case Piece::kTank:  // front:2, others:1
  case Piece::kCavaly:
    y_is_in_range |= (difference.y == -2);
    return ((y_is_in_range && !piece_hits_obstacle) || x_is_in_range);
  case Piece::kEngineer:  // all:*
    return (!piece_hits_obstacle);
    break;
  case Piece::kPlane:  // front&back:*, others:1
    y_is_in_range = (difference.y != 0);
    return (y_is_in_range || x_is_in_range);
  default:  // all:1
    return ((y_is_in_range && !piece_hits_obstacle) || x_is_in_range);
  }
}

bool Board::IsEnd(int *winners_id, bool *game_was_drawn) const {
  // Check which character win.
  *game_was_drawn = false;
  bool winners_id_is_initialized = true;
  for (int id = 0; id < kNumPlayers; ++id) {
    int opponents_id = 1 - id;

    // If opponent's piece is ranked between shosa ~ taisho, end the game.
    Piece headquarters = board(kHeadquarters[id][0]);
    if (headquarters.characters_id == opponents_id &&
        (headquarters.IsShokan() || headquarters.IsSakan())) {
      *winners_id = opponents_id;
      return true;
    }

    // Check whether there is no piece which can be moved.
    bool has_no_movable_piece = true;
    Point p;
    for (p.y = 0; p.y < kHeight && has_no_movable_piece; ++p.y) {
      for (p.x = 0; p.x < kWidth && has_no_movable_piece; ++p.x) {
        Piece piece = board(p);
        if (piece.characters_id == id && piece.IsMovable())
          has_no_movable_piece = false;
      }
    }
    if (has_no_movable_piece) {
      if (winners_id_is_initialized) {
        *winners_id = opponents_id;
        winners_id_is_initialized = false;
      } else {
        *game_was_drawn = true;
        return true;
      }
    }
  }
  if (!winners_id_is_initialized)
    return true;

  return false;
}

bool Board::IsPieceHittingObstacle(const Move &move) const {
  const Point kDifference = move.dest.Subtract(move.src);

  Point current = move.src;
  int prev_y = move.src.y;
  do {
    // Move a piece.
    if (kDifference.y != 0)
      current.y += abs(kDifference.y) / kDifference.y;
    else
      current.x += abs(kDifference.x) / kDifference.x;

    // If there is a piece.
    if (board(current).piece != Piece::kNone && !current.Equals(move.dest))
      return true;

    // If there is wall.
    double average_y = 0.5 * (current.y + prev_y);
    if (average_y == 0.5 * (kHeight - 1) &&
        current.x != 1 && current.x != kWidth - 1 - 1) {
      return true;
    }

    prev_y = current.y;
  } while (!current.Equals(move.dest));

  return false;
}

int Board::CountNumPieces(int characters_id) const {
  // Count the number of player's pieces.
  int num_pieces = 0;
  Point p;
  for (p.y = 0; p.y < kHeight; ++p.y) {
    for (p.x = 0; p.x < kWidth; ++p.x) {
      Piece piece = board(p);
      if (piece.characters_id == characters_id && piece.IsPiece())
        ++num_pieces;
    }
  }

  return num_pieces;
}

int Board::CountNumPlaceableSquares(const Point &src) const {
  Move move;
  move.src = src;

  // Count the number of placeable squares.
  int num_placeable_squares = 0;
  for (move.dest.y = 0; move.dest.y < kHeight; ++move.dest.y) {
    for (move.dest.x = 0; move.dest.x < kWidth; ++move.dest.x) {
      bool is_current_piece_not_dummy_headquarters =
          (board(move.dest).piece != Piece::kDummyHeadquarters);
      if (is_current_piece_not_dummy_headquarters && IsMoveValid(move))
        ++num_placeable_squares;
    }
  }

  return num_placeable_squares;
}

void Board::DeterminePointRandomly(int id, Point *point) const {
  point->y = rand() % (Board::kHeight / 2);
  point->y += (id == 1) ? Board::kHeight / 2 : 0;
  point->x = rand() % Board::kWidth;
}

void Board::SupposeBattle(int supposer_id, const Move &move) {
  const Piece kSrcPiece = board(move.src);
  const Piece kDestPiece = board(move.dest);
  const Piece::KindPiece kSrcKindPiece =
    (kSrcPiece.characters_id == supposer_id) ?
    kSrcPiece.piece : kSrcPiece.supposition;
  const Piece::KindPiece kDestKindPiece =
    (kDestPiece.characters_id == supposer_id) ?
    kDestPiece.piece : kDestPiece.supposition;

  // Log.
  add_log(move);

  // Check which piece is stronger.
  BattleResult result = kW;
  switch (kDestKindPiece) {
  case Piece::kNone:
    result = kW;
    break;
  default:
    result = kBattleTable[kSrcKindPiece][kDestKindPiece];
    break;
  }

  // Delete pieces.
  Delete(move.src);
  switch (result) {
  case kL: break;
  case kW: set_board(kSrcPiece, move.dest); break;
  case kD: Delete(move.dest); break;
  default: assert(true);
  }
}

int Board::MeasureDistanceToHeadquartersOf(int id, const Point &p) {
  // Measure distance to headquarters of id.
  // Determine the shortest distance as.
  int shorter_distance_to_headquarters = INT_MAX;
  for (int i = 0; i < 2; ++i) {
    Point difference = kHeadquarters[id][i].Subtract(p);
    int distance_to_headquarters = abs(difference.y) + abs(difference.x);
    if (distance_to_headquarters < shorter_distance_to_headquarters)
      shorter_distance_to_headquarters = distance_to_headquarters;
  }

  // Add distance to through an entrance.
  bool must_add_distance_y = (id != p.y / (kHeight / 2));
  bool must_add_distance_x = (p.x == kWidth / 2 - 1 || p.x == kWidth / 2);
  if (must_add_distance_y && must_add_distance_x)
    shorter_distance_to_headquarters += 2;

  return shorter_distance_to_headquarters;
}