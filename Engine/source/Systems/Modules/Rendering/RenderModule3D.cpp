#include <Systems/Modules/Rendering/RenderModule3D.hpp>
#include <Systems\Graphics\Windows\WindowsManager.hpp>
#include <Core\ServiceLocator.hpp>
#include <Systems\Graphics\Rendering\RenderSystem.hpp>
#include <Components/Graphics/MeshComponent.hpp>
#include <Components/Graphics/MaterialComponent.hpp>
#include <Components/Basic/TransformComponent.hpp>
namespace Rendering 
{
	void RenderModule3D::Update()
	{
		//If module was called window exists

		//Update data
		UpdateState();
		if (!m_windowManager || !m_currentScene) 
		{	
			//TODO ErrorMessage
			return; 
		}
		if (!m_cameraService || !m_meshService || !m_materialService || !m_transformService) return; //TODO Error Message
		SetUpContext();
		BuildCommands();
		RenderCommands();
	}

	void RenderModule3D::SetUpContext()
	{
		//��������� ��������� (GraphicsDevice)
	}

	void RenderModule3D::BuildCommands()
	{
		auto& entitySpace = m_currentScene->GetEntitySpace();

		ECS::View<Components::LocalTransformComponent, Components::MeshComponent, Components::MaterialComponent> view(entitySpace.GetRegistry());

		/*
		auto* camera = m_cameraService->GetRenderCamera();
		if (!camera) return;
		*/
		view.Each([&](auto& entity_id, Components::LocalTransformComponent& localt, Components::MeshComponent& mesh, Components::MaterialComponent& material)
			{
				Graphics::RenderCommand command;
				if (!m_meshService->UpdateResourceState(entity_id)) return; // Skip If mesh invalid - object will be black or invisible
				if (!m_windowManager->GetRenderWindow()->GetGraphicsDevice()->IsMeshValid(mesh.meshID))	//Skip this mesh
				{
					//TODO error
					return;
				}
				if (!m_materialService->UpdateResourceState(entity_id)) return; // Skip If material or shader are invalid - object will be black or invisible
				if (!m_windowManager->GetRenderWindow()->GetGraphicsDevice()->IsShaderValid(material.shaderID))	//Skip this object
				{
					//TODO error
					return;
				}
				command.mesh_id = mesh.meshID;
				command.shader_id = material.shaderID;
				command.uniforms = material.uniforms;
				//Add new uniforms
				command.uniforms["texture_exists"] = Graphics::UniformValue((Definitions::uint)0);
				for (auto& text : material.texturesID) 
				{
					if (text.second == Definitions::InvalidID) 
					{
						continue;
					}
					command.uniforms["texture_exists"] = command.uniforms["texture_exists"] | text.first;	// 00000000 + 00000001
					command.textures.push_back({ text.second,text.first });	//TextureID, TextureType (enum class) -> slot
				}

				//���������� ������ ���������

				//auto gt = context->GetEntitySpace().GetComponent<GlobalTransformComponent>(id);
				//if (m_settingsManager->GetSwitch(Switches::FrustumCulling))
				//{
				//	auto aabb = m_meshService->GetTransformedAABB(mesh,gt.ModelMatrix);
				//	if (!(m_cameraService->IsVisibleAABB(aabb, *camera)))
				//	{
				//		return;
				//	}
				//}
				//if (!gt.Visible) return;

				//command.uniforms["ViewMatrix"].data = camera->View;
				//command.uniforms["ProjectionMatrix"].data = camera->Projection;
				//command.uniforms["WorldMatrix"].data = gt.ModelMatrix;

				m_currentRenderCommands.push_back(command);	//global state
			});

	}
	void RenderModule3D::UpdateState()
	{
		m_windowManager = Core::ServiceLocator::Get<Windows::WindowsManager>();
		m_currentRenderCommands.clear(); 
		auto rs = Core::ServiceLocator::Get<RenderSystem>();
		auto sm = Core::ServiceLocator::Get<Scenes::SceneManager>();
		if (!rs || !sm || !m_windowManager) return;
		m_currentScene = sm->GetContext(rs->GetAttachedScene(m_windowManager->GetRenderWindowID()));
		
		auto& csl = m_currentScene->GetEntitySpace().GetServiceLocator();
		m_cameraService = csl.GetByService<Components::CameraComponentService>();
		m_transformService = csl.GetByService<Components::TransformComponentService>();
		m_meshService = csl.GetByService<Components::MeshComponentService>();
		m_materialService = csl.GetByService<Components::MaterialComponentService>();
		
	}
	void RenderModule3D::RenderCommands()
	{
		if (m_currentRenderCommands.empty()) return;
		auto device = m_windowManager->GetRenderWindow()->GetGraphicsDevice();
		if (!device) return; //TODO Error Message
		device->DrawBatch(m_currentRenderCommands);
	}
}