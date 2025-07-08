#include <Services\Resources\Graphics\ShaderResource.hpp>
#include <Services\Resources\Graphics\GLShader.hpp>
#include <Services/Resources/ResourceManager.hpp>
#include <Services/Resources/TextResource.hpp>
#include <Core\ServiceLocator.hpp>
#include <Services\Utility\Functions.hpp>


bool ShaderResource::Load(const std::string& path)
{

	auto text = ServiceLocator::Get<ResourceManager>()->Load<TextResource>(path)->GetContent();

	auto lines = SplitToLines(text);

	std::string name;
	std::string language;
	bool vertex_mode = false;
	bool fragment_mode = false;

	std::string vsrc;
	std::string fsrc;

	for (auto& line : lines) 
	{
		if (vertex_mode) 
		{
			if (line == "END")
			{
				vertex_mode = false;
				if (vsrc.size() != 0) vsrc.pop_back();
				continue;
			}
			vsrc += line + "\n";
		}
		if (fragment_mode)
		{
			if (line == "END")
			{
				fragment_mode = false;
				if (fsrc.size() != 0) fsrc.pop_back();
				continue;
			}
			fsrc += line + "\n";
		}

		auto separated_line = SplitString(line, ' ', 1);

		if (separated_line.size() == 0) continue;

		if (separated_line.size() == 1) 
		{
			if (separated_line[0] == "VERTEX") 
			{
				vertex_mode = true;
				continue;
			}
			
			if (separated_line[0] == "FRAGMENT")
			{
				fragment_mode = true;
				continue;
			}
		}
		
		if (separated_line.size() == 2) 
		{
			if (separated_line[0] == "NAME") name = separated_line[1];
			if (separated_line[0] == "LANGUAGE") language = separated_line[1];
		}
	}

	if (language == "GLSL")
	{
		auto shd = std::make_shared<GLShader>();
		shd->LoadFromSource(vsrc, fsrc);
		shd->SetName(name);
		content = shd;
		content->m_path = path;
	}
	else 
	{
		return false;
	}
	return true;
}

void ShaderResource::Release()
{
	content->Release();
}
