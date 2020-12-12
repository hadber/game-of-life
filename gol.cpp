#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <sstream>

using namespace std;

typedef struct {
	int x;
	int y;
} coord;

const int PANEL = 100;
const int THICKNESS = 3; // panel thickness

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800+PANEL;

const int BOARD_SIZE_X = SCREEN_WIDTH/10;
const int BOARD_SIZE_Y = (SCREEN_HEIGHT-PANEL)/10;

SDL_Window* gWindow = NULL; // The window we render to
SDL_Renderer* gRenderer = NULL; // The renderer
TTF_Font* gFont = NULL;

bool init();
void close();
void _drawFillRect(SDL_Color&, int, int, int, int);
void _drawOutlineRect(SDL_Color&, int, int, int, int);

int main(int argc, char* args[]) {

	std::cout << "This is working." << std::endl;

	if(!init()) {
		printf("Failed to initialize!\n");
		return 1;
	}
//	new Int(SCREEN_WIDTH * SCREEN_HEIGHT);
	bool rText = false;
	bool quit = false;
	bool paused = true;
	vector<coord> v;

	int board[BOARD_SIZE_X][BOARD_SIZE_Y] = {0};
	int nx[8] = {0, 1, 1, 1, 0, -1, -1, -1}; // neighbours
	int ny[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
	int previous = 0;
	int ticks = 0;
	int tpop = 0;

	SDL_Event e; // event handler
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(gFont, "", {0x10, 0x19, 0x2E, 0xFF});
	SDL_Texture* Message = SDL_CreateTextureFromSurface(gRenderer, surfaceMessage);

	while(!quit) { //While application is running
		while(SDL_PollEvent( &e )) {	// Handle events in queue
			switch(e.type) {
				case SDL_QUIT: {
					quit = true;
					break;
				}
				case SDL_MOUSEBUTTONDOWN: {
					SDL_MouseButtonEvent* me = (SDL_MouseButtonEvent*)&e;
					if(me->windowID == SDL_GetWindowID(gWindow)) { // potentially redundant
						//cout << "x: " << me->x << " y: " << me->y << endl;
						int cx = int(me->x/10);
						int cy = int(me->y/10);
						if(cx >= 0 && cx < BOARD_SIZE_X && cy >= 0 && cy < BOARD_SIZE_Y) {
							if(me->button == SDL_BUTTON_LEFT) {
								if(board[cx][cy] == 0) {
									board[cx][cy] = 1;
									tpop++;
								}
								//cout << "mouseevent" << cx << cy << endl;
							}
							else if(me->button == SDL_BUTTON_RIGHT) {
								if(board[cx][cy] == 1) {
									board[cx][cy] = 0;
									tpop--;
								}
							}
						}
					}
					break;
				}
				case SDL_KEYDOWN: {
					switch(e.key.keysym.sym) {
						case SDLK_SPACE:
							if(paused) {
								cout << "Game unpaused!" << endl;
								paused = false;
							} else {
								cout << "Game paused!" << endl;
								paused = true;
							}
						break;
					}
					break;
				}
			}
		}

		// #2c3448 - R44 G52 B72
		// Colors start here
		SDL_Color bg_Color = {0x2C, 0x34, 0x48, 0xFF}; // background color
		SDL_Color filled = {0x9D, 0xA3, 0xB3, 0xFF}; // #9da3b3
		SDL_Color panel_outline = {0x10, 0x19, 0x2E, 0xFF}; //#10192e
		// Colors end here

		// Clear screen
		SDL_SetRenderDrawColor(gRenderer, bg_Color.r, bg_Color.g, bg_Color.b, bg_Color.a);
		SDL_RenderClear(gRenderer);

		if(SDL_GetTicks() - previous >= 100)  {
			previous = SDL_GetTicks();
			if(!paused) { // do the game logic here
				int newboard[BOARD_SIZE_X][BOARD_SIZE_Y];
				std::copy(&board[0][0], &board[0][0]+BOARD_SIZE_X*BOARD_SIZE_Y, &newboard[0][0]);
				for(int i = 0; i < BOARD_SIZE_X; i++) {
					for(int j = 0; j < BOARD_SIZE_Y; j++) {
						int alive = 0;
						int dead = 0;
						for(int k = 0; k < 8; k++) {
							int newx = i - nx[k];
							int newy = j - ny[k];
							// Check whether the new coordinates are valid
							if(newx >= 0 && newx < BOARD_SIZE_X && newy >= 0 && newy < BOARD_SIZE_Y) {
								//cout << "valid: " << newx << "," << newy << "for" << i << "," << j << endl;
								if(board[newx][newy] == 0)
									dead++;
								else
									alive++;
							}
						}
						if(board[i][j] == 1 && (alive == 2 || alive == 3)) {
							continue;
						} else if(board[i][j] == 0 && alive == 3) {
							newboard[i][j] = 1;
							tpop++;
						} else {
							if(board[i][j] == 1) {
								newboard[i][j] = 0;
								tpop--;
							}
						}
					}
				}
				std::copy(&newboard[0][0], &newboard[0][0]+BOARD_SIZE_X*BOARD_SIZE_Y, &board[0][0]);

				//	SDL_Surface* surfaceMessage;
				//	SDL_Texture* Message;

				char paneltext[64];
				snprintf(paneltext, 64, "Ticks: %i Population: %i", ticks++, tpop);
				surfaceMessage = TTF_RenderText_Shaded(gFont, paneltext, panel_outline, bg_Color);
				Message = SDL_CreateTextureFromSurface(gRenderer, surfaceMessage);

				SDL_FreeSurface(surfaceMessage);
				//SDL_DestroyTexture(Message);
			}
		}

		SDL_Rect Message_rect = {20, SCREEN_HEIGHT-PANEL+20, 450, 75}; // Text position
		SDL_RenderCopy(gRenderer, Message, NULL, &Message_rect);

		for(int i = 0; i < BOARD_SIZE_X; i++) {
			for(int j = 0; j < BOARD_SIZE_Y; j++) {
				if(board[i][j] == 1) {
					_drawFillRect(filled, i*10, j*10, 10, 10);
				}
			}
		}

		// Draw of panel
/*		for(int t = 0; t < THICKNESS; t++) {
			cout << 0 + t << " " << SCREEN_HEIGHT-PANEL + t << " " << SCREEN_WIDTH - t - 1 << " " << PANEL - t - 1 << endl;
			_drawOutlineRect(panel_outline, 0 + t, SCREEN_HEIGHT-PANEL + t, SCREEN_WIDTH - (t*2), PANEL - (t*2));
		}
*/		//
		SDL_SetRenderDrawColor(gRenderer, panel_outline.r, panel_outline.g, panel_outline.b, panel_outline.a);
		for(int t = 0; t < THICKNESS; t++) {
			SDL_RenderDrawLine(gRenderer, 0, SCREEN_HEIGHT-PANEL + t, SCREEN_WIDTH, SCREEN_HEIGHT-PANEL + t);
		}

		SDL_RenderPresent(gRenderer); //Update screen

/*		if(Message != NULL && surfaceMessage != NULL) {
			SDL_FreeSurface(surfaceMessage);
			SDL_DestroyTexture(Message);
		}
*/
	}

	//Free resources and close SDL
	SDL_DestroyTexture(Message);
	close();
	return 0;
}

bool init() {
	bool success = true;

	if(TTF_Init() < 0) {
		printf("TTF_Init error: %s\n", TTF_GetError());
		success = false;
	}
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	} else { //Set texture filtering to linear
		if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
			printf("Warning: Linear texture filtering not enabled!");
		//Create window
		gWindow = SDL_CreateWindow("Game of life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if(gWindow == NULL) {
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else { //Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if(gRenderer == NULL) {
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}	else { //Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				gFont = TTF_OpenFont("font.ttf", 72);
				if(!gFont) {
					printf("TTF_OpenFont: %s\n", TTF_GetError());
					success = false;
				}
			}
		}
	}
	return success;
}

void close()
{
	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	TTF_CloseFont(gFont);

	//Quit SDL subsystems
	SDL_Quit();
	TTF_Quit();
}

void _drawFillRect(SDL_Color &col, int x, int y, int width, int height) {
	SDL_Rect fillRect = {x, y, width, height};
	SDL_SetRenderDrawColor(gRenderer, col.r, col.g, col.b, col.a);
	SDL_RenderFillRect(gRenderer, &fillRect);
}

void _drawOutlineRect(SDL_Color &col, int x, int y, int width, int height) {
	SDL_Rect outlineRect = {x, y, width, height};
	SDL_SetRenderDrawColor(gRenderer, col.r, col.g, col.b, col.a);
	SDL_RenderDrawRect(gRenderer, &outlineRect);
}

/*
int **multArray = new int*[y_max];
for (int i = 0; i < y_max; i++) {
  multArray[i] = new int[x_max];
}
Matrix in C++???!?!?
*/
