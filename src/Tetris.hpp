#pragma once

#include "Color.hpp"
#include "GridWithOffset.hpp"
#include "Pieces.hpp"
#include "sdl-related.hpp"

#include <atomic>
#include <iostream>
#include <vector>

extern std::atomic<int> keyPresses;
extern std::atomic<bool> quitting;

Color getColor(char block);

void drawBlocks(SDL_Surface* screen, GridWithOffset& grid);

bool isPieceColliding(GridWithOffset const & background, GridWithOffset const & piece);

bool isPieceOutsideBackground(GridWithOffset const & background, GridWithOffset const & piece);

std::vector<int> getCompletedLines(GridWithOffset const & background);

void copyToBackground(GridWithOffset& background, GridWithOffset const & grid);

int countVoidsNotFillableFromAbove(GridWithOffset const & background);

struct Wanted {
	int rotation;
	int xOffset;
	int voidPenalty;
	int heightPenalty;
	bool isOtherBetter(Wanted const & other);
};

std::ostream & operator<<(std::ostream& out, Wanted const & w);

Wanted getWanted(GridWithOffset const & background, std::vector<GridWithOffset> piece);


void tetris();
