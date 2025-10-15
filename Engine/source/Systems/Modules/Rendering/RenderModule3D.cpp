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
		//Настройка контекста (GraphicsDevice)
	}

	void RenderModule3D::BuildCommands()
	{
		
		auto& entitySpace = m_currentScene->GetEntitySpace();

		ECS::View<Components::LocalTransformComponent, Components::MeshComponent, Components::MaterialComponent> view(entitySpace.GetRegistry());

		/*
		auto* camera = m_cameraService->GetRenderCamera();
		if (!camera) return;
		*/

		view.Each([&](auto& entity_id, auto& localt, auto& mesh, auto& material)
			{
				Graphics::RenderCommand command;
				if (!mesh.isValid || mesh.currentResource == Definitions::InvalidID) return;
				if (mesh.currentResource != mesh.previousResource)
				{
					//Check and init resource if he isnt inited
					if (m_meshService->InitResource(mesh)) mesh.previousResource = mesh.currentResource;
				}
				if (!m_windowManager->GetRenderWindow()->GetGraphicsDevice()->IsMeshValid(mesh.meshID))	//Skip this mesh
				{
					//TODO error
					return;
				}
				
				command.mesh_id = mesh.meshID;
				//command.shader_id = material.shaderID
				/*
					command.shader_id = material.shaderID

					//Обновления матриц материала

					auto gt = context->GetEntitySpace().GetComponent<GlobalTransformComponent>(id);
					if (m_settingsManager->GetSwitch(Switches::FrustumCulling))
					{
						auto aabb = m_meshService->GetTransformedAABB(mesh,gt.ModelMatrix);
						if (!(m_cameraService->IsVisibleAABB(aabb, *camera)))
						{
							return;
						}
					}
					if (!gt.Visible) return;

					material.uniforms["ViewMatrix"].data = camera->View;
					material.uniforms["ProjectionMatrix"].data = camera->Projection;
					material.uniforms["WorldMatrix"].data = gt.ModelMatrix;

					command.uniforms = (material.uniforms)
					for (texture : material.textures)
					{
						command.textures.pushback({texture.textureID, texture.slot});
					}
				*/
				m_currentRenderCommands.push_back(command);
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