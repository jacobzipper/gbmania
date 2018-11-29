#include "graphics.h"
#include "gba.h"
#include "logic.h"
#include "tomogamebg.h"
#include "popup.h"
#include "soundbank.h"
#include "soundbank_bin.h"

#include <stdio.h>

static int vBlankToMillis(int vBlank) {
    return 17 * vBlank;
}

static int millisToPos(int millis) {
    return 160 - (millis / 4);
}

void fullDrawAppState(AppState *state) {
    int rectHeight = 130;

    drawFullScreenImageDMA(tomogamebg);
    drawRectDMA(20, 0, 90, rectHeight, BLACK);
    drawRectDMA(20, 130, 90, 30, GRAY);
    drawRectDMA(22, rectHeight, 20, 10, RED);
    drawRectDMA(44, rectHeight, 20, 10, BLUE);
    drawRectDMA(66, rectHeight, 20, 10, RED);
    drawRectDMA(88, rectHeight, 20, 10, BLUE);
    UNUSED(state);
}

static void removePressed(AppState *state) {
    int rectHeight = 130;
    if (state->curReleased & 8) {
        drawRectDMA(22, rectHeight, 20, 10, RED);
    }
    if (state->curReleased & 4) {
        drawRectDMA(44, rectHeight, 20, 10, BLUE);
    }
    if (state->curReleased & 2) {
        drawRectDMA(66, rectHeight, 20, 10, RED);
    }
    if (state->curReleased & 1) {
        drawRectDMA(88, rectHeight, 20, 10, BLUE);
    }
}

static void animationUndraw(AppState *state) {
    int animInd = 100;

    if (state->animationInd < 31) {
        drawBgReplacementDMA(130, 110, 70, 50, tomogamebg);
    }

    if (state->animationInd == 34 && !state->vblankwait) {
        drawBgReplacementDMA(110, animInd, 130, 20, tomogamebg);
    }

    if (state->animationInd == 36 && !state->vblankwait) {
        drawBgReplacementDMA(110, animInd, 130, 20, tomogamebg);
    }

    if (state->animationInd == 38 && !state->vblankwait) {
        drawBgReplacementDMA(110, animInd, 130, 20, tomogamebg);
    }
}

static void undrawScreen(AppState *state) {
    int screenBot = 130;
    if (state->gameStarted && (vBlankCounter % 2 == 0)) {
        drawRectDMA(20, 0, 90, screenBot, BLACK);
    }
    if (state->gameStarted && (vBlankCounter % 30 == 0)) {
        drawRectDMA(20, 130, 90, 30, GRAY);
        drawRectDMA(22, screenBot, 20, 10, RED);
        drawRectDMA(44, screenBot, 20, 10, BLUE);
        drawRectDMA(66, screenBot, 20, 10, RED);
        drawRectDMA(88, screenBot, 20, 10, BLUE);
    }
}

// This function will be used to undraw (i.e. erase) things that might
// move in a frame. E.g. in a Snake game, erase the Snake, the food & the score.
void undrawAppState(AppState *state) {
    // TA-TODO: IMPLEMENT.
    removePressed(state);

    undrawScreen(state);

    animationUndraw(state);
}

static void doAnimationCurrent(AppState *state) {
    int instHeight = 110;
    if (state->animationInd < 32) {
        drawImageDMA(170 - state->animationInd, HEIGHT - state->animationInd, 32, 32, popup);
        ++state->animationInd;
    } else if (state->animationInd == 32) {
        mmEffect(SFX_NANI);
        drawString(110, instHeight, "WELCOME TO OSU!MANIA", MAGENTA);
        ++state->animationInd;
    }

    if (state->animationInd == 34 && state->vblankwait == 0) {
        drawString(110, instHeight, "PLEASE MAP ABLR->DFJK", MAGENTA);
        ++state->animationInd;
    }

    if (state->animationInd == 36 && state->vblankwait == 0) {
        drawString(110, instHeight, "CLICK NOTES AT LINE", MAGENTA);
        ++state->animationInd;
    }

    if (state->animationInd == 38 && state->vblankwait == 0) {
        drawString(110, instHeight, "HAVE FUN!!!", MAGENTA);
        drawString(110, 90, "PRESS START TO PLAY", MAGENTA);
        drawString(110, 70, "PRESS SELECT TO EXIT", MAGENTA);
        ++state->animationInd;
    }

    if (state->vblankwait >= 0) {
        --state->vblankwait;
    }
}

static void doHitsCurrent(AppState *state) {
    int hitSpot = HEIGHT - 29;
    if (state->curHit & 8) {
        drawRectDMA(23, hitSpot, 18, 8, BLUE);
    }
    if (state->curHit & 4) {
        drawRectDMA(45, hitSpot, 18, 8, RED);
    }
    if (state->curHit & 2) {
        drawRectDMA(67, hitSpot, 18, 8, BLUE);
    }
    if (state->curHit & 1) {
        drawRectDMA(89, hitSpot, 18, 8, RED);
    }
}

static void showStats(AppState *state) {
    if (vBlankCounter % 180 == 0 && state->gameStarted) {
        drawBgReplacementDMA(110, 0, 130, 120, tomogamebg);
        int curMx = 0;
        for (int i = 0; i < state->map.len; i++) {
            if (state->map.notes[i].hit) {
                ++curMx;
            } else if (curMx > state->bestCombo) {
                state->bestCombo = curMx;
                curMx = 0;
            }
        }
        if (curMx > state->bestCombo) {
            state->bestCombo = curMx;
        }
        char buf[16];
        sprintf(buf, "BEST:  %d", state->bestCombo);
        drawString(110, 60, buf, MAGENTA);
        sprintf(buf, "HITS:  %d", state->notesHit);
        drawString(110, 70, buf, MAGENTA);
        sprintf(buf, "SCORE: %d", state->score);
        drawString(110, 80, buf, MAGENTA);
        if (state->map.pointer >= state->map.len) {
            state->gameOver = 1;
            state->gameStarted = 0;
        }
    }
}

static void drawCurrentBars(AppState *state) {
    for (int i = state->map.pointer; i < state->map.top; i++) {
        Note note = state->map.notes[i];
        if (note.high < state->elapsed) {
            ++state->map.pointer;
        } else {
            int pos = millisToPos(vBlankToMillis(note.high - state->elapsed));
            int curNote = 1 << (3 - note.note);
            if (!note.hit && (curNote & state->curHit)) {
                if (pos > 120 && pos < 150) {
                    state->map.notes[i].hit = 1;
                    state->score += 300;
                    ++state->notesHit;
                    mmEffectEx(&((mm_sound_effect){
                        SFX_SUGOI,
                        (int)(1.0f * (1<<10)),
                        0, 64, 0}));
                } else if (pos > 110 && pos < HEIGHT) {
                    state->map.notes[i].hit = 1;
                    state->score += 200;
                    ++state->notesHit;
                    mmEffect(SFX_YAY);
                } else if (pos > 100 && pos < 170) {
                    state->map.notes[i].hit = 1;
                    state->score += 100;
                    ++state->notesHit;
                    mmEffect(SFX_BAKA);
                }
            } else if (!note.hit) {
                drawRectDMA((22 + 22 * note.note), pos, 20, 5, MAGENTA);
            }
        }
    }
    for (int i = state->map.top; i < state->map.len; i++) {
        Note note = state->map.notes[i];
        if (state->elapsed >= note.low) {
            ++state->map.top;
            int pos = millisToPos(vBlankToMillis(note.high - state->elapsed));
            drawRectDMA((22 + 22 * note.note), pos, 20, 5, MAGENTA);
        } else {
            break;
        }
    }
}

// This function will be used to draw things that might have moved in a frame.
// For example, in a Snake game, draw the snake, the food, the score.
void drawAppState(AppState *state) {
    // TA-TODO: IMPLEMENT.
    doHitsCurrent(state);


    doAnimationCurrent(state);

    showStats(state);

    if (state->gameStarted && (vBlankCounter % 2 == 0)) {
        drawCurrentBars(state);
    }
}
