#include "TextureHolder.h"
#include <assert.h>

TextureHolder* TextureHolder::m_s_Instance = nullptr;
TextureHolder::TextureHolder()
{
	assert(m_s_Instance == nullptr);				// ends game if its not
	m_s_Instance = this;
}

Texture& TextureHolder::GetTexture(string const& filename)
{
	auto& m = m_s_Instance->m_Textures;				// create map // auto = map<string, Texture>
	auto keyValuePair = m.find(filename);			// iterator for holdind pairs // auto = map<string, Texture>::iterator
	if (keyValuePair != m.end())
	{
		return keyValuePair->second;				// return pair if find
	}
	else
	{
		auto& texture = m[filename];				// create a new pair
		texture.loadFromFile(filename);				// load texture
		return texture;
	}
}