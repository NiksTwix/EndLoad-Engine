#include <sstream>
#include <fstream>
#include <Core\ServiceLocator.hpp>
#include <Services\Settings\SettingsManager.hpp>
#include <Core\Logger.hpp>
#include <Services\Scene\SceneLoader.hpp>
#include <Services\Serialization\SerializationService.hpp>
#include <Services\Resources\ResourceManager.hpp>
#include <Resources\TextResource.hpp>
#include <ECS\Serialization\SerializationManager.hpp>

void SceneLoader::SaveAsELScene(std::string path, SceneContext& scene) const
{
	auto setMan = ServiceLocator::Get<SettingsManager>();

	auto serser = ServiceLocator::Get<SerializationService>();

	if (!setMan || !serser) 
	{
		ServiceLocator::Get<Logger>()->Log("(Scene loading): scene saving error. Meta data is invalid.",Logger::Level::Error);
		return;
	}

	std::stringstream text;
	text << "### Header\n";		//Information about engine, meta data
	text << "ENGINE_NAME" << " " << setMan->GetEngineName() << "\n";
	text << "ENGINE_VERSION" << " " << std::to_string(setMan->GetEngineVersion()) << "\n";
	// TODO добавить профиль юзера или что-то подобное. Просто рассмотреть идею

	text << "\n";

	text << "### SceneData\n";	//Information about scene

	text << "SCENE_NAME" << " " << scene.GetName() << "\n";

	//Scene desc

	text << "SCENE_DESCRIPTION\n";
	text << "{\n";
	text << scene.Description << "\n";
	text << "}\n";

	text << "\n";

	text << "### Viewports\n";	//Information about scene

	for (auto* viewport : scene.GetViewportService().GetViewports()) 
	{
		text << "Viewport " << viewport->GetID() << "\n{\n";

		for (auto& [field, value] : scene.GetViewportService().GetViewportFields(viewport->GetID())) 
		{
			text << "\t" << field << ":" << ECS::ESDL::ValueParser::ToString(value) << "\n";
		}
		text << "}\n";
	}

	text << "### Entities\n";
	
	text << "{\n";;

	auto entities = ECS::SerializationManager::SerializeToLines(scene.GetEntitySpace());

	for (auto entity : entities) 
	{
		text << "\t" + entity + "\n";
	}
	text << "}\n";
	text << "\n";
	text << "### FreeEntities\n";

	text << "{\n";
	text << "";

	auto free_entities = scene.GetEntitySpace().GetFreeEntities();

	for (auto entity : free_entities)
	{
		text << std::to_string(entity) << " ";
	}

	text << "\n";
	text << "}\n";

	if (path == "") path = scene.GetName() + ".elscene";

	try 
	{
		std::ofstream out;          // поток для записи
		out.open(path);

		if (out.is_open())
		{
			out << text.str() << std::endl;
		}

		out.close();
	}
	catch (std::exception e) 
	{
		ServiceLocator::Get<Logger>()->Log(e.what(), Logger::Level::Error);
		return;
	}
	ServiceLocator::Get<Logger>()->Log("Saving scene is success.", Logger::Level::Success);
}

SceneContext* SceneLoader::LoadELScene(std::string path) const
{
	SceneContext* scene = nullptr;

	auto resource = ServiceLocator::Get<ResourceManager>()->Load<TextResource>(path);
	if (!resource) 
	{
		ServiceLocator::Get<Logger>()->Log("(Scene loading): loading scene is failed. File is incorrect or doesnt exist.", Logger::Level::Error);
		return nullptr;
	}
	std::string text = resource->GetContent();
	auto blocks = SeparateToBlocks(text);

	//Которые должны обрабатываться в первую очередь
	if (!HeaderHandler(blocks["Header"])) return nullptr;
	if (!(scene = SceneDataHandler(blocks["SceneData"]))) return nullptr;

	for (auto block : blocks) 
	{
		if (block.first == "Entities")
		{
			EntitiesHandler(scene, blocks[block.first]);
		}
		else if (block.first == "Viewports")
		{
			ViewportsHandler(scene, blocks[block.first]);
		}
		else if (block.first == "FreeEntities")
		{
			auto list = block.second[2];	//WARNING Обезопасить эту заглушку

			auto splited_list = SplitString(list, ' ');

			for (auto n : splited_list) 
			{
				scene->GetEntitySpace().GetFreeEntities().push_back(std::stoi(n));
			}
		}
	}
	return scene;
}

std::map<std::string, std::vector<std::string>>  SceneLoader::SeparateToBlocks(const std::string& str) const
{
	std::map<std::string, std::vector<std::string>> blocks;
	auto lines = SplitToLines(str);

	std::string current_block = "";

	for (auto& line : lines) 
	{
		if (StartsWith(line, "###")) 
		{
			current_block = line.substr(4);
		}
		if (current_block != "")
		{
			blocks[current_block].push_back(line);
		}
	}

	return blocks;
}

bool SceneLoader::HeaderHandler(std::vector<std::string>& text) const
{
	auto elsm = ServiceLocator::Get<SettingsManager>();
	auto logger = ServiceLocator::Get<Logger>();

	constexpr float VERSION_WARN = 0.05f;
	constexpr float VERSION_ERROR = 1.0f;

	for (auto line : text) 
	{
		auto parameter_value = SplitString(line, ' ');

		if (parameter_value.size() == 0) continue;
		//if (p) {
		//	logger->Log("Invalid header line: " + line, ELLogger::Level::Warning);
		//	continue;
		//}

		if (parameter_value[0] == "ENGINE_NAME" && parameter_value.size() >= 2)
		{
			std::string name;
			for (int i = 1; i < parameter_value.size(); i++)
			{
				auto l = parameter_value[i];
				name += l + " ";
			}
			name.pop_back();
			if (name != elsm->GetEngineName())
			{
				logger->Log("(Scene loading): scene is from other engine.", Logger::Level::Error);
				return false;
			}
		}
		if (parameter_value[0] == "ENGINE_VERSION" && parameter_value.size() == 2)
		{
			const float sceneVersion = std::stof(parameter_value[1]);
			const float engineVersion = elsm->GetEngineVersion();
			const float diff = std::abs(sceneVersion - engineVersion);

			if (diff > VERSION_ERROR) {
				logger->Log("(Scene loading): scene and engine have too different versions. Scene: " +
					parameter_value[1] + " Engine: " +
					std::to_string(engineVersion),
					Logger::Level::Error);
				return false;
			}
			else if (diff > VERSION_WARN) {
				logger->Log("(Scene loading): scene and engine have different versions. Scene: " +
					parameter_value[1] + " Engine: " +
					std::to_string(engineVersion),
					Logger::Level::Warning);
				continue;
			}
		}
	}

	return true;
}

SceneContext* SceneLoader::SceneDataHandler(std::vector<std::string>& text) const
{
	// Если header пропустил сцену, то её можно начать обрабатывать
	SceneID context = ServiceLocator::Get<SceneManager>()->CreateContext("Scene");

	auto logger = ServiceLocator::Get<Logger>();

	for (size_t i = 0; i < text.size(); i++)
	{
		auto line = text[i];
		line = EraseString(line,"\t");
		auto parameter_value = SplitString(line, ' ');

		if (parameter_value.size() == 0) continue;

		

		if (parameter_value[0] == "SCENE_NAME" && parameter_value.size() == 2)
		{
			ServiceLocator::Get<SceneManager>()->GetContext(context)->SetName(parameter_value[1]);
			continue;
		}
		else if (parameter_value[0] == "SCENE_DESCRIPTION" && parameter_value.size() == 1)
		{
			std::string desc;
			for (size_t u = i + 1; u < text.size(); u++)
			{
				auto u_line = text[u];
				if (u_line =="}")
				{
					if (!desc.empty()) {
						desc.pop_back();  // Удаляем последний '\n'
					}
					i = u;	//Передвигаем i до u
					break;
				}
				if (u_line == "{") continue;	//Для описания сцены startblock всего один

				desc += u_line + "\n";
			}
			ServiceLocator::Get<SceneManager>()->GetContext(context)->SetDescription(desc);
			continue;
		}
	}

	return ServiceLocator::Get<SceneManager>()->GetContext(context);
}

void SceneLoader::EntitiesHandler(SceneContext* context, std::vector<std::string>& text) const
{
	ECS::SerializationManager::DeserializeFromLines(context->GetEntitySpace(), text);
}

void SceneLoader::ViewportsHandler(SceneContext* context, std::vector<std::string>& text) const
{
	std::unordered_map<std::string, ECS::ESDL::ESDLType> fields;

	bool viewportWrite = false;

	for (auto str : text) 
	{
		if (str.empty()) continue;
		str = ECS::ESDL::Strings::EraseString(str, "{");
		if (str.starts_with("Viewport")) 
		{
			viewportWrite = true;
			continue;
		}
		auto parsed = ECS::ESDL::Strings::SplitString(str, ':',1);
		if (parsed.size() == 2) 
		{
			fields[parsed[0]] = ECS::ESDL::ValueParser::Parse(parsed[1]);
		}

		if (str.back() == '}')
		{
			context->GetViewportService().CreateViewportByFields(fields);
			fields.clear();
			viewportWrite = false;
		}
	}
	viewportWrite = true;
}
