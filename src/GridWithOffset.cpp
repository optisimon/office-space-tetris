#include "GridWithOffset.hpp"


GridWithOffset::GridWithOffset(int xoffset, int yoffset, int wgrid, int hgrid) :
wGrid(wgrid),
hGrid(hgrid),
xOffset(xoffset),
yOffset(yoffset),
grid(wgrid*hgrid, EMPTY)
{}

char & GridWithOffset::operator()(int x, int y)
{
	return grid[y * wGrid + x];
}

char const & GridWithOffset::operator()(int x, int y) const
{
	return grid[y * wGrid + x];
}

bool GridWithOffset::isInside(int x, int y) const
{
	bool insideX = (x + xOffset) >= 0 && x < (xOffset + wGrid);
	bool insideY = (y + yOffset) >= 0 && y < (yOffset + hGrid);
	return insideX && insideY;
}

void GridWithOffset::setOffset(int x, int y)
{
	xOffset = x; yOffset = y;
}


std::ostream & operator<<(std::ostream & out, GridWithOffset const & grid)
{
	out << "xOffset=" << grid.getOffsetX() << ", yOffset=" << grid.getOffsetY() << "\n";
	for (int y = 0; y < grid.getHeight(); y++)
	{
		for (int x = 0; x < grid.getWidth(); x++)
		{
			out << char(('0' +  grid(x, y)));
		}
		out << "\n";
	}
	return out;
}

std::ostream & operator<<(std::ostream & out, std::vector<GridWithOffset> const & grids)
{
	out << "====[ Array of grids ]====\n";
	for (auto & grid : grids)
	{
		out << grid << "\n";
	}
	out << "==========================\n";
	return out;
}
