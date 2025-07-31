#pragma once
#include <unordered_map>
#include <memory>
#include <Services\Scene\SceneContext.hpp>
#include <Core\IServices.hpp>

/* SceneManager является заменой EntityManager
*/

using SceneID = size_t;	


class SceneManager : public IHelperService 
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

	SceneID CreateContext(std::string name);

	void DeleteContext(SceneID id);

	bool IsSceneValid(SceneID id) const;

	void Shutdown() override;

	void RegisterBasicCS(SceneContext* scene); // Регистрирует базовые сервисы компонентов

};