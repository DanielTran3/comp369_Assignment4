#ifndef _TOTHETOP_H
#define _TOTHETOP_H

#include <pthread.h>
#include "sprite.h"
#include "enemyhandler.h"
#include "platform.h"
#include "toTheTopDefs.h"

#define MODE_W GFX_AUTODETECT_WINDOWED
#define MODE_F GFX_AUTODETECT_FULLSCREEN

#define CHARACTER_WIDTH 8
#define LINE_SPACING 10

#define MAP "mappy/toTheTop.fmp"

#define WHITE makecol(255, 255, 255)
#define BLACK makecol(0, 0, 0)
#define LIGHT_GREEN makecol(100, 255, 0)
#define RED makecol(255, 0, 0)

void *soundsAndHelpThread(void *data);
void update_deathSpikes(void);
void update_time(void);
void drawImage(int datIndex);
void drawImage(int datIndex, BITMAP *bitmap);
void displayTitleScreen(FONT *titleFont, FONT *titleFont_sm);
void displayInstructions(FONT *titleFont, FONT *textFont);
void displayGameInformation(Sprite *player, BITMAP *dest);
void displayWinScreen();
void displayGameOverScreen(Sprite *player, FONT *gameOverTitleFont, FONT *gameOverTextFont);
int getHorizontalSpacing(const char *text);
void draw_pretty_box(const char * textToMeasure, int x, int y, int offset_x, int offset_y, int numLines);
BITMAP *grabframe(BITMAP *source, 
                  int width, int height, 
                  int startx, int starty, 
                  int columns, int frame);
int collided(int x, int y);
void drawHLineOfSprites(Sprite *sprite, BITMAP *dest, int xDistance, int yLocation, Sound *sounds);
void initializePlayer(Sprite *player);
Platform *initializeMovingPlatforms();
bool chooseToContinue();
void helpMenu(FONT *helpTitle, FONT *helpFont);

#endif
