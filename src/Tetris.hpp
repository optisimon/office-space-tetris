
#include "sdl-related.hpp"

#include <tuple>
#include <vector>

int const wBox = 32;
int const hBox = 32;

int const wGrid = 13;
int const hGrid = 20;

enum {
	EMPTY=0
};


char grid[wGrid*hGrid] = {};

struct Color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

Color getColor(SDL_Surface* screen, char block)
{
	if (block == 0)
	{
		return {0, 0, 0};
	}
	Color c = {uint8_t(block*25 + 70), uint8_t(block*25 + 70), uint8_t(block*25 + 70)};
	return c;
}

struct Grid {
	int wGrid;
	int hGrid;
	std::vector<char> grid;
	Grid(int wgrid, int hgrid) : wGrid(wgrid), hGrid(hgrid), grid(wgrid*hgrid, EMPTY)
	{}
};

struct GridWithOffset : public Grid {
	int xOffset;
	int yOffset;
	GridWithOffset(int xoffset, int yoffset, int wGrid, int hGrid) : Grid(wGrid, hGrid), xOffset(xoffset), yOffset(yoffset)
	{}

	char & operator()(int y, int x)
	{
			return grid[y*wGrid + x];
	}

	char const & operator()(int y, int x) const
	{
			return grid[y*wGrid + x];
	}

	bool isInside(int y, int x) const
	{
		 bool insideY = (y + yOffset) >= 0 && y < (yOffset + hGrid);
		 bool insideX = (x + xOffset) >= 0 && x < (xOffset + wGrid);
		 return insideX && insideY;
	}
};



void drawBlocks(SDL_Surface* screen, GridWithOffset& grid)
{
	for (int y = 0; y < grid.hGrid; y++)
	{
		for (int x = 0; x < grid.wGrid; x++)
		{
			char block = grid(y, x);
			if (block == 0)
			{
				continue;
			}

			SDL_Rect r;
			r.w = wBox;
			r.h = hBox;
			r.x = (grid.xOffset + x)*wBox;
			r.y = (grid.yOffset + y)*hBox;

			Color color_raw = getColor(screen, block);
			int R = color_raw.r;
			int G = color_raw.g;
			int B = color_raw.b;

			uint32_t color = SDL_MapRGB(screen->format, R, G, B);
			uint32_t color_lighter = SDL_MapRGB(screen->format, int(R*255.0/205), int(G*255.0/205), int(B*255.0/205));
			uint32_t color_darker = SDL_MapRGB(screen->format, int(R*165.0/205), int(G*165.0/205), int(B*165.0/205));
			SDL_FillRect(screen, &r, color);

			r.h = 5;
			SDL_FillRect(screen, &r, color_lighter);
			r.y += hBox - 5;
			SDL_FillRect(screen, &r, color_darker);
			r.h = hBox;
			r.w = 5;
			r.y = (grid.yOffset + y)*hBox;
			SDL_FillRect(screen, &r, color_darker);
			r.x += wBox - 5;
			SDL_FillRect(screen, &r, color_lighter);
		}
	}
}

bool isPieceColliding(GridWithOffset const & background, GridWithOffset const & piece)
{
	assert(background.yOffset == 0);
	for (int y = 0; y < piece.hGrid; y++)
	{
		for (int x = 0; x < piece.wGrid; x++)
		{
			if (piece(y, x) != EMPTY)
			{
				int i = x + piece.xOffset;
				int j = y + piece.yOffset;

				if (background.isInside(j, i) && background(j, i) != EMPTY)
				{
					return true;
				}
				if (j >= background.hGrid)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool isPieceOutsideBackground(GridWithOffset const & background, GridWithOffset const & piece)
{
	for (int y = 0; y < piece.hGrid; y++)
	{
		for (int x = 0; x < piece.wGrid; x++)
		{
			if (piece(y, x) != EMPTY)
			{
				int i = x + piece.xOffset;
				int j = y + piece.yOffset;

				if (j < 0) { j = 0; } // allow piece above playing field

				if (!background.isInside(j, i))
				{
					return true;
				}
			}
		}
	}
	return false;
}

std::vector<int> getCompletedLines(GridWithOffset const & background)
{
	assert(background.xOffset == 0);
	assert(background.yOffset == 0);
	std::vector<int> completed;
	for (int y = 0; y < background.hGrid; y++)
	{
		bool isFinished = true;
		for (int x = 0; x < background.wGrid; x++)
		{
			if(background(y, x) == EMPTY)
			{
				isFinished = false;
				break;
			}
		}
		if (isFinished) {
			completed.push_back(y);
		}
	}
	return completed;
}

void copyToBackground(GridWithOffset& background, GridWithOffset const & grid)
{
	for (int y = 0; y < grid.hGrid; y++)
	{
		for (int x = 0; x < grid.wGrid; x++)
		{
			if (grid(y, x) != EMPTY)
			{
				int i = x + grid.xOffset;
				int j = y + grid.yOffset;
				if (background.isInside(j,i))
				{
					background(j, i) = grid(y,x);
				}
			}
		}
	}
}


std::vector<GridWithOffset> getSquare()
{
	std::vector<GridWithOffset> squares;
	squares.emplace_back(0, 0, 2, 2);
	squares.back()(0,0) = 1;
	squares.back()(0,1) = 1;
	squares.back()(1,0) = 1;
	squares.back()(1,1) = 1;
	return squares;
}

std::vector<GridWithOffset> getRod()
{
	std::vector<GridWithOffset> shapes;

	shapes.emplace_back(1, 1, 4, 4);
	shapes.back()(1,0) = 2;
	shapes.back()(1,1) = 2;
	shapes.back()(1,2) = 2;
	shapes.back()(1,3) = 2;

	shapes.emplace_back(1, 1, 4, 4);
	shapes.back()(0,1) = 2;
	shapes.back()(1,1) = 2;
	shapes.back()(2,1) = 2;
	shapes.back()(3,1) = 2;
	return shapes;
}

std::vector<GridWithOffset> getT()
{
	std::vector<GridWithOffset> shapes;

	shapes.emplace_back(1, 1, 3, 3);
	shapes.back()(0,1) = 3;
	shapes.back()(1,0) = 3;
	shapes.back()(1,1) = 3;
	shapes.back()(1,2) = 3;

	shapes.emplace_back(1, 1, 3, 3);
	shapes.back()(0,1) = 3;
	shapes.back()(1,0) = 3;
	shapes.back()(1,1) = 3;
	shapes.back()(2,1) = 3;

	shapes.emplace_back(1, 1, 3, 3);
	shapes.back()(1,0) = 3;
	shapes.back()(1,1) = 3;
	shapes.back()(1,2) = 3;
	shapes.back()(2,1) = 3;

	shapes.emplace_back(1, 1, 3, 3);
	shapes.back()(0,1) = 3;
	shapes.back()(1,1) = 3;
	shapes.back()(1,2) = 3;
	shapes.back()(2,1) = 3;
	return shapes;
}

std::vector<GridWithOffset> getN()
{
	std::vector<GridWithOffset> shapes;

	shapes.emplace_back(1, 1, 3, 3);
	shapes.back()(0,0) = 4;
	shapes.back()(1,0) = 4;
	shapes.back()(1,1) = 4;
	shapes.back()(2,1) = 4;

	shapes.emplace_back(1, 1, 3, 3);
	shapes.back()(0,1) = 4;
	shapes.back()(0,2) = 4;
	shapes.back()(1,0) = 4;
	shapes.back()(1,1) = 4;
	return shapes;
}

std::vector<GridWithOffset> getZ()
{
	std::vector<GridWithOffset> shapes;

	shapes.emplace_back(1, 1, 3, 3);
	shapes.back()(0,1) = 5;
	shapes.back()(1,0) = 5;
	shapes.back()(1,1) = 5;
	shapes.back()(2,0) = 5;

	shapes.emplace_back(1, 1, 3, 3);
	shapes.back()(0,0) = 5;
	shapes.back()(0,1) = 5;
	shapes.back()(1,1) = 5;
	shapes.back()(1,2) = 5;
	return shapes;
}

std::vector<GridWithOffset> getRandomPiece()
{
	switch(rand() % 5)
	{
	case 0:	return getSquare();
	case 1: return getRod();
	case 2: return getT();
	case 3: return getN();
	case 4: return getZ();

	default: return getT();
	}
}

struct Piece
{
	Piece(std::vector<GridWithOffset> & piece, int x, int y) : _x(x), _y(y), _rotation(0), _piece(piece)
	{ }

	GridWithOffset getPose()
	{
		_piece[_rotation].xOffset = _x;
		_piece[_rotation].yOffset = _y;
		return _piece[_rotation];
	}

	int getNumOrientations() const { return _piece.size(); }
	int getRotation() const { return _rotation; }
	int getX() const { return _x; }
	int getY() const { return _y; }

	/** @return false if new position can't be set */
	bool setPosition(GridWithOffset const & background, int x, int y)
	{
		GridWithOffset tmp = _piece[_rotation];
		tmp.xOffset = x;
		tmp.yOffset = y;
		if (isPieceOutsideBackground(background, tmp))
		{
			return false;
		}
		if (isPieceColliding(background, tmp))
		{
			return false;
		}
		_x = x;
		_y = y;
		return true;
	}

	/** @return false if new rotation can't be set */
	bool setRotation(GridWithOffset const & background, int rotation)
	{
		GridWithOffset tmp = _piece[rotation];
		tmp.xOffset = _x;
		tmp.yOffset = _y;
		if (isPieceOutsideBackground(background, tmp))
		{
			return false;
		}
		if (isPieceColliding(background, tmp))
		{
			return false;
		}
		_rotation = rotation;
		return true;
	}

private:
	int _x;
	int _y;
	int _rotation;
	std::vector<GridWithOffset> & _piece;
};

int countVoidsNotFillableFromAbove(GridWithOffset const & background)
{
	int numVoids = 0;
	for (int x = 0; x < background.wGrid; x++)
	{
		bool haveHitFirstBlock = false;
		for (int y = 0; y < background.hGrid; y++)
		{
			if (haveHitFirstBlock && background(y, x) == EMPTY)
			{
				numVoids++;
			}
			if (background(y, x) != EMPTY)
			{
				haveHitFirstBlock = true;
			}
		}
	}
	return numVoids;
}

struct Wanted {
	int rotation;
	int xOffset;
	int voidPenalty;
	int heightPenalty;
	bool isOtherBetter(Wanted const & other)
	{
		return std::tie(other.voidPenalty, other.heightPenalty, other.xOffset, other.rotation) <
				std::tie(voidPenalty, heightPenalty, xOffset, rotation);
	}
};
std::ostream & operator<<(std::ostream& out, Wanted const & w)
{
	out
	<< "{ \"voidPenalty\":" << w.voidPenalty
	<< ", \"heightPenalty\":" << w.heightPenalty
	<< ", \"xOffset\":" << w.xOffset
	<< ", \"rotation\":" << w.rotation << "}";
	return out;
}

Wanted getWanted(GridWithOffset const & background, std::vector<GridWithOffset> piece)
{
	// 1) Create as few voids as possible
	// 2) of those solutions, get the bit as low as possible
	int maxVoidPenalty = background.wGrid * background.hGrid;

	Wanted solution = {0, 0, maxVoidPenalty, background.hGrid};

	for (int x = -5; x < background.wGrid+5; x++)
	{
		for (int rotation = 0; rotation < piece.size(); rotation++)
		{
			Piece p(piece, x, 0);
			if (!p.setRotation(background, rotation)) { continue; }

			// See how far down it can go
			if (!p.setPosition(background, x, 0)) { continue; }

			for (int y = 0; y < background.hGrid; y++)
			{
				if (!p.setPosition(background, x, y))
				{
					GridWithOffset bgCopy = background;
					copyToBackground(bgCopy, p.getPose());

					// The previous height was the last working one
					Wanted tmp = {rotation, x, countVoidsNotFillableFromAbove(bgCopy), bgCopy.hGrid - 1 - p.getY()};
					if (solution.isOtherBetter(tmp)) {
						std::cout << "   solution: " << solution << " replaced by " << tmp << "\n";
						solution = tmp;
					}
				}
			}
		}
	}

	std::cout << "Winner: " << solution << "\n";
	return solution; // TODO: throw exception if not possible???
}


void tetris()
{
	int w = wGrid * wBox + 2;
	int h = hGrid * hBox + 2;
	SDL_Surface* screen = setup_sdl(w, h);
	Slock(screen);

	GridWithOffset background(0, 0, wGrid, hGrid);

	// Add random blocks
	for (int y = background.hGrid - 1; y < background.hGrid; y++)
	{
		for (int x = 0; x < background.wGrid; x++)
		{
			background(y, x) = rand() % 6;
		}
	}

	SDL_FillRect(screen, nullptr,SDL_MapRGB(screen->format, 0, 0, 0));
	drawBlocks(screen, background);
	Sulock(screen);

	std::vector<GridWithOffset> currentPiece = getRandomPiece();
	GridWithOffset lastPiece(0, 0, 1, 1);
	int rotation = 0;
	int y = 0;
	int x = (wGrid + 1) / 2;

 	bool autoPlay = true;
	enum State {
		IS_ROTATING,
		IS_MOVING_SIDEWAYS,
		IS_FALLING,
	} state = IS_ROTATING;
	int cntr = 0;
	while (true)
	{
		//keyPresses=1;
		int const numMicroSleeps = 48;
		for (int j = 0; j < numMicroSleeps; j++)
		{
			cntr++;
			usleep(5000);
			if (keyPresses) { break; }
			if (autoPlay && state == IS_FALLING) { break; }
		}

		Slock(screen);
		SDL_FillRect(screen, nullptr, SDL_MapRGB(screen->format, 0, 0, 0));

		// Get suggestion
		Wanted wanted;
		if (autoPlay && state != IS_FALLING)
		{
			wanted = getWanted(background, currentPiece);
		}

		if (cntr > numMicroSleeps || state == IS_FALLING)
		{
			y++;
			cntr = std::max<int>( 0, cntr - numMicroSleeps);
		}
		else
		{
			if (autoPlay)
			{
				if (keyPresses)
				{
					std::cout << "======================================\n";
					if (state == IS_ROTATING)
					{
						if (rotation != wanted.rotation)
						{
							int newRotation = (rotation + 1) % currentPiece.size();
							GridWithOffset & piece = currentPiece[newRotation];
							piece.xOffset = x;
							piece.yOffset = y;

							if (!isPieceColliding(background, piece))
							{
								rotation = newRotation;
							}
						}
						else
						{
							state = IS_MOVING_SIDEWAYS;
						}
					}
					if (state == IS_MOVING_SIDEWAYS)
					{
						if (x == wanted.xOffset)
						{
							state = IS_FALLING;
						}
						else
						{
							GridWithOffset & piece = currentPiece[rotation];
							piece.yOffset = y;
							if (x < wanted.xOffset) {
								piece.xOffset = x + 1;
							} else {
								piece.xOffset = x - 1;
							}
							if (!isPieceColliding(background, piece)) {
								x = piece.xOffset;
							}
						}
					}
					if (state == IS_FALLING)
					{
						//
					}
					keyPresses = 0;
				}
			}
			else
			{
				int newRotation = (rotation+1) % currentPiece.size();
				GridWithOffset & piece = currentPiece[newRotation];
				piece.xOffset = x;
				piece.yOffset = y;

				if (!isPieceColliding(background, piece))
				{
					rotation = newRotation;
				}
			}
		}
		GridWithOffset & piece = currentPiece[rotation];
		piece.xOffset = x;
		piece.yOffset = y;

		if (isPieceColliding(background, piece))
		{
			copyToBackground(background, lastPiece);
			currentPiece = getRandomPiece();
			y = 0;
			rotation = 0;
			lastPiece = currentPiece[rotation];
			if (isPieceColliding(background, lastPiece))
			{
				quitting = true;
				usleep(500000);
				return;
			}
			state = IS_ROTATING;
		}
		else
		{
			drawBlocks(screen, piece);
			lastPiece = piece;
		}

		drawBlocks(screen, background);
		Sulock(screen);
		SDL_Flip(screen);

		std::vector<int> completed = getCompletedLines(background);

		for (int const row : completed)
		{
			// move everything above row down one step, and put empty fields on top
			for (int x = 0; x < background.wGrid; x++)
			{
				for (int y = row; y > 0; y--)
				{
					background(y, x) = background(y-1, x);
				}
				background(0, x) = EMPTY;
			}
		}

		drawBlocks(screen, background);
		Sulock(screen);
		SDL_Flip(screen);


	}

	sleep(1);
}
