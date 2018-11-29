#include "logic.h"
#include "beatmap.h"
#include "maxmod.h"
#include "soundbank.h"
#include "soundbank_bin.h"

void initializeAppState(AppState* appState) {
    // TA-TODO: Initialize everything that's part of this AppState struct here.
    // Suppose the struct contains random values, make sure everything gets
    // the value it should have when the app begins.
    appState->gameOver = 0;
    appState->animationInd = 0;
    appState->gameStarted = 0;
    appState->notesHit = 0;
    appState->score = 0;
    appState->bestCombo = 0;
    appState->vblankwait = 0;
    appState->elapsed = 0;
    appState->hitNotify = 0;
    appState->curHit = 0;
    appState->curReleased = 0;
    appState->map = (OsuMap) {notes, 0, 0, NOTES_LENGTH, 300 * NOTES_LENGTH};
    for (int i = 0; i < NOTES_LENGTH; i++) {
        notes[i].hit = 0;
    }
}

AppState processAppState(AppState *currentAppState, u32 keysPressedBefore, u32 keysPressedNow) {

    AppState nextAppState = *currentAppState;

    if (KEY_JUST_PRESSED(BUTTON_A, keysPressedNow, keysPressedBefore)) {
        nextAppState.curHit |= 8;
        nextAppState.curReleased &= ~8;
    } else if (KEY_JUST_RELEASED(BUTTON_A, keysPressedNow, keysPressedBefore)) {
        nextAppState.curHit &= ~8;
        nextAppState.curReleased |= 8;
    } else if (!KEY_DOWN(BUTTON_A, keysPressedNow)) {
        nextAppState.curHit &= ~8;
        nextAppState.curReleased &= ~8;
    }

    if (KEY_JUST_PRESSED(BUTTON_B, keysPressedNow, keysPressedBefore)) {
        nextAppState.curHit |= 4;
        nextAppState.curReleased &= ~4;
    } else if (KEY_JUST_RELEASED(BUTTON_B, keysPressedNow, keysPressedBefore)) {
        nextAppState.curHit &= ~4;
        nextAppState.curReleased |= 4;
    } else if (!KEY_DOWN(BUTTON_B, keysPressedNow)) {
        nextAppState.curHit &= ~4;
        nextAppState.curReleased &= ~4;
    }

    if (KEY_JUST_PRESSED(BUTTON_L, keysPressedNow, keysPressedBefore)) {
        nextAppState.curHit |= 2;
        nextAppState.curReleased &= ~2;
    } else if (KEY_JUST_RELEASED(BUTTON_L, keysPressedNow, keysPressedBefore)) {
        nextAppState.curHit &= ~2;
        nextAppState.curReleased |= 2;
    } else if (!KEY_DOWN(BUTTON_L, keysPressedNow)) {
        nextAppState.curHit &= ~2;
        nextAppState.curReleased &= ~2;
    }

    if (KEY_JUST_PRESSED(BUTTON_R, keysPressedNow, keysPressedBefore)) {
        nextAppState.curHit |= 1;
        nextAppState.curReleased &= ~1;
    } else if (KEY_JUST_RELEASED(BUTTON_R, keysPressedNow, keysPressedBefore)) {
        nextAppState.curHit &= ~1;
        nextAppState.curReleased |= 1;
    } else if (!KEY_DOWN(BUTTON_R, keysPressedNow)) {
        nextAppState.curHit &= ~1;
        nextAppState.curReleased &= ~1;
    }

    if ((nextAppState.animationInd == 39) && (!nextAppState.gameStarted) &&
        (KEY_JUST_PRESSED(BUTTON_START, keysPressedNow, keysPressedBefore))) {
        nextAppState.gameStarted = 1;
        mmEffect(SFX_IKU);
        mmEffect(SFX_HIME);
    }

    if (nextAppState.animationInd == 33) {
        nextAppState.vblankwait = 150;
        ++nextAppState.animationInd;
    }

    if (nextAppState.animationInd == 35) {
        nextAppState.vblankwait = 150;
        ++nextAppState.animationInd;
    }

    if (nextAppState.animationInd == 37 ) {
        nextAppState.vblankwait = 150;
        ++nextAppState.animationInd;
    }

    if (nextAppState.gameStarted) {
        ++nextAppState.elapsed;
    }
    return nextAppState;
}
