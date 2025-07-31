#include <Systems\Render\Drawings.hpp>


namespace Drawings
{
    std::unordered_map<GLuint, std::vector<GLuint>> ActiveGBuffers;

    GLuint CreateQuadVAO()
    {
        GLuint VAO, VBO;

        float vertices[] = {
            // pos          // tex
            -1.0f,  1.0f,   0.0f, 1.0f,  // верх-лево
            -1.0f, -1.0f,   0.0f, 0.0f,  // низ-лево
             1.0f, -1.0f,   1.0f, 0.0f,  // низ-право

            -1.0f,  1.0f,   0.0f, 1.0f,  // верх-лево
             1.0f, -1.0f,   1.0f, 0.0f,  // низ-право
             1.0f,  1.0f,   1.0f, 1.0f   // верх-право
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Атрибут 0: Позиции (2 компонента)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // Атрибут 1: Текстурные координаты (2 компонента)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        ActiveGBuffers[VAO] = { VBO };
        return VAO;
    }

    GLuint CreateUnsignedQuadVAO()
    {
        GLuint VAO, VBO;

        float vertices[] = {
            // pos            // tex
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,

            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Атрибут 0: Позиции (2 компонента)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // Атрибут 1: Текстурные координаты (2 компонента)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        ActiveGBuffers[VAO] = { VBO };
        return VAO;
    }

    void DeleteGArray(GLuint vao)
    {
        auto it = ActiveGBuffers.find(vao);
        if (it == ActiveGBuffers.end()) return; // Не найден - ничего не делаем

        // Удаляем все связанные VBO
        for (auto vbo : it->second)
        {
            glDeleteBuffers(1, &vbo);
        }

        // Удаляем VAO
        glDeleteVertexArrays(1, &vao);

        // Удаляем запись из map
        ActiveGBuffers.erase(it);
    }
    void BindGArray(GLuint vao)
    {
        glBindVertexArray(vao);
    }
}

