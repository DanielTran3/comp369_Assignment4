#include <stdio.h>
#include "assignment2.h"

bool gameOver;
bool closeThreads = false;
int lineLength = 200;
pthread_mutex_t threadsafe = PTHREAD_MUTEX_INITIALIZER;

Sound *hardlineSounds;
DATAFILE *data;
PlayerInfo *player;

Sprite *cursor;
Sprite *target;

BITMAP *buffer;
BITMAP *background_image;
BITMAP *mascotFrames[MASCOTFRAMES];

FONT *letter_gothic_48;
FONT *letter_gothic_28;
FONT *letter_gothic_24;
FONT *letter_gothic_12;
FONT *lucida_calligraphy_36;
/*
	Thread for drawing the player's sprite and background the double buffer
*/
void* backgroundAndSpritesThreadLoop(void *data) {
	// Get the thread's ID
	int threadID = *((int *) data);
	while (!closeThreads) {
		
		if (pthread_mutex_lock(&threadsafe)) {
			textout_ex(buffer, font, "ERROR: Thread mutex was locked", 0, 0, WHITE, 0);
		}
		// Update the position of the cursor and check the cursors position in respect
		// to the end of the lines and to the target
		cursor->UpdatePosition();
		checkCursorOnBoundary(cursor);
		checkIfCursorPassesTarget(cursor, target);
		
		// Update target position outside of mutex lock for concurrency with other thread function;
		// Check if the user presses SPACE to hit the target.
		if (keypressed()) {
			if(key[KEY_SPACE]) {
				// Stop the if hitTheTarget function from triggering multiple times
				// when the space key is pressed.
				clear_keybuf();
				// If the player's cursor hits the target, then increrase the player's score
				// and move the target to a random location
				if (hitTheTarget(cursor, target)) {
					player->IncreaseScore(cursor, hardlineSounds);
					relocateTarget(cursor, target);
				}
			}
		}
		
		// Draw the background, to the double buffer
		blit(background_image, buffer, 0, 0, 0, 0, WIDTH, HEIGHT);
		
		// Draw a thick black line that the target is on and the cursor runs across, onto the double buffer
		hline(buffer, (WIDTH - lineLength) / 2, HEIGHT / 2 - 1, (WIDTH + lineLength) / 2, BLACK);
		hline(buffer, (WIDTH - lineLength) / 2, HEIGHT / 2, (WIDTH + lineLength) / 2, BLACK);
		hline(buffer, (WIDTH - lineLength) / 2, HEIGHT / 2 + 1, (WIDTH + lineLength) / 2, BLACK);
		
		// Draw the cursor and the buffer onto the double buffer
		draw_sprite(buffer, target->getImage(), target->getX(), target->getY());
		
		draw_sprite(buffer, cursor->getImage(), cursor->getX(), cursor->getY());
		if (pthread_mutex_unlock(&threadsafe)) {
			textout_ex(buffer, font, "ERRR: Thread mutex unlock error", 0, 0, WHITE, 0);
		}
		// Slow down the thread a little bit
		rest(1);
	}
	
	// Terminate the thread
	pthread_exit(NULL);
	
	return NULL;
}

/*
	A helper thread for opening the help menu or turning off/on the sound
*/
void *soundsAndHelpThread(void *data) {
	while(!closeThreads) {
		// Check if the user wants to display the help menu or pause the music
		helpMenu(lucida_calligraphy_36, letter_gothic_12);
		hardlineSounds->PollTurnOnOrOffMusic();
	}
}

/*
	Draws a fullscreen image given the filename input. Generally used to display a background image
*/
void drawImage(int datIndex) {
	BITMAP *image;
	image = (BITMAP *) data[datIndex].dat;
	
	blit(image, screen, 0, 0, 0, 0, WIDTH, HEIGHT);
}

/*
	Draws a fullscreen image from an input file onto a bitmap.
*/
void drawImage(int datIndex, BITMAP *bitmap) {
	BITMAP *image;
	image = (BITMAP *) data[datIndex].dat;
	
	blit(image, bitmap, 0, 0, 0, 0, WIDTH, HEIGHT);
}

/*
	Animate and displays the title screen
*/
void displayTitleScreen(FONT *titleFont, FONT *titleFont_sm) {
	BITMAP *image = (BITMAP *) data[TITLE_PCX].dat;
	
	BITMAP *buffer;
	buffer = create_bitmap(WIDTH, HEIGHT);
	// Keeps track of the left-most character until the animation passes that characters'
	// position defined in title_pos
	int pointer = 0;
	char title[9] = "HARDLINE"; // Char array containing individual letters of the title
	int title_pos[8] = {200, 230, 260, 290, 320, 350, 380, 410};
	
	// Loops to animate the title transition from left to right.
	for (int i = 0; i < HEIGHT; i++) {
		blit(image, buffer, 0, 0, 0, 0, WIDTH, HEIGHT);
		// Moves all of the characters who's positions are greater than the current character's final position
		if (i < title_pos[pointer]) {
			for (int k = pointer; k < strlen(title); k++) {
				textprintf_ex(buffer, titleFont, i, 110, BLACK, -1, "%c", title[k]);				
			}	
		}
		else { // If the current character's final position is passed, move onto the next character
			if (pointer < strlen(title)) {
				pointer++;					
			}
		}
		// Display all of the characters where i has passed their final position (not animated anymore)
		for (int j = 0; j < pointer; j++) {
			textprintf_ex(buffer, titleFont, title_pos[j], 110, BLACK, -1, "%c", title[j]);				
		}
		acquire_screen();
		blit(buffer, screen, 0, 0, 0, 0, WIDTH - 1, HEIGHT - 1);
		release_screen();
	}
	// Draws the animations for the line under the title. Multiple lines are drawn to create the effect of a thicker line
	for (int i = 210; i < 450; i++) {
		hline(buffer, 200, 185, i, BLACK);
		hline(buffer, 200, 186, i, BLACK);
		hline(buffer, 200, 187, i, BLACK);
		hline(buffer, 200, 188, i, BLACK);
		acquire_screen();
		blit(buffer, screen, 0, 0, 0, 0, WIDTH - 1, HEIGHT - 1);
		release_screen();
	}
	
	textprintf_centre_ex(screen, titleFont_sm, WIDTH / 2, HEIGHT / 2 + 9 * LINE_SPACING, BLACK, -1, "Press ENTER to Start");
	
	// Allows for toggling of sound and music on this screen
	while (!key[KEY_ENTER]) {
		hardlineSounds->PollTurnOnOrOffMusic();
	}
	destroy_bitmap(buffer);
	// Rest and clear any characters in the keyboard buffer so key presses don't transfer to the next screen
	rest(100);
	clear_keybuf();
}

/*
	Displays the instructions screen
*/
void displayInstructions() {
	int xOffset = WIDTH / 2;
	int yOffset = HEIGHT / 4;
	
	drawImage(INSTRUCTIONS_PCX);
	draw_pretty_box("If you miss or the cursor passes the target, you lose the game!", 70, yOffset, 10, 30, 21);	
	textout_centre_ex(screen, font, "Instructions", xOffset, yOffset, WHITE, 0);
	textout_centre_ex(screen, font, "Select the options on the following screens", xOffset, yOffset + 2 * LINE_SPACING, WHITE, 0);
	textout_centre_ex(screen, font, "by pressing the specified keys on the screen.", xOffset, yOffset + 3 * LINE_SPACING, WHITE, 0);
	
	textout_centre_ex(screen, font, "A cursor moves on the screen and your goal is", xOffset, yOffset + 5 * LINE_SPACING, WHITE, 0);
	textout_centre_ex(screen, font, "to press SPACE when the cursor touches the target!", xOffset, yOffset + 6 * LINE_SPACING, WHITE, 0);
	
	textout_centre_ex(screen, font, "Everytime you hit the target, your score increases", xOffset, yOffset + 8 * LINE_SPACING, WHITE, 0);
	textout_centre_ex(screen, font, "and the cursor will move in the opposite direction.", xOffset, yOffset + 9 * LINE_SPACING, WHITE, 0);
	textout_centre_ex(screen, font, "Every 5 target hits will increase the speed of the cursor!", xOffset, yOffset + 10 * LINE_SPACING, WHITE, 0);
	textout_centre_ex(screen, font, "If you miss or the cursor passes the target, you lose the game!", xOffset, yOffset + 11 * LINE_SPACING, WHITE, 0);
		
	textout_centre_ex(screen, font, "---Controls---", xOffset, yOffset + 14 * LINE_SPACING, WHITE, 0);
	textout_ex(screen, font, "Hit the Target: SPACE", xOffset - 75, yOffset + 15 * LINE_SPACING, WHITE, 0);
	textout_ex(screen, font, "Toggle Music  : CTRL + M", xOffset - 75, yOffset + 16 * LINE_SPACING, WHITE, 0);
	textout_ex(screen, font, "Help Menu     : CTRL + H", xOffset - 75, yOffset + 17 * LINE_SPACING, WHITE, 0);
	textout_centre_ex(screen, font, "Press ENTER to Continue", xOffset, yOffset + 20 * LINE_SPACING, WHITE, 0);
	
	// Allows for toggling of sound and music on this screen
	while (!key[KEY_ENTER]) {
		hardlineSounds->PollTurnOnOrOffMusic();
	}
	// Rest and clear any characters in the keyboard buffer so key presses don't transfer to the next screen
	rest(100);
	clear_keybuf();
}

/*
	Displays the difficulty selection screen
*/
void displayDifficultySelectionScreen(FONT *headerFont, FONT *selectionFont, Sprite *cursor) {
	drawImage(LEVELS_PCX);
	int xOffset = WIDTH / 4;
    int yOffset = HEIGHT / 6 + 50;
	textprintf_ex(screen, headerFont, xOffset / 2, yOffset, BLACK, -1, "Select A Difficulty");
	textprintf_ex(screen, selectionFont, xOffset, yOffset + 6 * LINE_SPACING, BLACK, -1, "1 - Easy");
	textprintf_ex(screen, selectionFont, xOffset, yOffset + 10 * LINE_SPACING, BLACK, -1, "2 - Medium");
	textprintf_ex(screen, selectionFont, xOffset, yOffset + 14 * LINE_SPACING, BLACK, -1, "3 - Hard");
	// Changes the difficulty depending on the user's input
	while (1) {
		hardlineSounds->PollTurnOnOrOffMusic();
		if (key[KEY_1]) {
			easy(cursor);
			break;
		}
		if (key[KEY_2]) {
			medium(cursor);
			break;
		}
		if (key[KEY_3]) {
			hard(cursor);
			break;
		}
		if (key[KEY_ENTER]) {
			break;
		}
	}
}

/*
	Displays the help screen
*/
void displayHelpScreen(FONT *helpTitle, FONT *helpFont) {
	int xOffset = WIDTH / 2;
	
	drawImage(HELP_BMP);
	textprintf_centre_ex(screen, helpTitle, xOffset, 10, WHITE, -1, "Help");
	textprintf_centre_ex(screen, helpFont, xOffset, HEIGHT / 2 + 12 * LINE_SPACING, WHITE, -1, "Press SPACE at the correct time to increase your score!");
	textprintf_centre_ex(screen, helpFont, xOffset, HEIGHT / 2 + 16 * LINE_SPACING, WHITE, -1, "Press ENTER to exit the help screen");
	textprintf_centre_ex(screen, helpFont, xOffset, HEIGHT / 2 + 18 * LINE_SPACING, WHITE, -1, "OR SPACE to display the instructions");

	// Exit the help screen and continue the game, or display the instructions screen.
	while(1) {
		// Allows for toggling of sound and music on this screen
		hardlineSounds->PollTurnOnOrOffMusic();
		if (key[KEY_ENTER]) {
			break;
		}
		if (key[KEY_SPACE]) {
			displayInstructions();
		}
	}
}

/*
	Displays the game over screen
*/
void displayGameOverScreen(PlayerInfo *player, FONT *gameOverFont) {
	drawImage(ENDING_PCX);
    int xOffset = WIDTH / 2;
    int yOffset = HEIGHT / 4 + 50;

    draw_pretty_box("Press Enter To Retry or ESC to Exit", xOffset / 2 + 20, yOffset, 30, 30, 14);
    textprintf_centre_ex(screen, gameOverFont, xOffset, yOffset - 20, WHITE, -1, "Game Over");
    textprintf_centre_ex(screen, font, xOffset, yOffset + 5 * LINE_SPACING, WHITE, -1, "Your Score:    %i", player->getScore());
    textprintf_centre_ex(screen, font, xOffset, yOffset + 6.5 * LINE_SPACING, WHITE, -1, "Highest Score: %i", player->getHighestScore());
    textprintf_centre_ex(screen, font, xOffset, yOffset + 9 * LINE_SPACING, WHITE, -1, "Your Level:    %i", player->getLevel());
    textprintf_centre_ex(screen, font, xOffset, yOffset + 10.5 * LINE_SPACING, WHITE, -1, "Highest Level: %i", player->getHighestLevel());
	textprintf_centre_ex(screen, font, xOffset, yOffset + 13 * LINE_SPACING, WHITE, -1, "Press Enter To Retry or ESC to Exit");
}

/*
	Displays the user's current stats, including the current level, player's score, and the player's highest score
*/
void displayUserInformation(PlayerInfo *player, BITMAP *buffer) {
	int xOffset = WIDTH / 2;
	int yOffset = 5;
	rectfill(buffer, 0, 0, WIDTH, yOffset + 10, BLACK);
	textprintf_ex(buffer, font, 10, yOffset, WHITE, 0, "Level: %i | Highest Level: %i", 
						 player->getLevel(), player->getHighestLevel());
	textprintf_ex(buffer, font, WIDTH - 210, yOffset, WHITE, 0, "Score: %i | High Score: %i", 
						 player->getScore(), player->getHighestScore());
	hline(buffer, 0, yOffset + 10, WIDTH, WHITE);
}

/*
	Get the number of characters in the inputted text
*/
int getHorizontalSpacing(const char *text) {
	return text_length(font, text) / CHARACTER_WIDTH;
}

/*
	Draw a box based on the text length and number of lines
*/
void draw_pretty_box(const char * textToMeasure, int x, int y, int offset_x, int offset_y, int numLines) {
	int textLength = getHorizontalSpacing(textToMeasure) * CHARACTER_WIDTH;
	rectfill(screen, x - offset_x, y - offset_y, x + textLength + offset_x, y + (numLines * LINE_SPACING) + offset_y, BLACK);
	rect(screen, x - offset_x, y - offset_y, x + textLength + offset_x, y + (numLines * LINE_SPACING) + offset_y, WHITE);
}

/*
	Function that returns true if the player wants to retry the game, or false if they want to quit the game.
*/
bool chooseToContinue() {
	while(1) {
		// Allows for toggling of sound and music on this screen
		hardlineSounds->PollTurnOnOrOffMusic();
		if (key[KEY_ESC]) {
			return false;
		}
		if (key[KEY_ENTER]) {
			return true;
		}
	}
}

/*
	Settings for easy difficulty. A long line and slow moving cursor
*/
void easy(Sprite *cursor) {
	lineLength = 500;
	cursor->setXDelay(4);
	cursor->setYDelay(0);
	cursor->setVelX(1.5);
	cursor->setVelY(0);
}

/*
	Settings for medium difficulty. A medium length line and medium speed moving cursor
*/
void medium(Sprite *cursor) {
	lineLength = 350;
	cursor->setXDelay(4);
	cursor->setYDelay(0);
	cursor->setVelX(2.0);
	cursor->setVelY(0);
}

/*
	Settings for hard difficulty. A short line and fast speed moving cursor
*/
void hard(Sprite *cursor) {
	lineLength = 250;
	cursor->setXDelay(4);
	cursor->setYDelay(0);
	cursor->setVelX(3.0);
	cursor->setVelY(0);
}

/*
	Function that checks if the cursor hit the boundary of the line. If it did, then
	reverse the cursor's direction
*/
void checkCursorOnBoundary(Sprite *cursor) {
	// Cursor hit the right side of the line
	if (cursor->getX() > (WIDTH + lineLength - cursor->getWidth()) / 2) {
			cursor->setX((WIDTH + lineLength - cursor->getWidth()) / 2);
			cursor->setDirection(-1);
		}
	// Cursor hit the left side of the line
	if (cursor->getX() < (WIDTH - lineLength - cursor->getWidth()) / 2) {
		cursor->setX((WIDTH - lineLength - cursor->getWidth()) / 2);
		cursor->setDirection(1);
	}
}

/*
	Determines if the cursor has hit the target
*/
bool hitTheTarget(Sprite *cursor, Sprite *target) {
	// Check if the center of the cursor is anywhere in the space of the target.
	// If so, the target was hit and true is returned
	if (target->PointInside(cursor->CenterX(), cursor->CenterY())) {
		return true;
	}
	// Cursor missed the target, so set gameOver flag to true and return false
	else {
		gameOver = true;
		return false;
	}
}

/*
	Moves the target to a random position after the cursor hit the target.
*/
void relocateTarget(Sprite *cursor, Sprite *target) {
	// Subtract half the target width and a small factor so the target doesn't appear on the end of the line,
	// since the detection for losing the game is based on the right/left edge of the target
	int randomLocation = (WIDTH - lineLength + target->getWidth()) / 2 + rand() % (lineLength - 2 * target->getWidth());
	target->setX(randomLocation);
	
	// Ensure the new target position is not overlapping with the cursor, otherwise generate a new position
	while (cursor->Collided(target, 0)) {
		randomLocation = (WIDTH - lineLength + target->getWidth()) / 2 + rand() % (lineLength  - 2 * target->getWidth());
		target->setX(randomLocation);
	}
	
	// Reverse the direction of the cursor after a target is hit
	cursor->ChangeDirection();

	// Change the side of the target that the cursor must pass depending on the target's
	// position in respect to the cursor
	setTargetSides(cursor, target);
}

/*
	Sets the side of the target that will cause the game to end if the cursor passes it.
*/
void setTargetSides(Sprite *cursor, Sprite *target) {
	// Depending on the randomLocation placement of the target in respect to the current location of the cursor,
	// this can be used to determine which side of the target the cursor has to pass in order to determine that
	// the player has lost.
	// If the target spawns to the right of the cursor, check for the cursor passing the right side of the target
	if (cursor->CenterX() < target->CenterX()) {
			target->setLeftOrRightSide(target->getX() + target->getWidth());
			target->setPassLeftSideToLose(false);
	}
	// If the target spawns to the left of the cursor, check for the cursor passing the left side of the target
	else {
		target->setLeftOrRightSide(target->getX());
		target->setPassLeftSideToLose(true);
	}
}

/*
	Function to check if the cursor has passed the target's side that was set by the setTargetSides() function
*/
void checkIfCursorPassesTarget(Sprite *cursor, Sprite * target) {
	// Check for cursor passing the left side
	if (target->getPassLeftSideToLose()) {
		if (cursor->CenterX() < target->getLeftOrRightSide()) {
			gameOver = true;
		}	
	}
	// Check for cursor passing the right side
	else {
		if (cursor->CenterX() > target->getLeftOrRightSide()) {
			gameOver = true;
		}
	}
	
}

/*
	Frame grabbing function from Chapter 9 of the Textbook
*/
BITMAP *grabframe(BITMAP *source, 
                  int width, int height, 
                  int startx, int starty, 
                  int columns, int frame)
{
    BITMAP *temp = create_bitmap(width,height);

    int x = startx + (frame % columns) * width;
    int y = starty + (frame / columns) * height;

    blit(source,temp,x,y,0,0,width,height);

    return temp;
}

/*
	Initializing the mascot (Sonic)'s position, velocity, and animation frames
*/
Sprite *createMascotAnimSprite() {
	// Create and load the sprite 
	Sprite *tempSprite = new Sprite();
	int ret = tempSprite->Load((BITMAP *) data[SONIC_BMP].dat);
    if (!ret) {
    	allegro_message("Error loading mascot sprite sheet.");
    	return NULL;
	}
	
	// Get the animation fromes for Sonic
	for (int n = 0; n < 4; n++) {
    	mascotFrames[n] = grabframe(tempSprite->getImage(), 37, 46, 0, 0, 4, n);
	}
	
	// Set sonic at the right side of the screen and his velocity negative so he moves to the left.
    tempSprite->setX(647);
    tempSprite->setY(100);
    tempSprite->setWidth(mascotFrames[0]->w);
    tempSprite->setHeight(mascotFrames[0]->h);
    tempSprite->setXDelay(1);
    tempSprite->setYDelay(0);
    tempSprite->setXCount(0);
    tempSprite->setYCount(0);
    tempSprite->setVelX(-5);
    tempSprite->setVelY(0);
    tempSprite->setCurFrame(0);
    tempSprite->setTotalFrames(4);
    tempSprite->setAnimColumns(4);
    tempSprite->setFrameCount(0);
    tempSprite->setFrameDelay(6);
    tempSprite->setAnimDir(1);
    return tempSprite;
}

/*
	Animation for the game's mascot (Sonic) running past the screen on a level up
*/
void animateMascot(BITMAP * buffer, Sprite *mascot, PlayerInfo *player, FONT *levelFont) {
	// check if the user has leveled up (the level up flag is on
	if (player->HasLeveled()) {
		// If the mascot hasn't reached the right side of the screen yet, update his position and animation frame
		if (mascot->getX() > 0) {
			mascot->UpdatePosition();
        	mascot->UpdateAnimation();
			mascot->DrawFrame(buffer);
			textprintf_centre_ex(buffer, levelFont, WIDTH / 2, 100, BLACK, -1, "LEVEL %i", player->getLevel());
		}
		else {
			// Reset the mascot to the right side of the screen and set the leveled up variable false
			mascot->setX(647);
			player->ResetLeveled();	
		}
	}
}

/*
	Reset some of the game parameters
*/
void restartGame(PlayerInfo *player, Sprite *cursor, Sprite *target) {
	cursor->setX((WIDTH - lineLength) / 2);
	cursor->setY(HEIGHT / 2);            
	target->setX((WIDTH + lineLength) / 2 - 2 * target->getWidth());
	target->setY(HEIGHT / 2  - target->getHeight() / 2);
	setTargetSides(cursor, target);
	player->setLevel(1);
	player->setNumHits(0);
	player->setScore(0);
	player->ResetLeveled();
}

/*
	Checks for CTRL + H key combination to play the pause sound and display the help screen
*/
void helpMenu(FONT *helpTitle, FONT *helpFont) {
	if ((key[KEY_LCONTROL] && key[KEY_H]) ||
		(key[KEY_RCONTROL] && key[KEY_H])) { 
		hardlineSounds->setSoundEffect(PAUSE_SFX_WAV);
		hardlineSounds->playSoundEffect();
		
		if (pthread_mutex_lock(&threadsafe)) {
			textout_ex(buffer, font, "ERROR: Thread mutex was locked", 0, 0, WHITE, 0);
		}
		displayHelpScreen(helpTitle, helpFont);
		if (pthread_mutex_unlock(&threadsafe)) {
			textout_ex(buffer, font, "ERRR: Thread mutex unlock error", 0, 0, WHITE, 0);
		}
	}
}

int main(void) {
	// Initialize Allegro, keyboard, screen, and sounds
	allegro_init();
	set_color_depth(24);
	install_keyboard();
	
	int ret = set_gfx_mode(MODE_W, WIDTH, HEIGHT, 0, 0);
	if (ret != 0) {
		allegro_message("Error setting up screen!");
		return 1;
	}
	
	if (install_sound(DIGI_AUTODETECT, MIDI_NONE, "") != 0) {
		allegro_message("Error Initializing Sound System");
		return 1;
	}
	data = load_datafile("hl.dat");
	if (data == NULL) {
		allegro_message("Error loading datafile");
		return 1;
	}
	hardlineSounds = new Sound(data);
	
	// Initialize fonts to be used in the game
	letter_gothic_48 = (FONT *) data[LETTER_GOTHIC_STD_48_PCX].dat;
	letter_gothic_28 = (FONT *) data[LETTER_GOTHIC_STD_28_PCX].dat;
	letter_gothic_24 = (FONT *) data[LETTER_GOTHIC_STD_24_PCX].dat;
	letter_gothic_12 = (FONT *) data[LETTER_GOTHIC_STD_12_PCX].dat;
	lucida_calligraphy_36 = (FONT *) data[LUCIDA_CALLIGRAPHY_36_PCX].dat;
	if (!letter_gothic_48 || !letter_gothic_24 || !letter_gothic_12 || !lucida_calligraphy_36) {
		allegro_message("Cannot find one or more font files");
		return 1;
	}
	
	// Display title screen
	displayTitleScreen(letter_gothic_48, letter_gothic_24);
	// Display Instructions screen
	displayInstructions();
	
	// Initialize player settings, cursor, target, and mascot frames
	player = new PlayerInfo();
	Sprite *mascot = createMascotAnimSprite();
	cursor = new Sprite();
	target = new Sprite();
	cursor->Load((BITMAP *) data[CURSOR_BMP].dat);
	target->Load((BITMAP *) data[TARGET_BMP].dat);
	// Display difficulty screen that allows the user to select their difficulty
	displayDifficultySelectionScreen(letter_gothic_28, letter_gothic_24, cursor);
	// Initialize the cursor at the left side of the line, offset by 2 times its width
	cursor->setX((WIDTH - lineLength) / 2);
	cursor->setY(HEIGHT / 2);
	// Initialize the target to the right side of the line, offset by 2 times its width
	target->setX((WIDTH + lineLength) / 2 - 2 * target->getWidth());
	target->setY(HEIGHT / 2  - target->getHeight() / 2);
	setTargetSides(cursor, target);
	
	// Create the double buffer
	buffer = create_bitmap(WIDTH, HEIGHT);
	
	// Load the background image to be used in the game
	background_image = (BITMAP *) data[BACKGROUND_1_PCX].dat;
	if (!background_image) {
		allegro_message("Error Loading file number %s from dat file", BACKGROUND_1_PCX);
	}

	// Initialize and create the threads
	pthread_t backgroundAndPlayerThread;
	pthread_t helperThread;
	int id;
	int threadID0 = 0;
	int threadID1 = 1;
	
	id = pthread_create(&backgroundAndPlayerThread, NULL, backgroundAndSpritesThreadLoop, (void *)&threadID0);
	id = pthread_create(&helperThread, NULL, soundsAndHelpThread, (void *)&threadID1);
	
	// Game loop.
	while (!key[KEY_ESC]) {
			
		// Lock the mutex for blitting to the double buffer and displaying the double buffer to the screen
		if (pthread_mutex_lock(&threadsafe)) {
			textout_ex(buffer, font, "ERROR: Thread mutex was locked", 0, 0, WHITE, 0);
		}
	
		// Draw the user's stats
		displayUserInformation(player, buffer);
		// If the user has just leveled up, draw the mascot
		animateMascot(buffer, mascot, player, letter_gothic_24);

		// Draw the double buffer onto the screen
		acquire_screen();
		blit(buffer, screen, 0, 0, 0, 0, WIDTH - 1, HEIGHT - 1);
		release_screen();
		
		// Check if the user has lost the game.
		if (gameOver) {
			// Play the Game Over sound effect
			hardlineSounds->setSoundEffect(GAMEOVER_SFX_WAV);
			hardlineSounds->playSoundEffect();
			
			// Display the Game Over screen
			displayGameOverScreen(player, lucida_calligraphy_36);
			
			// Give the player the opportunity to restart the game or quit it.
			if (chooseToContinue()) {
				// Restore game to default (bring user to starting screen)
				gameOver = false;
				rest(100);
				clear_bitmap(buffer);
				displayInstructions();
				displayDifficultySelectionScreen(letter_gothic_28, letter_gothic_24, cursor);
				restartGame(player, cursor, target);
				
				// Unlock the mutex before restarting the game
				if (pthread_mutex_unlock(&threadsafe)) {
					textout_ex(buffer, font, "ERRR: Thread mutex unlock error", 0, 0, WHITE, 0);
				}
				continue;
			}
			else {
				clear_bitmap(buffer);
				//pthread_mutex_unlock(&threadsafe);
				break;
			}
		}
		if (pthread_mutex_unlock(&threadsafe)) {
			textout_ex(buffer, font, "ERRR: Thread mutex unlock error", 0, 0, WHITE, 0);
		}
	}
	if (pthread_mutex_unlock(&threadsafe)) {
		textout_ex(buffer, font, "ERRR: Thread mutex unlock error", 0, 0, WHITE, 0);
	}

	closeThreads = true;
	rest(100);
	
	// Kill the mutex
	pthread_mutex_destroy(&threadsafe);
	
	// Clean up objects
	for (int f = 0; f < MASCOTFRAMES; f++) {
		destroy_bitmap(mascotFrames[f]);
	}
    destroy_bitmap(buffer);
	delete hardlineSounds;
	delete cursor;
	delete target;
	delete mascot;
	
	// Unload the datafile
	unload_datafile(data);
	allegro_exit();
	return 0;
}
END_OF_MAIN();
