<img src="/img/banner.gif">
Conway's Game of Life is a cellular automaton, played on a 2D grid of cells. Each of the cells can be 'dead' or 'alive' based on that cell's 8 neighbors.

Installation
---------------------
Before you get this, you need
+ C++ compiler of your choice
+ SDL2
+ SDL2 TFF

If you need help with setting up SDL2, you can check out this [excellent tutorial](https://lazyfoo.net/tutorials/SDL/).

In order to get it running, first clone the repository
 ```
 git clone https://github.com/hadber/game-of-life.git
 ```

 and then compile it with

 ```
 g++ gol.cpp -w -lSDL2 -lSDL2_ttf -o gol
 ```

Usage
---------------------
You can left-click to set a cell as 'alive' or right-click to set it as 'dead'. The game starts as paused. To unpause (or pause later), press `space` on your keyboard.

Demo
---------------------
<img src="/img/demo.gif">
