#pragma once
#ifndef TEXTURE_HOLDER_H
#define TEXTURE_HOLDER_H

#include "SFML/Graphics.hpp"
#include <map>

using namespace std;
using namespace sf;

class TextureHolder
{
private:
	map<string, Texture> m_Textures;				// map of string-texture pairs
	static TextureHolder* m_s_Instance;				// pointer to itself

public:
	TextureHolder();
	static Texture& GetTexture(string const& filename);

};
#endif