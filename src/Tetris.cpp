#include "Tetris.hpp"

#include <iostream>
#include <tuple>
#include <vector>

#include <assert.h>
#include <unistd.h>


int const wBox = 32;
int const hBox = 32;

static int const gs_wGrid = 13;
static int const gs_hGrid = 20;


Color getColor(char block)
{
	if (block == 0)
	{
		return {0, 0, 0};
	}
	Color c = {uint8_t(block*25 + 70), uint8_t(block*25 + 70), uint8_t(block*25 + 70)};
	return c;
}

void drawBlocks(SDL_Surface* screen, GridWithOffset& grid)
{
	for (int y = 0; y < grid.getHeight(); y++)
	{
		for (int x = 0; x < grid.getWidth(); x++)
		{
			char block = grid(x, y);
			if (block == 0)
			{
				continue;
			}

			SDL_Rect r;
			r.w = wBox;
			r.h = hBox;
			r.x = (grid.getOffsetX() + x)*wBox;
			r.y = (grid.getOffsetY() + y)*hBox;

			Color color_raw = getColor(block);
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
			r.y = (grid.getOffsetY() + y)*hBox;
			SDL_FillRect(screen, &r, color_darker);
			r.x += wBox - 5;
			SDL_FillRect(screen, &r, color_lighter);
		}
	}
}

bool isPieceColliding(GridWithOffset const & background, GridWithOffset const & piece)
{
	assert(background.getOffsetY() == 0);
	for (int y = 0; y < piece.getHeight(); y++)
	{
		for (int x = 0; x < piece.getWidth(); x++)
		{
			if (piece(x, y) != EMPTY)
			{
				int i = x + piece.getOffsetX();
				int j = y + piece.getOffsetY();

				if (background.isInside(i, j) && background(i, j) != EMPTY)
				{
					return true;
				}
				if (j >= background.getHeight())
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
	for (int y = 0; y < piece.getHeight(); y++)
	{
		for (int x = 0; x < piece.getWidth(); x++)
		{
			if (piece(x, y) != EMPTY)
			{
				int i = x + piece.getOffsetX();
				int j = y + piece.getOffsetY();

				if (j < 0) { j = 0; } // allow piece above playing field

				if (!background.isInside(i, j))
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
	assert(background.getOffsetX() == 0);
	assert(background.getOffsetY() == 0);
	std::vector<int> completed;
	for (int y = 0; y < background.getHeight(); y++)
	{
		bool isFinished = true;
		for (int x = 0; x < background.getWidth(); x++)
		{
			if (background(x, y) == EMPTY)
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
	for (int y = 0; y < grid.getHeight(); y++)
	{
		for (int x = 0; x < grid.getWidth(); x++)
		{
			if (grid(x, y) != EMPTY)
			{
				int i = x + grid.getOffsetX();
				int j = y + grid.getOffsetY();
				if (background.isInside(i, j))
				{
					background(i, j) = grid(x, y);
				}
			}
		}
	}
}

int countVoidsNotFillableFromAbove(GridWithOffset const & background)
{
	int numVoids = 0;
	for (int x = 0; x < background.getWidth(); x++)
	{
		bool haveHitFirstBlock = false;
		for (int y = 0; y < background.getHeight(); y++)
		{
			if (haveHitFirstBlock && background(x, y) == EMPTY)
			{
				numVoids++;
			}
			if (background(x, y) != EMPTY)
			{
				haveHitFirstBlock = true;
			}
		}
	}
	return numVoids;
}

bool Wanted::isOtherBetter(Wanted const & other)
{
	return std::tie(other.voidPenalty, other.heightPenalty, other.xOffset, other.rotation) <
			std::tie(voidPenalty, heightPenalty, xOffset, rotation);
}

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
//	std::cout
//	<< "\n========== getWanted(..) ==========\n"
//	<< "Piece:" << piece << "\n"
//	<< "Background:\n" << background << "\n";

	// 1) Create as few voids as possible
	// 2) of those solutions, get the bit as low as possible
	int maxVoidPenalty = background.getWidth() * background.getHeight();

	Wanted solution = {0, 0, maxVoidPenalty, background.getHeight()};

	for (int x = -5; x < background.getWidth() + 5; x++)
	{
		for (int rotation = 0; rotation < int(piece.size()); rotation++)
		{
			GridWithOffset tmp = piece[rotation];
			tmp.setOffset(x, 0);
			if (isPieceColliding(background, tmp))
			{
				continue;
			}

			for (int y = 0; y < background.getHeight(); y++) // TODO: start from 0
			{
				tmp.setOffset(x, y);
				if (isPieceColliding(background, tmp))
				{
					break;
				}
				if (isPieceOutsideBackground(background, tmp))
				{
					continue;
				}

				GridWithOffset bgCopy = background;
				copyToBackground(bgCopy, tmp);

				// The previous height was the last working one
				Wanted alternative = {rotation, x, countVoidsNotFillableFromAbove(bgCopy), bgCopy.getHeight() - 1 - tmp.getOffsetY()};
				if (solution.isOtherBetter(alternative))
				{
//					std::cout
//					<< "   solution: " << solution << " replaced by " << alternative << ":"
//					<< 		bgCopy << "\n\n";
					solution = alternative;
				}
			}
		}
	}

//	std::cout << "Winner: " << solution << "\n";
	return solution; // TODO: throw exception if not possible???
}

void tetris()
{
	int w = gs_wGrid * wBox + 2;
	int h = gs_hGrid * hBox + 2;
	SDL_Surface* screen = setup_sdl(w, h);
	Slock(screen);

	GridWithOffset background(0, 0, gs_wGrid, gs_hGrid);

	// Add random blocks
	for (int y = background.getHeight() - 1; y < background.getHeight(); y++)
	{
		for (int x = 0; x < background.getWidth(); x++)
		{
			background(x, y) = rand() % 6;
		}
	}

	SDL_FillRect(screen, nullptr,SDL_MapRGB(screen->format, 0, 0, 0));
	drawBlocks(screen, background);
	Sulock(screen);

	std::vector<GridWithOffset> currentPiece = getRandomPiece();
	GridWithOffset lastPiece(0, 0, 1, 1);
	int rotation = 0;
	int y = 0;
	int x = (gs_wGrid + 1) / 2;

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
		Wanted wanted = {};
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
							piece.setOffset(x, y);

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

							bool goRight = x < wanted.xOffset;
							piece.setOffset(goRight ? x + 1 : x - 1, y);

							if (!isPieceColliding(background, piece)) {
								x = piece.getOffsetX();
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
				piece.setOffset(x, y);

				if (!isPieceColliding(background, piece))
				{
					rotation = newRotation;
				}
			}
		}
		GridWithOffset & piece = currentPiece[rotation];
		piece.setOffset(x, y);

		if (isPieceColliding(background, piece))
		{
			copyToBackground(background, lastPiece);
			currentPiece = getRandomPiece();
			y = 0;
			x = (gs_wGrid + 1) / 2;
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
			for (int x = 0; x < background.getWidth(); x++)
			{
				for (int y = row; y > 0; y--)
				{
					background(x, y) = background(x, y - 1);
				}
				background(x, 0) = EMPTY;
			}
		}

		drawBlocks(screen, background);
		Sulock(screen);
		SDL_Flip(screen);


	}

	sleep(1);
}


