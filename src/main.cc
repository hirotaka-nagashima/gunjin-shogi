//-----------------------------------------------------------------------------
// Copyright (c) 2016 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------
// 2017/01/13: Updated "Point."
// 2016/11/06: Reconstructed.
// 2016/02/25: Completed.
// 2016/02/14: Project was created.
//-----------------------------------------------------------------------------

#include "game.h"

// NOTE: Comment out below to use SDL in VS2015 or later versions.
// https://stackoverflow.com/questions/30412951/
FILE _iob[] = {*stdin, *stdout, *stderr};
extern "C" FILE * __cdecl __iob_func() {
  return _iob;
}

// NOTE: Format of the main function must be shown below for SDL.
// int main(int argc, char *argv[]) { ~ return 0; }
int main(int argc, char *argv[]) {
  Game game;
  game.Initialize();
  game.Main();
  game.Terminate();
  return 0;
}