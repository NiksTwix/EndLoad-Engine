#include <Services/Resources/Graphics\ReadyShaderTexts.hpp>

//Временное решение

std::string DefaultLineOGLShader_V = R"SHADER(
#version 450 core
layout (location = 0) in vec3 aPos;


uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 WorldMatrix;


void main() {
    gl_Position = ProjectionMatrix*ViewMatrix*vec4(aPos, 1.0);    //*WorldMatrix
}
)SHADER";

std::string DefaultLineOGLShader_F = R"SHADER(
#version 450 core
out vec4 FragColor;

uniform vec4 Color;

void main() {

    float depth = gl_FragCoord.z; // Диапазон [0,1]
    FragColor = Color;
    //FragColor = vec4(dist/10.0f, 0.3, 0.2, 1.0); // Оранжевый
}
)SHADER";


std::string DefaultSpriteOGLShader_V = R"glsl(
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 nPos;
layout (location = 2) in vec2 tPos;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 WorldMatrix;

uniform float PixelPerUnit;
uniform vec2 TextureSize;

out vec2 TexCoords;

void main() {
    TexCoords = tPos;
    vec2 scaledTS = TextureSize / PixelPerUnit;
    gl_Position = ProjectionMatrix * ViewMatrix * WorldMatrix *  vec4(aPos * vec3(scaledTS.xy,1.0), 1.0);
}
 )glsl";

std::string DefaultSpriteOGLShader_F = R"glsl(
#version 450 core
in vec2 TexCoords;
out vec4 Color;
uniform sampler2D Image;
uniform vec4 SpriteColor;

void main() {
    Color = texture(Image, TexCoords) * SpriteColor;
}
)glsl";