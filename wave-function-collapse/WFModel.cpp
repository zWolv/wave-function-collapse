#include "WFModel.h"


WFModel::WFModel()
= default;

WFModel::WFModel(int width, int height, const std::map<Tile, float>& weights, const std::map<Tile, std::map<Dir, std::vector<std::string>>>& tilemap)
{
	this->width = width;
	this->height = height;
	wavefunction = Wavefunction(width, height, weights);
	this->tilemap = tilemap;
	for(int i = 0; i < width * height; ++i)
	{
		grid.emplace_back(TileType::EMPTY);
	}
}

std::vector<Tile> WFModel::Iterate()
{
	
	const int index = GetLowestEntropyIndex();
	if (index == 7) 
		auto x = 5;
	grid[index] = wavefunction.Collapse(index);
	Propagate(index);
	int x = wavefunction.EntropyAt(index);

	return grid;
}

bool WFModel::FullyCollapsed() const
{
	return wavefunction.Collapsed();
}

std::vector<Tile> WFModel::FinishedGrid() const
{
	return wavefunction.GetAllCollapsed();
}

void WFModel::Propagate(int index)
{
	std::stack<int> stack;
	stack.push(index);
	while(!stack.empty())
	{
		const int current = stack.top();
		stack.pop();
		std::vector<Tile> possibleTiles = wavefunction.GetPossibleTilesAt(current);
		std::vector<Dir> dirs = ValidNeighbours(current);
		for(const Dir dir : dirs)
		{
			const int otherIndex = GetNeighbour(current, dir);
			const std::vector<Tile> otherTiles = wavefunction.GetPossibleTilesAt(otherIndex);
			for(const Tile otherTile : otherTiles)
			{
				bool tilePossible = false;
				for(const Tile tile : possibleTiles)
				{
					tilePossible |= Check(tile, otherTile, dir);
				}

				if (!tilePossible && otherTiles.size() != 1)
				{
					wavefunction.RemoveTileAt(otherIndex, otherTile);
					stack.push(otherIndex);
					const std::pair pos = Util::ToPos(otherIndex, width, height);
					const int x = pos.first;
					const int y = pos.second;
					std::cout << "Removed: " << otherTile << " from " << x << ", " << y << std::endl;
				}
			}
		}
	}
}

int WFModel::GetLowestEntropyIndex() const
{
	const float* minEntropy = nullptr;
	int index = 0;
	for(int i = 0; (size_t)i < grid.size(); ++i)
	{
		if (wavefunction.GetPossibleTilesAt(i).size() == 1) continue;

		const float entropy = wavefunction.EntropyAt(i);
		float noisedEntropy = entropy - Util::RandomFloat(0, 1) / 1000;
		if(!minEntropy or noisedEntropy < *minEntropy)
		{
			minEntropy = &noisedEntropy;
			index = i;
		}
	}

	return index;
}

std::vector<Dir> WFModel::ValidNeighbours(int index) const
{
	std::vector<Dir> result;
	const int right = Util::Right(index, width);
	const int left = Util::Left(index, width);
	const int top = Util::Top(index, width);
	const int bottom = Util::Bottom(index, width);
	if (Util::IsOnGrid(right, grid) && wavefunction.GetPossibleTilesAt(right).size() != 1) result.emplace_back(RIGHT);
	if (Util::IsOnGrid(left, grid) && wavefunction.GetPossibleTilesAt(left).size() != 1) result.emplace_back(LEFT);
	if (Util::IsOnGrid(top, grid) && wavefunction.GetPossibleTilesAt(top).size() != 1) result.emplace_back(UP);
	if (Util::IsOnGrid(bottom, grid) && wavefunction.GetPossibleTilesAt(bottom).size() != 1) result.emplace_back(DOWN);
	return result;
}

int WFModel::GetNeighbour(int index, Dir direction) const
{
	switch(direction)
	{
	case UP:
		if (Util::IsOnGrid(Util::Top(index, width), grid)) return Util::Top(index, width);
		break;
	case DOWN:
		if (Util::IsOnGrid(Util::Bottom(index, width), grid)) return Util::Bottom(index, width);
		break;
	case RIGHT:
		if (Util::IsOnGrid(Util::Right(index, width), grid)) return Util::Right(index, width);
		break;
	case LEFT:
		if (Util::IsOnGrid(Util::Left(index, width), grid)) return Util::Left(index, width);
		break;
	}
	
	return -1;
}

bool WFModel::Check(Tile tile1, Tile tile2, Dir dir, int rotation1, int rotation2) const
{
	//if (tilemap.at(tile1).at(dir) == tilemap.at(tile2).at(OppositeDirection(dir))) return true;
	switch(tile1.type)
	{
	case TileType::BEACH:
		if (tile2 == TileType::WATER or tile2 == TileType::BEACH or tile2 == TileType::GRASS) return true;
		break;
	case TileType::GRASS:
		if (tile2 == TileType::GRASS or tile2 == TileType::BEACH) return true;
		break;
	case TileType::WATER:
		if (tile2 == TileType::WATER or tile2 == TileType::BEACH) return true;
		break;
	default:
		break;
	}
	return false;
}


Dir WFModel::OppositeDirection(const Dir dir) const
{
	switch(dir)
	{
	case UP:
		return DOWN;
	case DOWN:
		return UP;
	case RIGHT:
		return LEFT;
	case LEFT:
		return RIGHT;
	}

	return UP;
}
