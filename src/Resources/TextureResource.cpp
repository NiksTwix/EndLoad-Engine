#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <Resources\TextureResource.hpp>
#include <Core\ServiceLocator.hpp>
#include <Systems\Render\Render.hpp>


bool TextureResource::Load(const std::string& path)
{
	int x, y, comp;
	stbi_set_flip_vertically_on_load(true); // Äëÿ OpenGL
	auto* image = stbi_load(path.c_str(), &x, &y, &comp, STBI_rgb_alpha);
	if (image == nullptr) return false;

	size = Math::Vector2(x, y);

	this->path = path;

	id = ServiceLocator::Get<Render>()->GetContext()->CreateTexture(image, size,4);

	stbi_image_free(image);

	return true;
}

void TextureResource::Release()
{
}
