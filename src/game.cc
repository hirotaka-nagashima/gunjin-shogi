//-----------------------------------------------------------------------------
// Copyright (c) 2016 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------
// Information on this class is described in "game.h".
//-----------------------------------------------------------------------------

#include "game.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include "character.h"

void Game::Initialize() {
  // Reset random seed randomly.
  srand(static_cast<unsigned int>(time(NULL)));

  // Initialize a window.
  graphic().Initialize();
  
  // Initialize a board randomly.
  board()->Initialize();

  // Replace pieces.
  for (int player = 0; player < kNumPlayers; ++player) {
    Character *current_player = characters(player);
    if (play_with_player())
      graphic().WaitNextPlayer(current_player->name());
    current_player->ReplacePieces();
  }
}

void Game::Terminate() {
  // Terminate the process.
  graphic().Terminate();
}

void Game::Main() {
  bool game_was_drawn;
  int winners_id;

  // Move pieces alternately.
  bool is_end = false;
  bool is_first = true;
  for (int id = 0; !is_end; id = 1 - id) {
    Character *character = characters(id);
    Character *opponent = characters(1 - id);
    bool is_players_turn = (Character::kPlayer == character->type());

    // Check whether the game ends.
    is_end = board()->IsEnd(&winners_id, &game_was_drawn);
    if (is_end && play_with_player())
      DisplayResult(winners_id, game_was_drawn);

    // Confirm the player is right one to protect board information
    // if he plays with another player.
    if (play_with_player())
      graphic().WaitNextPlayer(character->name());

    // Display previous move.
    if (is_players_turn && !is_first)
      DisplayPrevMove(id);
    is_first = false;

    // Move a piece and battle.
    if (!is_end) {
      Move move = character->MovePiece();
      for (int i = 0; i < kNumPlayers; ++i)
        characters(i)->UpdateScore();
    }

    // Display the result of the battle.
    if (is_players_turn) {
      graphic().DisplayBoard(*board(), *character);
      graphic().window()->Sleep(play_with_player() ? 1000 : 0);
    }
  }

  DisplayResult(winners_id, game_was_drawn);
}

void Game::DisplayPrevMove(int id) {
  Character * const character = characters(id);
  Move prev_move = board()->prev_move();
  board()->Undo();
  graphic().DisplayBoard(*board(), *character);
  graphic().window()->Sleep(100);
  graphic().HilightSquare(prev_move.src, id);
  graphic().window()->Sleep(100);
  graphic().HilightSquare(prev_move.dest, id);
  graphic().window()->Sleep(550);
  board()->Battle(prev_move);
  graphic().UnhilightSquares(*board(), id);
}

void Game::DisplayResult(int winners_id, bool game_was_drawn) {
  // Display a winner.
  std::string winners_name = "";
  if (!game_was_drawn) {
    Character *winner = characters(winners_id);
    winners_name = winner->name();
  }
  graphic().DisplayResult(*board(), winners_name, game_was_drawn);
}