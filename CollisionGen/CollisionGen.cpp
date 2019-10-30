#include "CollisionGen.h"
#include "json/json.h"
#include "cimg/CImg.h"
#include <fstream>

using namespace cimg_library;

constexpr char* FILE_NAME = "colliders.bmp";
constexpr int EMPTY_TILE = 0;
constexpr int TILE_SIZE = 32;

struct Collider
{
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
};

std::vector<Collider> colliders;
std::vector<std::vector<bool>> visitedMap;
std::vector<std::vector<bool>> bitmask;

void buildCollider(int _startY, int _startX)
{
	int y = _startY;
	int x = _startX;

	Collider newCollider{ x, y, 0, 0 };
	while(true)
	{
		if(x < bitmask[0].size() && bitmask[y][x] && !visitedMap[y][x])
		{
			visitedMap[y][x] = true;
			++newCollider.width;
			++x;
		}
		else
		{
			++newCollider.height;
			break;
		}
	}

	x = _startX;
	int width = 0;
	while (true)
	{
		++y;
		if(y >= bitmask.size())
		{
			break;
		}
		
		while(x < bitmask[0].size() && bitmask[y][x] && !visitedMap[y][x])
		{
			++x;
		}

		width = x - _startX;
		
		if(width == newCollider.width)
		{
			++newCollider.height;
			for(auto i = _startX; i < x; ++i)
			{
				visitedMap[y][i] = true;
			}
			x = _startX;
			width = 0;
		}
		else
		{
			break;
		}
	}

	colliders.push_back(newCollider);
}

void drawColliders(){
	CImg<unsigned char> image(bitmask[0].size() * TILE_SIZE, bitmask.size() * TILE_SIZE, 1, 3, 0);
	
	for(auto&& collider : colliders)
	{
		unsigned char randomColor[3];
		randomColor[0] = rand() % 256;
		randomColor[1] = rand() % 256;
		randomColor[2] = rand() % 256;

		
		for(auto x = 0; x < collider.width * TILE_SIZE; ++x)
		{
			for (auto y = 0; y < collider.height * TILE_SIZE; ++y)
			{
				image.draw_point(x + (collider.x * TILE_SIZE), y + (collider.y * TILE_SIZE), randomColor);
			}
		}
	}

	auto result = image.save(FILE_NAME);
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		std::cout << "Map file is required, drag and drop desired map file into .exe";
		auto c = std::getchar();
		return EXIT_FAILURE;
	}

	const auto fileName = argv[1];

	Json::Value root;
	std::ifstream in(fileName);
	in >> root;

	Json::Value terrainLayer;
	bool found = false;
	for (auto it = root["layers"].begin(); it != root["layers"].end(); ++it)
	{
		if ((*it)["name"] == "terrain" || (*it)["name"] == "Terrain")
		{
			terrainLayer = (*it);
			found = true;
			break;
		}
	}

	if(!found)
	{
		std::cout << "No layer named 'terrain' or 'Terrain' found, make sure it exists." << std::endl;
		auto c = std::getchar();
		return EXIT_FAILURE;
	}
	
	const auto data = terrainLayer["data"];
	const auto row = terrainLayer["height"].asInt();
	const auto col = terrainLayer["width"].asInt();

	for(auto y = 0; y < row; ++y)
	{
		visitedMap.emplace_back();
		bitmask.emplace_back();
		for (auto x = 0; x < col; ++x)
		{
			const auto val = data[(y * col) + x].asInt() != EMPTY_TILE;
			bitmask[y].push_back(val);
			visitedMap[y].push_back(false);
		}
	}

	std::cout << "Loaded terrain with width of " << col << " and height of " << row << " tiles." << std::endl;

	for (auto y = 0; y < row; ++y)
	{
		for (auto x = 0; x < col; ++x)
		{
			if (bitmask[y][x] && !visitedMap[y][x])
			{
				buildCollider(y, x);
			}
		}
	}
	
	std::cout << "Converted terrain to " << colliders.size() << " colliders." << std::endl;
	std::cout << "Writing example image." << std::endl;

	drawColliders();

	auto c = std::getchar();
	
	return EXIT_SUCCESS;
}
