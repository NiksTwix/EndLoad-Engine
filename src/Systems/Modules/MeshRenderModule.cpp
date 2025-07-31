#include <Systems\Modules\MeshRenderModule.hpp>
#include <Core/ServiceLocator.hpp>
#include <Services\Scene\SceneContext.hpp>
#include <Services\Settings\SettingsManager.hpp>
#include <Systems\Render\Render.hpp>

MeshRenderModule::MeshRenderModule()
{
}

uint8_t MeshRenderModule::GetPriority() const
{
	return 1;
}

void MeshRenderModule::Update(SceneContext* context, float deltaTime)
{
	if (!context) return;
	auto& csl = context->GetEntitySpace().GetServiceLocator();
	m_cameraService = csl.GetByService<CameraService>();
	m_settingsManager = ServiceLocator::Get<SettingsManager>();
	m_transformService = csl.GetByService<TransformService>();
	m_meshService = csl.GetByService<MeshService>();
	m_materialService = csl.GetByService<MaterialService>();
	if (!m_cameraService || m_cameraService->GetRenderCamera() == nullptr) return;


	auto* rcontext = ServiceLocator::Get<Render>()->GetContext();
	rcontext->Set(RCSettings::DEPTH_TEST, RCSettingsValues::TRUE);
	ECS::Registry& registry = context->GetEntitySpace().GetRegistry();

	ECS::View<LocalTransformComponent, MeshComponent, MaterialComponent> view(registry);

	view.Each([&](ECS::EntityID entity_id, auto& transform, auto& mesh, auto& material)
		{
			rcontext->ResetBuffers();
			RenderMesh(entity_id, transform, mesh, material, context);
		});
}

void MeshRenderModule::RenderMesh(ECS::EntityID id, LocalTransformComponent& transform, MeshComponent& mesh, MaterialComponent& material, SceneContext* context)
{
	auto gt = context->GetEntitySpace().GetComponent<GlobalTransformComponent>(id);
	if (m_settingsManager->GetSwitch(Switches::FrustumCulling))
	{
		auto aabb = m_meshService->GetTransformedAABB(mesh,gt.ModelMatrix);
		if (!(m_cameraService->IsVisibleAABB(aabb, *m_cameraService->GetRenderCamera())))
		{
			return;
		}
	}

	if (!gt.Visible) return;

	material.uniforms["ViewMatrix"].data = m_cameraService->GetRenderCamera()->View;
	material.uniforms["ProjectionMatrix"].data = m_cameraService->GetRenderCamera()->Projection;
	material.uniforms["WorldMatrix"].data = gt.ModelMatrix;

	m_materialService->Bind(material);
	m_meshService->Bind(mesh.mesh_id);
	m_materialService->Unbind(material);
}
