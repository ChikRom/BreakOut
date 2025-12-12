#include "shader.h"


// read shader files, generate shaders, compile and link them
void Shader::Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
	/*std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}

	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();*/

	unsigned int vertex, fragment, geometry;

	// Vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexSource, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");

	// Fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentSource, NULL);
	glCompileShader(fragment);

	checkCompileErrors(fragment, "FRAGMENT");

	// if geometry shader source is given, also compile geometry shader
	if (geometrySource != nullptr)
	{
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &geometrySource, NULL);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");
	}

	// Shader program
	ID = glCreateProgram();
	glAttachShader(this->ID, vertex);
	glAttachShader(this->ID, fragment);
	if (geometrySource != nullptr)
		glAttachShader(this->ID, geometry);
	glLinkProgram(this->ID);
	checkCompileErrors(this->ID, "PROGRAM");


	// delete shaders
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometrySource != nullptr)
		glDeleteShader(geometry);
}

Shader &Shader::Use() 
{
	glUseProgram(this->ID);
	return *this;
}


// set bool uniform (input) for our shaders
void Shader::setBool(const std::string& name, bool value, bool useShader)
{
	if (useShader)
		this->Use();
	glUniform1i(glGetUniformLocation(this->ID, name.c_str()), (int)value);
}


// set int uniform (input) for our shaders
void Shader::setInt(const std::string& name, int value, bool useShader)
{
	if (useShader)
		this->Use();
	glUniform1i(glGetUniformLocation(this->ID, name.c_str()), value);
}

// set float uniform (input) for our shaders
void Shader::setFloat(const std::string& name, float value, bool useShader)
{
	if (useShader)
		this->Use();
	glUniform1f(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, float x, float y, bool useShader)
{
	if (useShader)
		this->Use();
	glUniform2f(glGetUniformLocation(this->ID, name.c_str()), x, y);

}

void Shader::setVec2(const std::string& name, const glm::vec2& vector, bool useShader)
{
	if (useShader)
		this->Use();
	glUniform2fv(glGetUniformLocation(this->ID, name.c_str()),1,&vector[0]);
}

// set vec 3 uniform (input) for our shaders
void Shader::setVec3(const std::string& name, const glm::vec3& vector, bool useShader)
{
	if (useShader)
		this->Use();
	glUniform3fv(glGetUniformLocation(this->ID, name.c_str()), 1, &vector[0]);
}

void Shader::setVec3(const std::string& name, float x,float y, float z, bool useShader)
{
	if (useShader)
		this->Use();
	glUniform3f(glGetUniformLocation(this->ID, name.c_str()),x,y,z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& vector, bool useShader)
{
	if (useShader)
		this->Use();
	glUniform4fv(glGetUniformLocation(this->ID, name.c_str()), 1, &vector[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w, bool useShader)
{
	if (useShader)
		this->Use();
	glUniform4f(glGetUniformLocation(this->ID, name.c_str()), x, y, z,w);
}

// set matrix4 uniform (input) for our shaders
void Shader::setMat4(const std::string& name, const glm::mat4& matrix, bool useShader)
{
	if (useShader)
		this->Use();
	glUniformMatrix4fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
}


// check errors for compiling and linking shaders
void Shader::checkCompileErrors(unsigned int object, std::string type)
{
	int success;
	char infoLog[1024];
	if (type == "PROGRAM")
	{
		glGetProgramiv(object, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::SHADER: Link-time error: Type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
	else
	{
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
}