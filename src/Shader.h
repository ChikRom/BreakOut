#ifndef SHADER_H
#define SHADER_H


#include "GLAD/glad/glad.h"
#include "glm/glm.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>



class Shader
{

public:
	unsigned int ID;
public:
	// reads and builds the shader

	// constructor
	Shader() { }
	// compile the shaders
	void Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);
	
	Shader &Use(); // activate the shader

	//utility for uniform functions
	void setBool(const std::string& name, bool value, bool useShader = false);
	void setInt(const std::string& name, int value, bool useShader = false);
	void setFloat(const std::string& name, float value, bool useShader = false);
	void setVec2(const std::string& name, const glm::vec2& vector, bool useShader = false);
	void setVec2(const std::string& name, float x, float y, bool useShader = false);
	void setVec3(const std::string& name, const glm::vec3& vector, bool useShader = false);
	void setVec3(const std::string& name, float x, float y, float z, bool useShader = false);
	void setVec4(const std::string& name, const glm::vec4& vector, bool useShader = false);
	void setVec4(const std::string& name, float x, float y, float z, float w, bool useShader = false);
	void setMat4(const std::string& name, const glm::mat4& matrix, bool useShader = false);
private:
	void checkCompileErrors(unsigned int object, std::string type);

};



#endif