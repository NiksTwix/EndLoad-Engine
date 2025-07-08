#pragma once
#include <Core\GL.hpp>
#include <unordered_map>
#include <vector>
namespace Drawings 
{
	extern std::unordered_map<GLuint, std::vector<GLuint>> ActiveGBuffers;	//VAO è VBO's

	

	GLuint CreateQuadVAO();	//Return VAO of gl Quat's buffer
	GLuint CreateUnsignedQuadVAO();	//Return VAO of gl Quat's buffer
	void DeleteGArray(GLuint vao);
	void BindGArray(GLuint vao);
}



