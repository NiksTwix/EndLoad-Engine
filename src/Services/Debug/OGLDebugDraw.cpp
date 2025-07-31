#include <Services\Debug\OGLDebugDraw.hpp>
#include <Core\ServiceLocator.hpp>
#include <Resources\ReadyShaderTexts.hpp>

OGLDebugDraw::OGLDebugDraw()
{
    Init();
}

OGLDebugDraw::~OGLDebugDraw()
{
    delete lineShader;
}

void OGLDebugDraw::Render()
{
    //Отрисовка линий
    
    glLineWidth(3.0f);
    glEnable(GL_DEPTH_TEST);
    // Отрисовка линий
    for (int i = 0; i < lines.size();)
    {
        auto& line = lines[i];
        DrawLine(line.start, line.end, line.color);
    
        if (line.duration <= 0)
        {
            // Безопасный обмен с последним элементом и удаление
            if (i != lines.size() - 1) {
                std::swap(lines[i], lines.back());
            }
            lines.pop_back();
        }
        else
        {
            i++; // Переход к следующему элементу только если не удалили текущий
        }
    }
    
    // Отрисовка осей
    for (int i = 0; i < axes.size();)
    {
        auto& axis = axes[i];
        DrawLine(axis.origin, axis.origin + axis.XDir, Math::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
        DrawLine(axis.origin, axis.origin + axis.YDir, Math::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
        DrawLine(axis.origin, axis.origin + axis.ZDir, Math::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    
        if (axis.duration <= 0)
        {
            if (i != axes.size() - 1) {
                std::swap(axes[i], axes.back());
            }
            axes.pop_back();
        }
        else
        {
            i++;
        }
    }
    glLineWidth(1.0f);
    // Отрисовка сетки
    for (const auto& grid : grids)
    {
        DrawGrid(grid.cellSize, grid.cellSize, grid.color);
    }
    
    glLineWidth(1.0f);
}

void OGLDebugDraw::Init()
{
    if (m_isValid) return; m_isValid = true;
    lineShader = new GLShader();
    lineShader->LoadFromSource(DefaultLineOGLShader_V, DefaultLineOGLShader_F);


    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*6, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
}

void OGLDebugDraw::Shutdown()
{
    delete lineShader;
    m_isValid = false;
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void OGLDebugDraw::SetCameraData(Math::Matrix4x4 projection, Math::Matrix4x4 view, Math::Vector3 pos_of_view)
{
    this->projection = projection;
    this->view = view;
    this->pos_of_view = pos_of_view;
}

void OGLDebugDraw::DrawLine(Math::Vector3 start, Math::Vector3 end, Math::Vector4 color)
{
    if (lineShader == nullptr) return;

    float vertices[] = { start.x, start.y, start.z, end.x, end.y, end.z };


    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    
    lineShader->Bind();
    lineShader->SetUniform("ViewMatrix", view);
    lineShader->SetUniform("ProjectionMatrix", projection);
    lineShader->SetUniform("Color", color);
    glDrawArrays(GL_LINES, 0, 2);

    // 5. Чистим (можно не удалять, если вызывается часто, а переиспользовать)
    glBindVertexArray(0);
    
    lineShader->Unbind();
}

void OGLDebugDraw::DrawGrid(float cellSize, int cellCount, Math::Vector4 color)
{
    Math::Vector3 gridCenter = {
        floor(pos_of_view.x / cellSize) * cellSize,
        0,
        floor(pos_of_view.z / cellSize) * cellSize
    };

    for (int i = -cellCount; i <= cellCount; ++i) {
        DrawLine(
            { gridCenter.x + i * cellSize, 0, gridCenter.z - cellCount * cellSize },
            { gridCenter.x + i * cellSize, 0, gridCenter.z + cellCount * cellSize },
            color
        );
    }

    for (int i = -cellCount; i <= cellCount; ++i) {
        DrawLine(
            { gridCenter.x - cellCount * cellSize, 0, gridCenter.z+ i * cellSize },
            { gridCenter.x + cellCount * cellSize, 0, gridCenter.z+ i * cellSize },
            color
        );
    }
}

void OGLDebugDraw::DrawAABB(const AABB& aabb, Math::Vector4 color)
{
    //Horizontal

    Math::Vector3 L1_1 = aabb.Min;
    Math::Vector3 L1_2 = Math::Vector3(aabb.Max.x,aabb.Min.y,aabb.Min.z);
                 
    Math::Vector3 L2_1 = Math::Vector3(aabb.Min.x, aabb.Max.y, aabb.Min.z);
    Math::Vector3 L2_2 = Math::Vector3(aabb.Max.x, aabb.Max.y, aabb.Min.z);
                  
    Math::Vector3 L3_1 = Math::Vector3(aabb.Min.x, aabb.Min.y, aabb.Max.z);
    Math::Vector3 L3_2 = Math::Vector3(aabb.Max.x, aabb.Min.y, aabb.Max.z);
                  
    Math::Vector3 L4_1 = Math::Vector3(aabb.Min.x, aabb.Max.y, aabb.Max.z);
    Math::Vector3 L4_2 = Math::Vector3(aabb.Max.x, aabb.Max.y, aabb.Max.z);

    DrawLine(L1_1, L1_2, color);
    DrawLine(L2_1, L2_2, color);
    DrawLine(L3_1, L3_2, color);
    DrawLine(L4_1, L4_2, color);

    L1_1 = aabb.Min;
    L1_2 = Math::Vector3(aabb.Min.x, aabb.Min.y, aabb.Max.z);
    
    L2_1 = Math::Vector3(aabb.Min.x, aabb.Max.y, aabb.Min.z);
    L2_2 = Math::Vector3(aabb.Min.x, aabb.Max.y, aabb.Max.z);
    
    L3_1 = Math::Vector3(aabb.Max.x, aabb.Min.y, aabb.Min.z);
    L3_2 = Math::Vector3(aabb.Max.x, aabb.Min.y, aabb.Max.z);
    
    L4_1 = Math::Vector3(aabb.Max.x, aabb.Max.y, aabb.Min.z);
    L4_2 = Math::Vector3(aabb.Max.x, aabb.Max.y, aabb.Max.z);

    DrawLine(L1_1, L1_2, color);
    DrawLine(L2_1, L2_2, color);
    DrawLine(L3_1, L3_2, color);
    DrawLine(L4_1, L4_2, color);
    //Vertical

    L1_1 = aabb.Min;
    L1_2 = Math::Vector3(aabb.Min.x, aabb.Max.y, aabb.Min.z);
    
    L2_1 = Math::Vector3(aabb.Max.x, aabb.Min.y, aabb.Min.z);
    L2_2 = Math::Vector3(aabb.Max.x, aabb.Max.y, aabb.Min.z);
    
    L3_1 = Math::Vector3(aabb.Min.x, aabb.Min.y, aabb.Max.z);
    L3_2 = Math::Vector3(aabb.Min.x, aabb.Max.y, aabb.Max.z);
    
    L4_1 = Math::Vector3(aabb.Max.x, aabb.Min.y, aabb.Max.z);
    L4_2 = Math::Vector3(aabb.Max.x, aabb.Max.y, aabb.Max.z);

    DrawLine(L1_1, L1_2, color);
    DrawLine(L2_1, L2_2, color);
    DrawLine(L3_1, L3_2, color);
    DrawLine(L4_1, L4_2, color);
}
