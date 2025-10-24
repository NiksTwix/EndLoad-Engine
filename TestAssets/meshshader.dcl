tag::shader MeshShader
{
	Language: "GLSL";
	key Name: "MeshShader";
	tag::sources Vertex 
	{
		Type: "vertex";
		Code: "
#version 450 core
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_tc;     //Texture coordinates
layout (location = 3) in vec3 v_tangent;
layout (location = 4) in vec3 v_bitangent;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 WorldMatrix;

out vec2 TextureCoords;
out vec3 Normal;

void main() {
    gl_Position = ProjectionMatrix*ViewMatrix*WorldMatrix*vec4(v_position, 1.0);    
    TextureCoords = v_tc;
    Normal = v_normal;
}

";
	}
	tag::sources Fragment
	{
		Type: "fragment";
		Code: "
#version 450 core
out vec4 FragColor;

in vec2 TextureCoords;
in vec3 Normal;

const uint ALBEDO_MASK = 1;
const uint NORMAL_MASK = 2; 
const uint METALLIC_MASK = 4;

uniform sampler2D ALBEDO;
uniform uint TextureExists;
uniform vec4 BaseColor;

void main() {

    float depth = gl_FragCoord.z; // Диапазон [0,1]
    vec2 coords = vec2(TextureCoords.x,1 - TextureCoords.y);
    float useAlbedo = float((TextureExists & 1) != 0); // 0.0 или 1.0
    
    vec3 albedo = mix(
        BaseColor.rgb,                           // if useAlbedo = 0
        texture(ALBEDO, TextureCoords).rgb,  // if useAlbedo = 1  
        useAlbedo                            // interpolation coefficient
    );
    FragColor = vec4(depth * albedo,1.0f);
}

";
	}
}