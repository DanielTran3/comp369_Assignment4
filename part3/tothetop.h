#ifndef _TOTHETOP_H
#define _TOTHETOP_H

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

void displayGameInformation(Sprite *player, BITMAP *dest);
void displayGameOverScreen();
int getHorizontalSpacing(const char *text);
void draw_pretty_box(const char * textToMeasure, int x, int y, int offset_x, int offset_y, int numLines);

#endif
