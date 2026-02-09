//-----------------------------------------------------------------------------
// Copyright (c) 2016 Hirotaka Nagashima. All rights reserved.
//-----------------------------------------------------------------------------
// This class is main class of gunjin shogi.
//-----------------------------------------------------------------------------

#ifndef GUNJIN_SHOGI_GAME_H_
#define GUNJIN_SHOGI_GAME_H_

#include "board.h"
#include "player.h"
#include "ai.h"
#include "graphic.h"

class Character;
class Game {
public:
  static const int kNumPlayers = 2;

  Game() : board_(new Board) {
    // Register characters.
    // If you want to play with a human, edit here.
    set_characters(0, new Player(&graphic(), board(), 0, "Player1"));
    //set_characters(1, new Player(&graphic(), board(), 1, "Player2"));
    set_characters(1, new Ai(board(), 1, "Computer"));
    set_play_with_player(Character::kPlayer == characters(0)->type() &&
                         Character::kPlayer == characters(1)->type());
  }
  ~Game() {
    delete board();
    delete characters(0);
    delete characters(1);
  }

  void Initialize();
  void Terminate();
  void Main();

private:
  void DisplayPrevMove(int id);
  void DisplayResult(int winners_id, bool game_was_drawn);

  Graphic &graphic() { return graphic_; }
  Character *characters(int id) const { return characters_[id]; }
  Board *board() const { return board_; }
  bool play_with_player() const { return play_with_player_; }
  void set_play_with_player(bool play_with_player) {
    play_with_player_ = play_with_player;
  }
  void set_characters(int id, Character *character) {
    characters_[id] = character;
  }

  Graphic graphic_;
  Character *characters_[kNumPlayers];
  Board *board_;
  bool play_with_player_;
};

#endif  // GUNJIN_SHOGI_GAME_H_