#pragma once
#include<string>
#include<unordered_map>

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};
class Shader
{
private :
	std::string m_FilePath;
	unsigned int m_RendererId;
	std::unordered_map<std::string, int> m_UniformLocationCache;
	
public:
	Shader(const std::string &filepath);
	~Shader();

	void Bind() const;
	void UnBind() const;

	//set Uniforms

	void SetUniform4f(const std::string&name, float v1, float v2, float v3, float v4);
	void SetUniform1i(const std::string &name, int v1);
private:
	unsigned int CompileShader(unsigned int type, const std::string& source);
	ShaderProgramSource ParseShader(const std::string &filePath);
	int CreateShader(const std::string& vertexShader, const std::string & fragmentShader);
	int GetUniformLocation(const std::string &name);
};

