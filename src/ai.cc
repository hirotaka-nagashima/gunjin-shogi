//-----------------------------------------------------------------------------
// Copyright (c) 2016 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------
// Information on this class is described in "ai.h".
//-----------------------------------------------------------------------------

#include "ai.h"
#include <algorithm>
#include "point.h"
#include "board.h"

const int Ai::kMaxTimesSwapPiecesRandomly = 2;
const char *Ai::kFormationFileUrl = "resources/formations.txt";

void Ai::ReplacePieces() {
  LoadFormationRandomly();
  ReplaceSomePiecesRandomly();
}

Move Ai::MovePiece() {
  if (board()->prev_move_is_initialized()) {
    Board::Piece attacked_piece = board()->prev_dest_piece();
    SupposeOpponentsFormation(attacked_piece);
  }

  // Determine a source.
  int best_evaluation_value = INT_MIN;
  Move move, best_move;
  for (move.src.y = 0; move.src.y < Board::kHeight; ++move.src.y) {
    for (move.src.x = 0; move.src.x < Board::kWidth; ++move.src.x) {
      Board::Piece current_src = board()->board(move.src);
      // If a current piece is unavailable as source.
      if (current_src.characters_id != id() ||
          board()->CountNumPlaceableSquares(move.src) <= 0) {
        continue;
      }

      // Determine a destination.
      for (move.dest.y = 0; move.dest.y < Board::kHeight; ++move.dest.y) {
        for (move.dest.x = 0; move.dest.x < Board::kWidth; ++move.dest.x) {
          // If a current piece is unavailable as dest.
          if (!board()->IsMoveValid(move))
            continue;

          board()->SupposeBattle(id(), move);
          int evaluation_value = EvaluateBoard();
          board()->Undo();

          // Update the best move.
          if (best_evaluation_value < evaluation_value) {
            best_move = move;
            best_evaluation_value = evaluation_value;
          }
        }
      }
    }
  }

  // Determine a move.
  board()->Battle(best_move);
  Board::Piece attacking_piece = board()->prev_src_piece();
  SupposeOpponentsFormation(attacking_piece);

  return move;
}

int Ai::EvaluateBoard() const {
  // Calculate offensive power and defensive power.
  int attack[Board::kNumPlayers] = {0};
  int defense[Board::kNumPlayers] = {0};
  for (int i = 0; i < Board::kNumPlayers; ++i) {
    Point p;
    for (p.y = 0; p.y < Board::kHeight; ++p.y) {
      for (p.x = 0; p.x < Board::kWidth; ++p.x) {
        // Get the current piece.
        Board::Piece piece = board()->board(p);
        if (!piece.IsPiece())
          continue;

        // Evaluate the board with distance to each headquarters
        // and its strength.
        Board::Piece::KindPiece strength =
          static_cast<Board::Piece::KindPiece>(
          Board::Piece::kNumKindPieces - ((i == Ai::id()) ? piece.piece :
          (piece.supposition != Board::Piece::kNone) ? piece.supposition :
          Board::Piece::kChusa));
        int distance_to_headquarters = (Board::kHeight + Board::kWidth) -
          board()->MeasureDistanceToHeadquartersOf(i, p);
        if (piece.characters_id == i)
          defense[i] += distance_to_headquarters * strength;
        else
          attack[1 - i] += distance_to_headquarters * strength;
      }
    }
  }

  int score = board()->CountNumPieces(id()) -
    board()->CountNumPieces(opponents_id());
  int evaluation_value_ai = attack[id()] + defense[id()];
  int evaluation_value_opponent =
    attack[opponents_id()] + defense[opponents_id()];
  int evaluation_value = evaluation_value_ai - evaluation_value_opponent +
    score * 10;

  return evaluation_value;
}

void Ai::SupposeOpponentsFormation(const Board::Piece &ais_piece) {
  // If the moved opponent's piece lost.
  Move prev_move = board()->prev_move();
  Board::Piece current_piece = board()->board(prev_move.dest);
  bool ai_won = (current_piece.characters_id == id() ||
                 current_piece.piece == Board::Piece::kNone);
  if (ai_won)
    return;

  // Suppose the opponent's piece.
  Point difference = prev_move.dest.Subtract(prev_move.src);
  if (board()->prev_src_piece().characters_id == 0)
    difference = difference.Inverse();
  Board::Piece::KindPiece supposition = Board::Piece::kNone;
  if (board()->IsPieceHittingObstacle(prev_move)) {
    supposition = Board::Piece::kPlane;
  } else if (2 <= abs(difference.x)) {
    supposition = Board::Piece::kEngineer;
  } else if (2 <= difference.y) {
    supposition = Board::Piece::kPlane;
  } else if (difference.y <= -2) {
    supposition = Board::Piece::kCavaly;
    if (ais_piece.piece != Board::Piece::kNone) {
      for (int i = Board::Piece::kNumKindPieces - 3; 0 <= i; --i) {
        bool piece_can_go_ahead_2 = (i == Board::Piece::kPlane ||
                                     i == Board::Piece::kTank ||
                                     i == Board::Piece::kEngineer ||
                                     i == Board::Piece::kCavaly);
        bool ai_lose = (!board()->kBattleTable[ais_piece.piece][i]);
        if (piece_can_go_ahead_2 && ai_lose) {
          supposition = static_cast<Board::Piece::KindPiece>(i);
          break;
        }
      }
    }
  } else if (ais_piece.piece != Board::Piece::kNone) {
    for (int i = Board::Piece::kNumKindPieces - 3; 0 <= i; --i) {
      bool ai_lose = (!board()->kBattleTable[ais_piece.piece][i]);
      if (ai_lose) {
        supposition = static_cast<Board::Piece::KindPiece>(i);
        break;
      }
    }
  }

  // Set supposition into the piece.
  current_piece.supposition =
      std::min(current_piece.supposition, supposition);
  board()->set_prev_dest(current_piece);
  board()->set_board(current_piece, prev_move.dest);
}

void Ai::LoadFormationRandomly() {
  FILE *file;
  errno_t error = fopen_s(&file, kFormationFileUrl, "r");
  if (error != 0) {
    fprintf(stderr, "ERROR: %s is not existed.\n", kFormationFileUrl);
    exit(-1);
  }

  // Choose a formation randomly.
  int num_formations;
  fscanf_s(file, "%d", &num_formations);
  int formation_id = rand() % num_formations;

  // Skip untill the choosen formation.
  int size_board = Board::kWidth * Board::kHeight / 2;
  for (int i = 0, dummy; i < size_board * formation_id; ++i)
    fscanf_s(file, "%d", &dummy);

  // Load the formation.
  for (int y = 0; y < Board::kHeight / 2; ++y) {
    for (int x = 0; x < Board::kWidth; ++x) {
      Board::Piece piece;
      piece.characters_id = id();
      piece.supposition = Board::Piece::kNone;

      // Read a piece.
      int temp_kind_piece;
      fscanf_s(file, "%d", &temp_kind_piece);
      piece.piece = static_cast<Board::Piece::KindPiece>(temp_kind_piece);
      if (!piece.IsPiece()) {
        fprintf_s(stderr, "ERROR: The formation file is unavailable.\n");
        exit(-1);
      }

      // Place a piece.
      Point dest = {(id() == 0) ? y : Board::kHeight - 1 - y,
        (id() == 0) ? x : Board::kWidth - 1 - x};
      board()->set_board(piece, dest);
    }
  }

  fclose(file);
}

void Ai::ReplaceSomePiecesRandomly() {
  if (kMaxTimesSwapPiecesRandomly <= 0)
    return;

  // Determine the number of times to swap pieces.
  int num_times = rand() % kMaxTimesSwapPiecesRandomly;

  // Swap pieces.
  for (int i = 0; i < num_times; ++i) {
    Move move;
    board()->DeterminePointRandomly(id(), &move.src);
    board()->DeterminePointRandomly(id(), &move.dest);
    board()->Swap(move);
  }

  // If the board is invalid.
  std::vector<Point> error;
  while (!board()->IsValid(id(), &error)) {
    for (int i = 0; i < static_cast<int>(error.size()); ++i) {
      Move move;
      move.src = error[i];
      board()->DeterminePointRandomly(id(), &move.dest);
      board()->Swap(move);
    }
  }
}