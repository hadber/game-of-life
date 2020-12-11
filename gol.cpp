#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <algorithm>

using namespace std;

typedef struct {
	int x;
	int y;
} coord;


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

const int BOARD_SIZE_X = SCREEN_WIDTH/10;
const int BOARD_SIZE_Y = SCREEN_WIDTH/10;

SDL_Window* gWindow = NULL; // The window we render to
SDL_Renderer* gRenderer = NULL; // The renderer

bool init();
void close();
void _drawFillRect(SDL_Color&, int, int, int, int);

int main(int argc, char* args[]) {

	std::cout << "This is working." << std::endl;

	if(!init()) {
		printf("Failed to initialize!\n");
		return 1;
	}

//	new Int(SCREEN_WIDTH * SCREEN_HEIGHT);

	bool quit = false;
	bool paused = true;
	SDL_Event e; // event handler
	int previous = 0;
	vector<coord> v;

	int board[BOARD_SIZE_X][BOARD_SIZE_Y] = {0};
	int nx[8] = {0, 1, 1, 1, 0, -1, -1, -1}; // neighbours
	int ny[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

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
						if(me->button == SDL_BUTTON_LEFT) {
							//cout << "x: " << me->x << " y: " << me->y << endl;
							int cx = int(me->x/10);
							int cy = int(me->y/10);
							board[cx][cy] = 1;
							//cout << "mouseevent" << cx << cy << endl;
						}
						else if(me->button == SDL_BUTTON_RIGHT) {
							int cx = int(me->x/10);
							int cy = int(me->y/10);
							board[cx][cy] = 0;
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
						} else {
							newboard[i][j] = 0;
						}
					}
				}
				std::copy(&newboard[0][0], &newboard[0][0]+BOARD_SIZE_X*BOARD_SIZE_Y, &board[0][0]);
			}
		}

		for(int i = 0; i < BOARD_SIZE_X; i++) {
			for(int j = 0; j < BOARD_SIZE_Y; j++) {
				if(board[i][j] == 1) {
					_drawFillRect(filled, i*10, j*10, 10, 10);
				}
			}
		}

		SDL_RenderPresent(gRenderer); //Update screen
	}

	//Free resources and close SDL
	close();
	return 0;
}

bool init() {
	bool success = true;

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

	//Quit SDL subsystems
	SDL_Quit();
}

void _drawFillRect(SDL_Color &col, int x, int y, int width, int height) {
	SDL_Rect fillRect = {x, y, width, height};
	SDL_SetRenderDrawColor(gRenderer, col.r, col.g, col.b, col.a);
	SDL_RenderFillRect(gRenderer, &fillRect);
}

/*
int **multArray = new int*[y_max];
for (int i = 0; i < y_max; i++) {
  multArray[i] = new int[x_max];
}
Matrix in C++???!?!?
*/
