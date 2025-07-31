#pragma once
#include <Resources\IResource.hpp>
#include <Math\Vector2.hpp>
class TextureResource : public IResource 
{
private:
	unsigned int id;
	std::string path;

	Math::Vector2 size;

public:
	TextureResource() = default;

	Math::Vector2 GetSize() const { return size; };

	bool Load(const std::string& path) override;

	unsigned int GetID() const { return id; }
	std::string GetPath() const { return path; }

	bool IsValid() const { return id >= 0; }

	void Release() override;

	std::string GetType() const override { return  "Texture"; }
};