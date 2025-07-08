#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <Services\Resources\Graphics\TextureResource.hpp>

bool TextureResource::Load(const std::string& path)
{
	int x, y, comp;
	stbi_set_flip_vertically_on_load(true); // Äëÿ OpenGL
	auto* image = stbi_load(path.c_str(), &x, &y, &comp, STBI_rgb_alpha);
	if (image == nullptr) return false;

	size = Math::Vector2(x, y);

	this->path = path;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image);

	return true;
}

void TextureResource::Release()
{
}
