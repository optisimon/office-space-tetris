#pragma once

#include <iostream>
#include <vector>


enum {
	EMPTY=0
};


class GridWithOffset
{
public:
	GridWithOffset(int xoffset, int yoffset, int wgrid, int hgrid);

	char & operator()(int x, int y);

	char const & operator()(int x, int y) const;

	bool isInside(int x, int y) const;

	int getWidth() const { return wGrid; }
	int getHeight() const { return hGrid; }
	int getOffsetX() const { return xOffset; }
	int getOffsetY() const { return yOffset; }

	void setOffset(int x, int y);

private:
	int wGrid;
	int hGrid;
	int xOffset;
	int yOffset;
	std::vector<char> grid;
};

std::ostream & operator<<(std::ostream & out, GridWithOffset const & grid);

std::ostream & operator<<(std::ostream & out, std::vector<GridWithOffset> const & grids);
