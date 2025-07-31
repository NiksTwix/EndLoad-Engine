#include <Systems\Modules\SpriteRenderModule.hpp>
#include <Services\Scene\SceneContext.hpp>
#include <Core\ServiceLocator.hpp>
#include <Services/Resources/ResourceManager.hpp>
#include <Resources/TextureResource.hpp>
#include <Resources\ShaderResource.hpp>
#include <Systems\Render\Drawings.hpp>
#include <Systems\Render\Render.hpp>
#include <Services\Settings\SettingsManager.hpp>

SpriteRenderModule::SpriteRenderModule()
{
	auto shader_ = ServiceLocator::Get<ResourceManager>()->Load<ShaderResource>(ServiceLocator::Get<SettingsManager>()->GetPaths(Paths::SpriteShader));
	if (shader_ == nullptr) 
	{
		ServiceLocator::Get<Logger>()->Log("(SpriteRenderModule): invalid shader.", Logger::Level::Error);
		return;
	}
	shader = shader_->GetContent();
	//shader = std::make_unique<GLShader>();
	//shader->LoadFromSource(DefaultSpriteOGLShader_V, DefaultSpriteOGLShader_F);
	
	std::vector<Math::Vertex> vertices =
	{
		Math::Vertex(Math::Vector3(-0.5f, -0.5f, 0.0f),	Math::Vector3(0),Math::Vector2(0, 0)),		// Низ-лево
		Math::Vertex(Math::Vector3(0.5f, -0.5f, 0.0f),	Math::Vector3(0),Math::Vector2(1, 0)),		// Низ-право
		Math::Vertex(Math::Vector3(0.5f, 0.5f, 0.0f),	Math::Vector3(0),Math::Vector2(1, 1)),		// Верх-право
		Math::Vertex(Math::Vector3(-0.5f, 0.5f, 0.0f),	Math::Vector3(0),Math::Vector2(0, 1)),		// Верх-лево
	
	};
	
	std::vector<unsigned int> indices = {
			0, 1, 2,  // Первый треугольник (нижний правый угол)
			2, 3, 0   // Второй треугольник (верхний левый угол)
	};
	indices_count = indices.size();
	plane_vao = ServiceLocator::Get<Render>()->GetContext()->SetupMesh(vertices, indices);
}

uint8_t SpriteRenderModule::GetPriority() const
{
	return 2;
}

void SpriteRenderModule::Update(SceneContext* context, float deltaTime)
{
	if (!context || !shader) return;
	m_transformService = context->GetEntitySpace().GetServiceLocator().GetByService<TransformService>();
	m_cameraService = context->GetEntitySpace().GetServiceLocator().GetByService<CameraService>();
	if (!m_cameraService->GetRenderCamera()) return;
	ECS::Registry& registry = context->GetEntitySpace().GetRegistry();

	auto* rcontext = ServiceLocator::Get<Render>()->GetContext();

	rcontext->Set(RCSettings::BLEND, RCSettingsValues::TRUE);
	rcontext->Set(RCSettings::BLEND_MODE, RCSettingsValues::SRC_ALPHA);

	auto m_currentViewport = rcontext->GetViewport();

	rcontext->Set(RCSettings::DEPTH_TEST, RCSettingsValues::TRUE);
	

	Math::Matrix4x4 projection = m_cameraService->GetRenderCamera()->Projection;
	Math::Matrix4x4 view_m = m_cameraService->GetRenderCamera()->View;
	shader->Bind();
	shader->SetUniform("ProjectionMatrix", projection);
	shader->SetUniform("ViewMatrix", view_m);

	ECS::View<LocalTransformComponent, SpriteComponent> view(registry);
	view.Each([&](ECS::EntityID entity_id, auto& transform, auto& sprite) {
		rcontext->ResetBuffers();
		RenderSprite(entity_id, transform, sprite, context);
		});
	shader->Unbind();
	rcontext->Set(RCSettings::BLEND, RCSettingsValues::FALSE);
	rcontext->Set(RCSettings::DEPTH_TEST, RCSettingsValues::FALSE);
}

SpriteRenderModule::~SpriteRenderModule()
{
	shader->Release();
	ServiceLocator::Get<Render>()->GetContext()->DestroyMesh(plane_vao);
}

void SpriteRenderModule::RenderSprite(ECS::EntityID id, LocalTransformComponent& transform, SpriteComponent& sprite, SceneContext* context)
{
	if (!transform.Visible) return;
	auto gt = context->GetEntitySpace().GetComponent<GlobalTransformComponent>(id);
	if (!gt.Visible) return;

	// Загружаем текстуру
	auto texture_source = ServiceLocator::Get<ResourceManager>()
		->Load<TextureResource>(sprite.TexturePath);
	if (!texture_source) return;

	GLuint texture = texture_source->GetID();

	// Устанавливаем параметры рендеринга
	shader->SetUniform("WorldMatrix", gt.ModelMatrix);
	shader->SetUniform("SpriteColor", sprite.Color);
	shader->SetUniform("TextureSize", texture_source->GetSize());
	shader->SetUniform("PixelPerUnit", pixelPerUnit);
	// Привязываем текстуру и рисуем
	// Привязка текстуры и отрисовка
	auto* rcontext = ServiceLocator::Get<Render>()->GetContext();
	rcontext->BindTexture(0, texture_source->GetID());
	shader->SetUniform("Image", 0);

	// Отрисовка меша
	rcontext->DrawElements(plane_vao, indices_count);  
}
