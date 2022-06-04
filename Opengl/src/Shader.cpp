#include "Shader.h"
#include<string>
#include<unordered_map>
#include<sstream>
#include<iostream>
#include<fstream>
#include<GL/glew.h>
#include "Renderer.h"
using namespace std;
Shader::Shader(const std::string & filepath):m_FilePath(filepath),m_RendererId(0)
{
	ShaderProgramSource source = ParseShader(filepath);
	m_RendererId = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererId));
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererId));
}

void Shader::UnBind() const
{
	GLCall(glUseProgram(0));
}


ShaderProgramSource Shader::ParseShader(const std::string &filePath) {
	std::ifstream stream(filePath);
	std::string line;
	enum  class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1

	};
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				//set mode to vertex
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				//set mode to fragment
				type = ShaderType::FRAGMENT;

			}
		}
		else {
			//push into the 
			ss[(int)type] << line << "\n";
		}
	}

	return { ss[0].str(),ss[1].str() };


}
int Shader::CreateShader(const std::string& vertexShader, const std::string & fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);

	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}
unsigned int Shader::GetUniformLocation(const std::string &name)
{

	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
		return m_UniformLocationCache[name];
	}
	GLCall(unsigned int location= glGetUniformLocation(m_RendererId, name.c_str()));
	if (location == -1){
		std::cout << "Warning :uniform " << name << " doesn't exists"<<std::endl;
	}
	m_UniformLocationCache[name] = location;
	return location;
}
void Shader::SetUniform4f(const std::string & name, float v1, float v2, float v3, float v4)
{
	GLCall(glUniform4f(GetUniformLocation(name), v1, v2, v3, v4));
}
unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Fail o compile" << (type == GL_VERTEX_SHADER ? "vertshader" : "fragmentShader") << " shader !" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	//todo:error handling

	return id;
}
