#ifndef LOGIC_SEEN
#define LOGIC_SEEN

#include "gba.h"
#include "maxmod.h"

typedef struct { // The current note
    int low;
    int high;
    int note;
    int hit;
} Note;

typedef struct { // Full map to pull from
    Note *notes;
    int pointer;
    int top;
    int len;
    int topScore;
} OsuMap;

typedef struct {
    // Store whether or not the game is over in this member:
    int gameOver;
    int animationInd;
    int gameStarted;
    int notesHit;
    int score;
    int bestCombo;
    int vblankwait;
    int hitNotify;
    int elapsed;
    int curHit;
    int curReleased;
    OsuMap map;

} AppState;

/*
* TA-TODO: Add any additional structs that you need for your app.
*
* For example, for a Snake game, one could be:
*
* typedef struct {
*   int heading;
*   int length;
*   int x;
*   int y;
* } Snake;
*
*/

// This function can initialize an unused AppState struct.
void initializeAppState(AppState *appState);

// This function will be used to process app frames.
AppState processAppState(AppState *currentAppState, u32 keysPressedBefore, u32 keysPressedNow);

// If you have anything else you need accessible from outside the logic.c
// file, you can add them here. You likely won't.

#endif
