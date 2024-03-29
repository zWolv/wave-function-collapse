#include "SpriteManager.h"

SpriteManager::SpriteManager()
= default;

Texture2D SpriteManager::LookupTexture(std::string textureName) const
{
	return textureLookup.at(textureName);
}

void SpriteManager::LoadSprites()
{
	std::cout << "Loading images" << std::endl;
	std::filesystem::path current = std::filesystem::current_path().parent_path();
	const std::filesystem::path path = current.append("Resources");
	auto dirIt = std::filesystem::directory_iterator(path);
	int files = std::count_if(begin(dirIt), end(dirIt), [](auto& entry) { return is_regular_file(entry.path()); } );
	std::cout << files << " images." << std::endl;
	for(const auto& entry : std::filesystem::directory_iterator(path))
	{
		std::string str = entry.path().string();
		std::string strP = str;
		const char* const p = strP.c_str();
		std::reverse(str.begin(), str.end());
		const size_t pos = str.find('\\');
		const std::string::iterator it = str.begin() + (int)pos;
		str.erase(it, str.end());
		str.erase(str.begin(), str.begin() + 4);
		std::reverse(str.begin(), str.end());
		
		Image image = LoadImage(p);
		Texture2D tex = LoadTextureFromImage(image);
		Color* colors = LoadImageColors(image);
		std::cout << "TESTING: " << &colors << std::endl;
		

		for(int i = 0; i < image.width; ++i)
		{
			colorMap[str][UP].emplace_back(ColorToInt(colors[i]));
		}

		for(int i = 0; i < image.width; ++i)
		{
			colorMap[str][DOWN].emplace_back(ColorToInt(colors[(image.height - 1) * image.width + i]));
		}

		for(int i = 0; i < image.height; ++i)
		{
			colorMap[str][LEFT].emplace_back(ColorToInt(colors[(ptrdiff_t)i * image.width]));
		}

		for(int i = 0; i < image.height; ++i)
		{
			colorMap[str][RIGHT].emplace_back(ColorToInt(colors[i * image.width + image.width - 1]));
		}

		UnloadImageColors(colors);
		textureLookup[str] = tex;
		images.emplace_back(str);
		ImageRotateCW(&image);
		tex = LoadTextureFromImage(image);
		textureLookup[str + "R90"] = tex;
		images.emplace_back(str + "R90");

		ImageRotateCW(&image);
		tex = LoadTextureFromImage(image);
		textureLookup[str + "R180"] = tex;
		images.emplace_back(str + "R180");

		ImageRotateCW(&image);
		tex = LoadTextureFromImage(image);
		textureLookup[str + "R270"] = tex;
		images.emplace_back(str + "R270");

		UnloadImage(image);
	}

	std::cout << "Finished loading images" << std::endl;
}

bool SpriteManager::CompareColors(std::string first, std::string second, Dir dir) const
{
	int firstDir = dir;
	int secondDir = Util::OppositeDirection(dir);
	size_t fR = first.find("R");
	size_t sR = second.find("R");
	if (fR != std::string::npos)
	{
		firstDir += std::stoi(first.substr(fR + 1, 3)) / 90;
		firstDir %= 4;
		first.erase(first.begin() + (int)fR, first.end());
	}
	if (sR != std::string::npos)
	{
		secondDir += std::stoi(second.substr(sR + 1, 3)) / 90;
		secondDir %= 4;
		second.erase(second.begin() + (int)sR, second.end());
	}
	
	return colorMap.at(first).at((Dir)firstDir) == colorMap.at(second).at((Dir)secondDir);
}
