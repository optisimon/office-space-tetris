#include "Pieces.hpp"

std::vector<GridWithOffset> getSquare()
{
	std::vector<GridWithOffset> squares;
	squares.emplace_back(0, 0, 2, 2);
	squares.back()(0, 0) = 1;
	squares.back()(1, 0) = 1;
	squares.back()(0, 1) = 1;
	squares.back()(1, 1) = 1;
	return squares;
}

std::vector<GridWithOffset> getRod()
{
	std::vector<GridWithOffset> shapes;

	shapes.emplace_back(1, 1, 4, 4);
	shapes.back()(0, 1) = 2;
	shapes.back()(1, 1) = 2;
	shapes.back()(2, 1) = 2;
	shapes.back()(3, 1) = 2;

	shapes.emplace_back(1, 1, 4, 4);
	shapes.back()(1, 0) = 2;
	shapes.back()(1, 1) = 2;
	shapes.back()(1, 2) = 2;
	shapes.back()(1, 3) = 2;
	return shapes;
}

std::vector<GridWithOffset> getT()
{
	std::vector<GridWithOffset> shapes;

	shapes.emplace_back(1, 1, 3, 3);
	shapes.back()(1, 0) = 3;
	shapes.back()(0, 1) = 3;
	shapes.back()(1, 1) = 3;
	shapes.back()(2, 1) = 3;

	shapes.emplace_back(1, 1, 3, 3);
	shapes.back()(1, 0) = 3;
	shapes.back()(0, 1) = 3;
	shapes.back()(1, 1) = 3;
	shapes.back()(1, 2) = 3;

	shapes.emplace_back(1, 1, 3, 3);
	shapes.back()(0, 1) = 3;
	shapes.back()(1, 1) = 3;
	shapes.back()(2, 1) = 3;
	shapes.back()(1, 2) = 3;

	shapes.emplace_back(1, 1, 3, 3);
	shapes.back()(1, 0) = 3;
	shapes.back()(1, 1) = 3;
	shapes.back()(2, 1) = 3;
	shapes.back()(1, 2) = 3;
	return shapes;
}

std::vector<GridWithOffset> getN()
{
	std::vector<GridWithOffset> shapes;

	shapes.emplace_back(1, 1, 3, 3);
	shapes.back()(0, 0) = 4;
	shapes.back()(0, 1) = 4;
	shapes.back()(1, 1) = 4;
	shapes.back()(1, 2) = 4;

	shapes.emplace_back(1, 1, 3, 3);
	shapes.back()(1, 0) = 4;
	shapes.back()(2, 0) = 4;
	shapes.back()(0, 1) = 4;
	shapes.back()(1, 1) = 4;
	return shapes;
}

std::vector<GridWithOffset> getZ()
{
	std::vector<GridWithOffset> shapes;

	shapes.emplace_back(1, 1, 3, 3);
	shapes.back()(1, 0) = 5;
	shapes.back()(0, 1) = 5;
	shapes.back()(1, 1) = 5;
	shapes.back()(0, 2) = 5;

	shapes.emplace_back(1, 1, 3, 3);
	shapes.back()(0, 0) = 5;
	shapes.back()(1, 0) = 5;
	shapes.back()(1, 1) = 5;
	shapes.back()(2, 1) = 5;
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
