#pragma once
#include <Core\IServices.hpp>
#include <Services\Scene\SceneManager.hpp>
#include <map>


class SceneLoader : public IHelperService
{
	/*
		����� ��� ���������� ����� � ������ ��������
	*/
public:
	void SaveAsELScene(std::string path, SceneContext& scene) const;
	/*
		ELScene - ��� ����������� � ������������ ������ ������, ������� ����� ������� � ������/�������������� ��� ��������
	*/

	SceneContext* LoadELScene(std::string path) const;

	void SaveAsJson(std::string path, SceneContext& scene) const;
	void SaveAsBinary(std::string path, SceneContext& scene) const;
private:
	std::map<std::string, std::vector<std::string>>  SeparateToBlocks(const std::string& str) const;	//�������� ������ � elscene

	bool HeaderHandler(std::vector<std::string>& text) const;	//��������� ���������� �����

	SceneContext* SceneDataHandler(std::vector<std::string>& text) const;

	void EntitiesHandler(SceneContext* context, std::vector<std::string>& text) const;

	void ViewportsHandler(SceneContext* context, std::vector<std::string>& text) const;

};