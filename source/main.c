#include "splash.h"
#include "endgame.h"

#include "gba.h"
#include "gba_interrupt.h"
#include "logic.h"
#include "graphics.h"

#include "maxmod.h"
#include "soundbank.h"
#include "soundbank_bin.h"

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    START,
    START_NODRAW,
    APP_INIT,
    APP,
    APP_EXIT,
    APP_EXIT_NODRAW,
} GBAState;

int main(void) {

    REG_BASE = MODE3 | BG2_ENABLE;

    irqInit();
    irqSet( IRQ_VBLANK, mmVBlank );
    irqEnable( IRQ_VBLANK );

    GBAState state = START;

    // We store the "previous" and "current" states.
    AppState currentAppState, nextAppState;

    // We store the current and previous values of the button input.
    u32 previousButtons = BUTTONS;
    u32 currentButtons = BUTTONS;

    mmInitDefault( (mm_addr)soundbank_bin, 8 );

    while(1) {
        // Load the current state of the buttons
        currentButtons = BUTTONS;

        if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
            state = START;
            initializeAppState(&currentAppState);
            mmEffectCancelAll();
            mmEffect(SFX_JANE);
        }

        // TA-TODO: Manipulate the state machine below as needed.
        switch(state) {
        case START:

            // TA-TODO: Draw the start state here.
            drawFullScreenImageDMA(splash);
            mmEffect(SFX_CIRCLES);

            drawCenteredString(WIDTH / 2, HEIGHT / 2, 0, 0, "OSU!MANIA", MAGENTA);
            drawCenteredString(WIDTH / 2, (HEIGHT / 2) + 10, 0, 0, "PRESS START TO PLAY", MAGENTA);
            drawCenteredString(WIDTH / 2, (HEIGHT / 2) + 20, 0, 0, "MAP ABLR TO DFJK PLEASE", MAGENTA);
            drawCenteredString(WIDTH / 2, (HEIGHT / 2) + 30, 0, 0, "SELECT TO RETURN HERE", MAGENTA);
            drawCenteredString(WIDTH / 2, (HEIGHT / 2) + 60, 0, 0, "JACOB ZIPPER :^)", MAGENTA);

            // Wait for VBlank
            waitForVBlank();
            mmFrame();
            state = START_NODRAW;
            break;
        case START_NODRAW:
            // TA-TODO: Check for a button press here to start the app.
            // Start the app by switching the state to APP_INIT.
            waitForVBlank();
            mmFrame();
            if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
                mmEffectCancelAll();
                // mmEffectRelease(curSound);
                state = APP_INIT;
            }
            break;
        case APP_INIT:
            mmFrame();
            // Initialize the app. Switch to the APP state.
            initializeAppState(&currentAppState);

            // Draw the initial state of the app
            fullDrawAppState(&currentAppState);

            state = APP;
            break;
        case APP:
            // Process the app for one frame, store the next state
            nextAppState = processAppState(&currentAppState, previousButtons, currentButtons);

            // Wait for VBlank before we do any drawing.
            waitForVBlank();
            mmFrame();

            // Undraw the previous state
            undrawAppState(&currentAppState);

            // Draw the current state
            drawAppState(&nextAppState);

            // Now set the current state as the next state for the next iter.
            currentAppState = nextAppState;

            // Check if the app is exiting. If it is, then go to the exit state.
            if (nextAppState.gameOver) state = APP_EXIT;

            break;
        case APP_EXIT:
            // Wait for VBlank
            waitForVBlank();
            mmFrame();

            // TA-TODO: Draw the exit / gameover screen
            drawFullScreenImageDMA(endgame);
            char buf[16];
            sprintf(buf, "BEST:  %d", nextAppState.bestCombo);
            drawString(110, 60, buf, MAGENTA);
            sprintf(buf, "HITS:  %d", nextAppState.notesHit);
            drawString(110, 70, buf, MAGENTA);
            sprintf(buf, "SCORE: %d", nextAppState.score);
            drawString(110, 80, buf, MAGENTA);
            sprintf(buf, "PCNT:  %.2f%%", (100.0f * nextAppState.score) / nextAppState.map.topScore);
            drawString(110, 90, buf, MAGENTA);
            state = APP_EXIT_NODRAW;
            break;
        case APP_EXIT_NODRAW:
            // TA-TODO: Check for a button press here to go back to the start screen
            waitForVBlank();
            mmFrame();
            break;
        }

        // Store the current state of the buttons
        previousButtons = currentButtons;
    }

    return 0;
}


