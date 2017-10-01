#ifndef _ASSIGNMENT2_H
#define _ASSIGNMENT2_H 1

#include <allegro.h>
#include "assignment2.h"
#include "sprite.h"
#include "spritehandler.h"
#include "playerinfo.h"
#include "defines.h"

#define MODE_W GFX_AUTODETECT_WINDOWED
#define MODE_F GFX_AUTODETECT_FULLSCREEN

#define WIDTH 640
#define HEIGHT 480

#define CHARACTER_WIDTH 8
#define LINE_SPACING 10
#define MASCOTFRAMES 4

#define WHITE makecol(255, 255, 255)
#define BLACK makecol(0, 0, 0)
#define LIGHT_GREEN makecol(100, 255, 0)
#define RED makecol(255, 0, 0)


void drawImage(const char * filename);
void drawImage(const char * filename, BITMAP *bitmap);
void displayTitleScreen(FONT *titleFont, FONT *titleFont_sm);
void displayInstructions();
void displayDifficultySelectionScreen(FONT *headerFont, FONT *selectionFont, Sprite *cursor);
void displayHelpScreen(FONT *helpTitle, FONT *helpFont);
void displayGameOverScreen(PlayerInfo *player);
void displayUserInformation(PlayerInfo *player);
int getHorizontalSpacing(const char *text);
void draw_pretty_box(const char * textToMeasure, int x, int y, int offset_x, int offset_y, int numLines);
bool chooseToContinue();
void easy(Sprite *cursor);
void medium(Sprite *cursor);
void hard(Sprite *cursor);
void checkCursorOnBoundary(Sprite *cursor);
bool hitTheTarget(Sprite *cursor, Sprite *target);
void relocateTarget(Sprite *cursor, Sprite *target);
void setTargetSides(Sprite *cursor, Sprite *target);
void checkIfCursorPassesTarget(Sprite *cursor, Sprite * target);
BITMAP *grabframe(BITMAP *source, int width, int height, int startx, int starty, int columns, int frame);
Sprite *createMascotAnimSprite();
void animateMascot(BITMAP * buffer, Sprite *mascot, PlayerInfo *player, FONT *levelFont);
void restartGame(PlayerInfo *player, Sprite *cursor, Sprite *target);
void helpMenu(FONT *helpTitle, FONT *helpFont);
#endif
