// ti_platform.h
//
// Platform-glue hooks for the TI Extended BASIC interpreter.
//
// This header declares every function that bridges the language layer
// (which lives in this library) to a project's hardware: character /
// screen rendering, sprite draw, sound, and joystick input.
//
// Each hook has a weak no-op default in ti_platform.cpp. To override
// from a project, define a strong (non-weak) function with the same
// signature in your .ino — the linker prefers strong over weak so your
// version wins.
//
//   void tiPrintChar(char c) { Serial.write(c); ... }   // strong override
//
// Hooks deliberately NOT in here (and that DO still use explicit
// setCallbacks() on TokenParser): file I/O (FLASH/SD/DSK<n>), shell
// commands (NEW/RUN/SAVE/OLD/etc.), throttle, IMAGE lookup, DATA/READ.
// Those are language-layer concerns, not hardware.

#ifndef TI_PLATFORM_H
#define TI_PLATFORM_H

#include <stdint.h>

// ---------------------------------------------------------------------------
// Console output
// ---------------------------------------------------------------------------
void tiPrintChar(char c);
void tiPrintString(const char* s);
void tiClearScreen();

// ---------------------------------------------------------------------------
// Char grid manipulation (CALL HCHAR / VCHAR / GCHAR / CHAR / COLOR / SCREEN)
// ---------------------------------------------------------------------------
void tiSetChar(int row, int col, char ch);
char tiGetChar(int row, int col);
void tiSetScreenColor(int idx);                       // CALL SCREEN(idx)
void tiSetCharColor(int set, int fg, int bg);         // CALL COLOR
void tiSetCharPattern(int charCode, const uint8_t* bytes);   // CALL CHAR
void tiGetCharPattern(int charCode, uint8_t* bytes);
void tiResetCharset();
// CALL CHARSET("PC" | "TI") — switch the default font ROM the simulator
// uses for chars 32-127. Host should persist the selection (e.g. NVS) and
// refresh `charPatterns[]` before the next CALL CHARSET (which actually
// reloads 32-127). Mode values match the TiFontMode enum in ti_font.h.
void tiSetCharsetMode(int mode);
void tiMoveCursor(int row, int col);                  // DISPLAY AT

// ---------------------------------------------------------------------------
// Sprites (CALL SPRITE / PATTERN / LOCATE / DELSPRITE / MAGNIFY)
// ---------------------------------------------------------------------------
void tiSpriteDraw(int slot);
void tiSpriteErase(int slot);

// ---------------------------------------------------------------------------
// Input — CALL KEY / CALL JOYST
// ---------------------------------------------------------------------------
int  tiReadKey();                                     // -1 == no key
void tiReadJoystick(int unit, int* outX, int* outY);  // each in {-4, 0, +4}

// ---------------------------------------------------------------------------
// BLE pairing — CALL PAIR / CALL UNPAIR
// ---------------------------------------------------------------------------
// CALL PAIR — open a 30-second pairing window so a new BLE HID device
// can join. Default no-op; projects with a BLE HID host override this
// with their own pairing trigger (e.g., BleHidHost::requestPairingMode).
void tiPair();
// CALL UNPAIR — forget every currently-bonded BLE HID peer. Default
// no-op; the host project routes this to its own NVS-clearing path.
void tiUnpair();

// ---------------------------------------------------------------------------
// Sound — CALL SOUND
// ---------------------------------------------------------------------------
// CALL SOUND(duration, f1, v1 [, f2, v2 [, f3, v3 [, f4, v4]]]).
// Voices 1..3 are tone, voice 4 is noise.
//   duration: milliseconds. Positive = wait for prior sound to finish
//             before starting; negative = cancel prior, start immediately.
//             Range -4250..+4250.
//   freq:     for tones, Hz (110..40000). For noise voice, -1..-8 TI
//             noise type. 0 means voice unused.
//   vol:      0 (loudest) .. 30 (silent), SN76489 convention.
//
// The interpreter handles the wait-for-previous and cancel-in-flight
// timing. The platform's tiSoundPlay just programs the chip. The
// platform is responsible for stopping voices when their duration
// elapses (or it can ignore duration and rely on tiSoundStop being
// called at the right time).
void tiSoundPlay(int duration,
                 int freq1, int vol1,
                 int freq2, int vol2,
                 int freq3, int vol3,
                 int freq4, int vol4);
void tiSoundStop();

#endif // TI_PLATFORM_H
