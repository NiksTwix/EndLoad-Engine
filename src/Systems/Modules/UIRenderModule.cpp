#include <Systems\Modules\UIRenderModule.hpp>
#include <Services\Scene\SceneContext.hpp>
#include <Systems\Render\Render.hpp>
#include <Resources\ShaderResource.hpp>
#include <Services\Settings\SettingsManager.hpp>
#include <Services/Resources/ResourceManager.hpp>


UIRenderModule::UIRenderModule()
{

	auto shader_ = ServiceLocator::Get<ResourceManager>()->Load<ShaderResource>(ServiceLocator::Get<SettingsManager>()->GetPaths(Paths::PlaneShader));
	if (shader_ == nullptr)
	{
		ServiceLocator::Get<Logger>()->Log("(UIRenderModule): invalid bgshader.", Logger::Level::Error);
		return;
	}
	bgShader = shader_->GetContent();
	shader_ = ServiceLocator::Get<ResourceManager>()->Load<ShaderResource>(ServiceLocator::Get<SettingsManager>()->GetPaths(Paths::TextShader));
	if (shader_ == nullptr)
	{
		ServiceLocator::Get<Logger>()->Log("(UIRenderModule): invalid textshader.", Logger::Level::Error);
		return;
	}
	textShader = shader_->GetContent();
	std::vector<Math::Vertex> vertices =
	{
		Math::Vertex(Math::Vector3(-0.5f, -0.5f, 0.0f),	Math::Vector3(0),Math::Vector2(0, 0)),		// ���-����
		Math::Vertex(Math::Vector3(0.5f, -0.5f, 0.0f),	Math::Vector3(0),Math::Vector2(1, 0)),		// ���-�����
		Math::Vertex(Math::Vector3(0.5f, 0.5f, 0.0f),	Math::Vector3(0),Math::Vector2(1, 1)),		// ����-�����
		Math::Vertex(Math::Vector3(-0.5f, 0.5f, 0.0f),	Math::Vector3(0),Math::Vector2(0, 1)),		// ����-����

	};

	std::vector<unsigned int> indices = {
			0, 1, 2,  // ������ ����������� (������ ������ ����)
			2, 3, 0   // ������ ����������� (������� ����� ����)
	};
	indices_count = indices.size();
	plane_vao = ServiceLocator::Get<Render>()->GetContext()->SetupMesh(vertices, indices);
	text_plane_vao = ServiceLocator::Get<Render>()->GetContext()->SetupMesh(vertices, indices, RCBufferModes::DYNAMIC);
}

uint8_t UIRenderModule::GetPriority() const
{
	return 3;
}

void UIRenderModule::Update(SceneContext* context, float deltaTime)
{
	auto* service = context->GetEntitySpace().GetServiceLocator().GetByService<UIElementService>();

	//��������� ������ � UI ����� ��������
	//TODOFAST
	service->UpdateCameraID();	//33 �� ��� 500 �������� (1),  �� ��� ��������� 500 ��������
	// ���� ������ ����, � �� ��������, �������� ������ ���� ��� ���� ui ��������. ������������ BFS/DFS (����� � ������/�������). ����� ��������� ����������� ������, �� �� ��� ����
	service->UpdateUIGT();
	reg = &context->GetEntitySpace().GetRegistry();
	csl_ = &context->GetEntitySpace().GetServiceLocator();

	auto* rcontext = ServiceLocator::Get<Render>()->GetContext();
	rcontext->Set(RCSettings::BLEND, RCSettingsValues::TRUE);
	rcontext->Set(RCSettings::BLEND_MODE, RCSettingsValues::SRC_ALPHA);

	rcontext->Set(RCSettings::DEPTH_TEST, RCSettingsValues::FALSE);


	ECS::View<UIElementComponent,LocalTransformComponent> view(*reg);
	view.Each([&](ECS::EntityID entity_id, UIElementComponent& uiElement, LocalTransformComponent& transform)
	{
			if (!csl_->GetByService<CameraService>()->IsRenderCamera(uiElement.CameraID)) return; 

			DrawBackground(entity_id,uiElement);

			auto c_types = reg->GetComponentsTypes(entity_id);

			for (std::type_index& c_type : c_types) 
			{
				//csl_->GetByTypeIndex(c_type)->UpdateComponent(entity_id);
				if (c_type == typeid(TextRenderComponent)) 
				{
					DrawText(entity_id, uiElement);
				}

			}
	}
	);
	rcontext->Set(RCSettings::BLEND, RCSettingsValues::FALSE);	// 423 ��, 356 ��
}

void UIRenderModule::DrawBackground(ECS::EntityID entity, UIElementComponent& uiElement)
{
	if (!bgShader || !csl_) return;

	auto* rcontext = ServiceLocator::Get<Render>()->GetContext();
	auto* ts = csl_->GetByService<TransformService>();
	bgShader->Bind();
	bgShader->SetUniform("ProjectionMatrix", rcontext->GetViewport()->GetMatrix());//rcontext->GetViewport()->GetMatrix());csl_->GetByService<CameraService>()->GetRenderCamera()->Projection);
	bgShader->SetUniform("WorldMatrix", reg->Get<GlobalTransformComponent>(entity).ModelMatrix);
	bgShader->SetUniform("Size", uiElement.Size);
	bgShader->SetUniform("Color", uiElement.Color);
	rcontext->DrawElements(plane_vao, indices_count);
	bgShader->Unbind();
}

void UIRenderModule::DrawText(ECS::EntityID entity, UIElementComponent& uiElement)
{
	auto& textRender = reg->Get<TextRenderComponent>(entity);
	if (textRender.Text.empty() || !textRender.Font) return;

	auto* rcontext = ServiceLocator::Get<Render>()->GetContext();
	textShader->Bind();
	textShader->SetUniform("ProjectionMatrix", rcontext->GetViewport()->GetMatrix());
	textShader->SetUniform("Color", textRender.Color);
	textShader->SetUniform("Texture",0);
	auto* trs = csl_->GetByService<TextRenderService>();

	// �������� ���������� ������� (����� UI-��������)
	auto globalTransform = reg->Get<GlobalTransformComponent>(entity);
	Math::Vector2 textPosition = (Math::Vector2)globalTransform.Translation;

	// ������������ ��������� ������� ������ � ������ ������������
	Math::Vector2 textSize = trs->CalculateTextSize(textRender); 
	textPosition += trs->GetAlignmentOffset(textRender, uiElement.Size, textSize);

	// �������� ������ ������
	for (char32_t c : textRender.Text) {
		Character ch = textRender.Font->GetCharacter(c);
		if (ch.textureID == 0 || ch.size.x == 0 || ch.size.y == 0) {
			textPosition.x += ((int)ch.advance.x >> 6) * textRender.RenderScale.x; // ���������� ��������� �������, �� ��������� advance
			continue;
		}


		// ������� ������� � ������ bearing (�������� �����)
		float x = textPosition.x + ch.bearing.x * textRender.RenderScale.x;
		float y = textPosition.y - ch.bearing.y * textRender.RenderScale.y;

		// ������ �������
		float w = ch.size.x * textRender.RenderScale.x;
		float h = ch.size.y * textRender.RenderScale.y;

		std::vector<Math::Vertex> vertices =
		{
			Math::Vertex(Math::Vector3(x,		y + h,	0.0f),			Math::Vector3(0),	Math::Vector2(0,1)),		// ���-����
			Math::Vertex(Math::Vector3(x + w,	y + h,	0.0f),			Math::Vector3(0),	Math::Vector2(1,1)),		// ���-�����
			Math::Vertex(Math::Vector3(x + w,	y,		0.0f),			Math::Vector3(0),	Math::Vector2(1,0)),		// ����-�����
			Math::Vertex(Math::Vector3(x,		y,		0.0f),			Math::Vector3(0),	Math::Vector2(0,0)),		// ����-����

		};
		 
		rcontext->UpdateVBOData(text_plane_vao, vertices);
		rcontext->BindTexture(0, ch.textureID);
		rcontext->DrawElements(text_plane_vao, indices_count);

		// �������� ������� ��� ���������� ������� (� ������ advance)
		textPosition.x += ((int)ch.advance.x >> 6) * textRender.RenderScale.x;
	}

	rcontext->BindTexture(0, 0);
	textShader->Unbind();
}
