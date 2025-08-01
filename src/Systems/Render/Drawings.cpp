#include <Systems\Render\Drawings.hpp>


namespace Drawings
{
    std::unordered_map<GLuint, std::vector<GLuint>> ActiveGBuffers;

    GLuint CreateQuadVAO()
    {
        GLuint VAO, VBO;

        float vertices[] = {
            // pos          // tex
            -1.0f,  1.0f,   0.0f, 1.0f,  // ����-����
            -1.0f, -1.0f,   0.0f, 0.0f,  // ���-����
             1.0f, -1.0f,   1.0f, 0.0f,  // ���-�����

            -1.0f,  1.0f,   0.0f, 1.0f,  // ����-����
             1.0f, -1.0f,   1.0f, 0.0f,  // ���-�����
             1.0f,  1.0f,   1.0f, 1.0f   // ����-�����
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // ������� 0: ������� (2 ����������)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // ������� 1: ���������� ���������� (2 ����������)
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

        // ������� 0: ������� (2 ����������)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // ������� 1: ���������� ���������� (2 ����������)
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
        if (it == ActiveGBuffers.end()) return; // �� ������ - ������ �� ������

        // ������� ��� ��������� VBO
        for (auto vbo : it->second)
        {
            glDeleteBuffers(1, &vbo);
        }

        // ������� VAO
        glDeleteVertexArrays(1, &vao);

        // ������� ������ �� map
        ActiveGBuffers.erase(it);
    }
    void BindGArray(GLuint vao)
    {
        glBindVertexArray(vao);
    }
}

