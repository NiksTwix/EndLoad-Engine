#pragma once
#include <Services\Scenes\SceneContext.hpp>
#include <Core\IServices.hpp>

/* SceneManager replace's EntityManager
*/

namespace Scenes 
{

	class SceneManager : public Core::IService
	{
	private:
		std::unordered_map<SceneID, std::unique_ptr<SceneContext>> scenes;
		SceneID last_id = 1;
	public:
		//Разрешаем перемещать
		SceneManager(SceneManager&&) = default;
		SceneManager& operator=(SceneManager&&) = default;
		~SceneManager();

		SceneManager();

		SceneContext* GetContext(SceneID id) const;

		SceneContext* CreateContext(std::string name);

		void DeleteContext(SceneID id);

		bool IsSceneValid(SceneID id) const;

		void Shutdown() override;

		void RegisterBasicCS(SceneContext* scene); // Register basic component's services
	};
}

