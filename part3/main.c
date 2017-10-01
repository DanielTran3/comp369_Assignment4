#include <allegro.h>
#include "toTheTop.h"

int mapxoff, mapyoff;
int oldSpikesY = BOTTOM;
volatile int spikesY = BOTTOM;
volatile int timeElapsed = 0;
int gameoverFlag = 0;
bool closeThreads = false;
pthread_mutex_t threadsafe = PTHREAD_MUTEX_INITIALIZER;

Sprite *spike;
Sprite *player;
Platform *cloudPlatforms;

Sound *toTheTopSounds;
DATAFILE *data;

BITMAP *buffer;

FONT *tempus_sans_itc_48;
FONT *tempus_sans_itc_24;
FONT *tempus_sans_itc_9;

/*
	Thread for blitting mappy foreground and background tiles, moving platforms,, and spikes
*/
void *secondaryThreadLoop(void *data) {
	while(!closeThreads) {
		if (pthread_mutex_lock(&threadsafe)) {
			textout_ex(buffer, font, "ERROR: Thread mutex was locked", 0, 0, WHITE, 0);
		}
		//update the map scroll position
		mapxoff = WIDTH / 2;
		mapyoff = player->getY() - HEIGHT / 2;

        //avoid moving beyond the map edge
		if (mapxoff < 0) mapxoff = 0;
		if (mapxoff > (mapwidth * mapblockwidth - WIDTH))
            mapxoff = mapwidth * mapblockwidth - WIDTH;
		if (mapyoff < 0) 
            mapyoff = 0;
		if (mapyoff > (mapheight * mapblockheight - HEIGHT)) 
            mapyoff = mapheight * mapblockheight - HEIGHT;
        
		//draw the background tiles
		MapDrawBG(buffer, mapxoff, mapyoff, 0, 0, WIDTH-1, HEIGHT-1);

        //draw foreground tiles
		MapDrawFG(buffer, mapxoff, mapyoff, 0, 0, WIDTH-1, HEIGHT-1, 0);
		
		// If mapyoff + HEIGHT (Bottom of the screen) > spikesY, then the spikes should be
		// displayed on the screen.
		if (mapyoff + HEIGHT > spikesY) {
			// Display spikes from HEIGHT - (mapyoff + HEIGHT - spikesY) to the bottom of the screen
			drawHLineOfSprites(spike, buffer, WIDTH, spikesY - mapyoff, toTheTopSounds);
		}
		
		// Draw the platforms if they are in range
		cloudPlatforms->DrawPlatforms(buffer, mapyoff, mapxoff, mapyoff, player);
		if (pthread_mutex_unlock(&threadsafe)) {
			textout_ex(buffer, font, "ERROR: Thread mutex unlock error", 0, 0, WHITE, 0);
		}
		rest(1);
	}
}

/*
	A helper thread for opening the help menu or turning off/on the sound
*/
void *soundsAndHelpThread(void *data) {
	while(!closeThreads) {
		// Check if the user wants to display the help menu or pause the music
		helpMenu(tempus_sans_itc_48, tempus_sans_itc_9);
		toTheTopSounds->PollTurnOnOrOffMusic();
	}
}

/*
	Update the level that the spikes are at
*/
void update_deathSpikes(void) {
	spikesY -= mapblockheight;
}
END_OF_FUNCTION(update_deathSpikes)

/*
	Update the elapsed time
*/
void update_time(void) {
	timeElapsed++;
}
END_OF_FUNCTION(update_time);

/*
	Draws a fullscreen image given the filename input. Generally used to display a background image
*/
void drawImage(int datIndex) {
	BITMAP *image;
	image = (BITMAP *) data[datIndex].dat;
//	if (!image) {
//		allegro_message("Error Loading %s", datIndex);
//	}
	
	blit(image, screen, 0, 0, 0, 0, WIDTH, HEIGHT);
	//destroy_bitmap(image);
}

/*
	Draws a fullscreen image from an input file onto a bitmap.
*/
void drawImage(int datIndex, BITMAP *bitmap) {
	BITMAP *image;
	image = (BITMAP *) data[datIndex].dat;
//	if (!image) {
//		allegro_message("Error Loading %s", datIndex);
//	}
	
	blit(image, bitmap, 0, 0, 0, 0, WIDTH, HEIGHT);
	//destroy_bitmap(image);
}

/*
	Animate and displays the title screen
*/
void displayTitleScreen(FONT *titleFont, FONT *titleFont_sm) {
	BITMAP *image = (BITMAP *) data[TITLEBACKGROUND_PCX].dat;
	
	BITMAP *buffer;
	buffer = create_bitmap(WIDTH, HEIGHT);
	// Keeps track of the left-most character until the animation passes that characters'
	// position defined in title_pos
	int pointer = 0;
	char title[11] = "To The Top"; // Char array containing individual letters of the title
	int title_pos[10] = {150, 180, 210, 240, 270, 300, 330, 360, 390, 430};
	
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
	for (int i = 150; i < 450; i++) {
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
		toTheTopSounds->PollTurnOnOrOffMusic();
	}
	destroy_bitmap(buffer);
	// Rest and clear any characters in the keyboard buffer so key presses don't transfer to the next screen
	rest(100);
	clear_keybuf();
}

/*
	Displays the instructions screen
*/
void displayInstructions(FONT *titleFont, FONT *textFont) {
	int xOffset = WIDTH / 2;
	int yOffset = HEIGHT / 8;
	int controlsX = xOffset - 125;
	
	drawImage(INSTRUCTIONSBACKGROUND_PCX);
	draw_pretty_box("Reach the entrance to the Castle in the Sky                  ", 70, yOffset, 10, 30, 32);	
	textout_centre_ex(screen, titleFont, "Instructions", xOffset, yOffset, WHITE, 0);
	textout_centre_ex(screen, textFont, "Reach the entrance to the Castle in the Sky", xOffset, yOffset + 10 * LINE_SPACING, WHITE, 0);
	textout_centre_ex(screen, textFont, "while avoiding enemies and traps!", xOffset, yOffset + 12 * LINE_SPACING, WHITE, 0);

	textout_centre_ex(screen, textFont, "---Controls---", xOffset, yOffset + 15 * LINE_SPACING, WHITE, 0);
	textout_ex(screen, textFont, "Move Left        : LEFT ARROW KEY", controlsX, yOffset + 17 * LINE_SPACING, WHITE, 0);
	textout_ex(screen, textFont, "Move Right       : RIGHT ARROW KEY", controlsX, yOffset + 19 * LINE_SPACING, WHITE, 0);
	textout_ex(screen, textFont, "Jump             : SPACE", controlsX, yOffset + 21 * LINE_SPACING, WHITE, 0);
	textout_ex(screen, textFont, "Toggle Music      : CTRL + M", controlsX, yOffset + 23 * LINE_SPACING, WHITE, 0);
	textout_ex(screen, textFont, "Help Menu        : CTRL + H", controlsX, yOffset + 25 * LINE_SPACING, WHITE, 0);
	textout_centre_ex(screen, textFont, "Press ENTER to Continue", xOffset, yOffset + 28 * LINE_SPACING, WHITE, 0);
	
	// Allows for toggling of sound and music on this screen
	while (!key[KEY_ENTER]) {
		toTheTopSounds->PollTurnOnOrOffMusic();
	}
	// Rest and clear any characters in the keyboard buffer so key presses don't transfer to the next screen
	rest(100);
	clear_keybuf();
}

/*
	Displays the player's current level and the time that has elapsed
*/
void displayGameInformation(Sprite *player, BITMAP *dest) {
	int xOffset = WIDTH / 2;
	int yOffset = 5;
	rectfill(dest, 0, 0, WIDTH, yOffset + 10, BLACK);
	textprintf_ex(dest, font, 10, yOffset, WHITE, 0, "Level Reached: %i       Spikes Level Reached: %i", 
				  player->getLevelReached(), mapheight - spikesY / mapblockheight);
	textprintf_ex(dest, font, WIDTH - 210, yOffset, WHITE, 0, "Time Elapsed: %i seconds", timeElapsed);
	hline(dest, 0, yOffset + 10, WIDTH, WHITE);
}

/*
	Displays the Winner Screen and user details
*/
void displayWinScreen() {
	int xOffset = WIDTH / 2 + 10;
	int yOffset = HEIGHT / 2;
	FONT *brush_script_mt_48 = (FONT *) data[BRUSH_SCRIPT_MT_48_PCX].dat;
	FONT *chaparral_pro_light_12 = (FONT *) data[CHAPARRAL_PRO_LIGHT_12_PCX].dat;
	if (!brush_script_mt_48 || !chaparral_pro_light_12) {
		allegro_message("Cannot find one or more font files");
	}
	drawImage(WINBACKGROUND_BMP);
	draw_pretty_box("Congratulations! You passed all 234 levels and rescued Ophelia!", xOffset / 4, yOffset / 2, 0, 30, 25);
	textprintf_centre_ex(screen, brush_script_mt_48, xOffset, yOffset / 2, WHITE, 0, "Congratulations!");
	textprintf_centre_ex(screen, chaparral_pro_light_12, xOffset, yOffset, WHITE, 0, "You passed all 234 levels and rescued Ophelia!", timeElapsed);
	textprintf_centre_ex(screen, chaparral_pro_light_12, xOffset, yOffset + 4 * LINE_SPACING, WHITE, 0, "Time Elapsed: %i Seconds", timeElapsed);
	textprintf_centre_ex(screen, chaparral_pro_light_12, xOffset, yOffset + 8 * LINE_SPACING, WHITE, 0, "Press ESC to Quit!");
	toTheTopSounds->stopMusic();
	toTheTopSounds->setBGM(BGM_WIN_WAV);
	toTheTopSounds->playMusic();
}

/*
	Displays the game over screen
*/
void displayGameOverScreen(Sprite *player, FONT *gameOverTitleFont, FONT *gameOverTextFont) {
    int xOffset = WIDTH / 2;
    int yOffset = HEIGHT / 4 + 50;

    draw_pretty_box("Press Enter To Retry or ESC to Exit", xOffset / 2 + 20, yOffset, 30, 30, 14);
    textprintf_centre_ex(screen, gameOverTitleFont, xOffset, yOffset - 20, WHITE, -1, "Game Over");
	textprintf_centre_ex(screen, gameOverTextFont, xOffset, yOffset + 8 * LINE_SPACING, WHITE, -1, "Level Reached: %i", player->getLevelReached());
	textprintf_centre_ex(screen, gameOverTextFont, xOffset, yOffset + 10 * LINE_SPACING, WHITE, -1, "Time Elapsed: %i Seconds", timeElapsed);
	textprintf_centre_ex(screen, gameOverTextFont, xOffset, yOffset + 13 * LINE_SPACING, WHITE, -1, "Press Enter To Retry or ESC to Exit");
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

//tile grabber
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
	Check for collision with a mappy block
*/
int collided(int x, int y)
{
    BLKSTR *blockdata;
	blockdata = MapGetBlock(x/mapblockwidth, y/mapblockheight);
	return blockdata->tl;
}

/*
	Draw a line of sprites. This is used for drawing a line of spike sprites
*/
void drawHLineOfSprites(Sprite *sprite, BITMAP *dest, int xDistance, int yLocation, Sound *sounds) {
	int tempY = yLocation;
	int soundsFlag = 0;
	while (tempY < HEIGHT) {
		for (int x = 0; x < mapwidth; x++) {
			sprite->setX(x * mapblockwidth);
			sprite->setY(tempY);
			sprite->Draw(dest, 0, 0);
		}
		tempY += sprite->getHeight();
	}
	// If oldSpikesY != spikesY, then spikes have been displayed because spikesY has been updated by the timer
	if (oldSpikesY != spikesY) {
		sounds->setSoundEffect(SFX_SPIKES_WAV);
		sounds->playSoundEffect();
		oldSpikesY = spikesY;
	}
}

/*
	Function to initialize the player's starting position and settings;
*/
void initializePlayer(Sprite *player) {
	player->Load((BITMAP *) data[PLAYER_BMP].dat);
	player->setX(WIDTH / 2);
	player->setY((1200 * mapblockheight) - player->getHeight() - mapblockheight - 1);
	player->setWidth(24);
	player->setHeight(22);
	player->setAnimColumns(11);
	player->setCurFrame(8);
	player->setFrameDelay(10);
	player->setXDelay(1);
	player->setYDelay(5);
	player->setVelX(-3);
	player->setVelY(5);
}

/*
	Moving platforms in Stage 3, sky stage
*/
Platform *initializeMovingPlatforms() {
	int movingPlatformsStartingLevel = 1313;
	int movingSpeed = 0;
	Platform *movingPlatforms = new Platform(data);
	while (movingPlatforms->getNumPlatforms() < NUM_PLATFORMS) {
		// Platforms move between 1 to 2 in speed
		movingSpeed = 1 + rand() % 2;
		
		movingPlatforms->CreatePlatform(mapblockwidth + rand() % (WIDTH - 3 * mapblockwidth), movingPlatformsStartingLevel * mapblockheight, movingSpeed, 0);
		movingPlatformsStartingLevel -= 2;
	}
	return movingPlatforms;
}

/*
	Function that returns true if the player wants to retry the game, or false if they want to quit the game.
*/
bool chooseToContinue() {
	while(1) {
		// Allows for toggling of sound and music on this screen
		toTheTopSounds->PollTurnOnOrOffMusic();
		if (key[KEY_ESC]) {
			return false;
		}
		if (key[KEY_ENTER]) {
			return true;
		}
	}
}

/*
	Checks for CTRL + H key combination to play the pause sound and display the help screen
*/
void helpMenu(FONT *helpTitle, FONT *helpFont) {
	if ((key[KEY_LCONTROL] && key[KEY_H]) ||
		(key[KEY_RCONTROL] && key[KEY_H])) {
		if (pthread_mutex_lock(&threadsafe)) {
			textout_ex(buffer, font, "ERROR: Thread mutex was locked", 0, 0, WHITE, 0);
		} 
		displayInstructions(helpTitle, helpFont);
		if (pthread_mutex_unlock(&threadsafe)) {
			textout_ex(buffer, font, "ERROR: Thread muetx unlock error", 0, 0, WHITE, 0);
		}
	}
}

int main(void) {
	// Initial Variables

	int spikeTimer = 3000;
	int spikeYThreshold = 1425;
	gameoverFlag = 0;
	
	// Set up Allegro
	allegro_init();	
	set_color_depth(24);
	install_keyboard();
	install_timer();
	srand(time(NULL));
	
	int ret = set_gfx_mode(MODE_W, WIDTH, HEIGHT, 0, 0);
	if (ret != 0) {
		allegro_message("Error setting up the screen!");
		return 1;
	}
	
	if (install_sound(DIGI_AUTODETECT, MIDI_NONE, "") != 0) {
		allegro_message("Error initializing sound system!");
		return 1;
	}
	data = load_datafile("tothetop.dat");
	// Set up sounds
	toTheTopSounds = new Sound(data);
	
	// Create double buffer
	buffer = create_bitmap(WIDTH, HEIGHT);
	clear(buffer);
	
	// Load Mappy Map
	MapLoad(MAP);
	
	// Initialize Spike Sprite
	spike = new Sprite();
	ret = spike->Load((BITMAP *) data[DEATHSPIKE_BMP].dat);
	if (ret == 0) {
		allegro_message("Error loading mappy/deathSpike.bmp");
		return 1;
	}
	spike->setWidth(32);
	spike->setHeight(32);
	spike->setX(0);
	spike->setY(spikesY);
	
	// Initialize Player
	player = new Sprite();
	initializePlayer(player);

	// Initialize Enemies in a enemyHandler
	EnemyHandler *enemyHandler = new EnemyHandler(data);
	enemyHandler->SpawnEnemies();

	// Create the moving cloud platforms to be used in Stage 3
	cloudPlatforms = initializeMovingPlatforms();
	
	tempus_sans_itc_48 = (FONT *) data[TEMPUS_SANS_ITC_48_PCX].dat;
	tempus_sans_itc_24 = (FONT *) data[TEMPUS_SANS_ITC_24_PCX].dat;
	tempus_sans_itc_9 = (FONT *) data[TEMPUS_SANS_ITC_9_PCX].dat;
	if (!tempus_sans_itc_48 || !tempus_sans_itc_24 || !tempus_sans_itc_9) {
		allegro_message("Cannot find one or more font files");
		return 1;
	}
		
	displayTitleScreen(tempus_sans_itc_48, tempus_sans_itc_24);
	displayInstructions(tempus_sans_itc_48, tempus_sans_itc_9);
	
	// Install interrupts last, because we don't want them to start up when the game starts
	// Update the spike's location every 3 seconds to move up a level
	LOCK_VARIABLE(spikesY);
    LOCK_FUNCTION(update_deathSpikes);
    install_int(update_deathSpikes, spikeTimer);
    
    // Update the timer that the user is alive
	LOCK_VARIABLE(timeElapsed);
	LOCK_FUNCTION(update_time);
	install_int(update_time, 1000);
	
	pthread_t helperThread;
	pthread_t secondaryThread;
	int id;
	int threadID0 = 0;
	int threadID1 = 1;
	
	id = pthread_create(&secondaryThread, NULL, secondaryThreadLoop, (void *) &threadID0);
	id = pthread_create(&helperThread, NULL, soundsAndHelpThread, (void *) &threadID1);
	
	while(!key[KEY_ESC]) {
		if (pthread_mutex_lock(&threadsafe)) {
			textout_ex(buffer, font, "ERROR: Thread mutex was locked", 0, 0, WHITE, 0);
		} 
		player->UpdateLevelReached();
		player->PlayerControls(toTheTopSounds);
		
		// Update the spike's timer and threshold if the current threshold is passed.
		if (player->getY() < (spikeYThreshold * mapblockheight)) {
			if (spikeTimer > 1000) {
				spikeYThreshold -= 75;
				spikeTimer -= 1000;
		    	install_int(update_deathSpikes, spikeTimer);
			}
		}
		
		// See if player contacts a deadly block
		if (player->GetBlockData1(player->getX(), player->getY()) == 1) {
			gameoverFlag = 1;
		}
		
		// If the player reaches touches the block that has userdata1 with a value 2, they win
		if (player->GetBlockData1(player->getX(), player->getY()) == 2) {
			gameoverFlag = -1;
		}
		
		// Draws Enemy and checks for player collision with enemy
		if (enemyHandler->DrawEnemies(buffer, mapyoff, mapxoff, mapyoff, player)) {
			gameoverFlag = 1;
		}
		// Draw the player
		player->DrawFrame(buffer, mapxoff, mapyoff);
		
		// Draw player statistics
		displayGameInformation(player, buffer);
		
		// Draw the screen
		vsync();
        acquire_screen();
		blit(buffer, screen, 0, 0, 0, 0, WIDTH-1, HEIGHT-1);
        release_screen();
		
		// Check if the bottom of the character has contacted the spikes
		if (player->getY() + player->getHeight() >= spikesY) {
			gameoverFlag = 1;
		}
		
		if (gameoverFlag == 1) {
			toTheTopSounds->stopMusic();
			toTheTopSounds->setSoundEffect(SFX_DEATH_WAV);
			toTheTopSounds->playSoundEffect();
			// Ensure spikes are drawn, to show death from the spikes properly, since the spikes update
			// occur after spikes are drawn, but not draw the next level of spikes
			drawHLineOfSprites(spike, buffer, WIDTH, spikesY - mapyoff, toTheTopSounds);
			blit(buffer, screen, 0, 0, 0, 0, WIDTH-1, HEIGHT-1);
			displayGameOverScreen(player, tempus_sans_itc_48, tempus_sans_itc_9);
			
			// Let the sound effect play out a bit
			rest(1000);
			
			// If they choose to restart, reset game values
			if (chooseToContinue()) {
				gameoverFlag = 0;
				spikeTimer = 3000;
				spikeYThreshold = 1425;
				spikesY = BOTTOM;
				timeElapsed = 0;
				toTheTopSounds->playMusic();
				player->setX(WIDTH / 2);
				player->setY((1499 * mapblockheight) - player->getHeight() - mapblockheight - 1);
				
				// Respawn enemies
				enemyHandler = new EnemyHandler(data);
				enemyHandler->SpawnEnemies();
				
				// Reset interrupts
				remove_int(update_time);
				install_int(update_time, 1000);
				remove_int(update_deathSpikes);
				install_int(update_deathSpikes, spikeTimer);
			}
			else {
				break;
			}
		}
		
		if (gameoverFlag == -1) {
			displayWinScreen();
			while(!key[KEY_ESC]);
			break;
		}
		if (pthread_mutex_unlock(&threadsafe)) {
			textout_ex(buffer, font, "ERROR: Thread mutex unlock error", 0, 0, WHITE, 0);
		}
	}
	
	closeThreads = true;
	// Clean up the game
	remove_int(update_time);
	remove_int(update_deathSpikes);
	destroy_bitmap(buffer);

	delete enemyHandler;
	delete cloudPlatforms;
	delete player;
	delete spike;
	
	allegro_exit();
	return 1;
}
END_OF_MAIN();
