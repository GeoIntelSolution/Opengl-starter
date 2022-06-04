#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};
static ShaderProgramSource ParseShader(const std::string &filePath) {
	std::ifstream stream(filePath);
	std::string line;
	enum  class ShaderType
	{
		NONE=-1,VERTEX=0,FRAGMENT=1

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
			ss[(int)type] << line<<"\n";
		}
	}

	return { ss[0].str(),ss[1].str() };


}

static unsigned int  CompileShader( unsigned int type, const std::string& source) {
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
		std::cout << "Fail o compile"<<(type==GL_VERTEX_SHADER?"vertshader":"fragmentShader")<<" shader !" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	//todo:error handling

	return id;

}

static int CreateShader(const std::string& vertexShader, const std::string & fragmentShader) {
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

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	//default profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);


	
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);


	if (glewInit() != GLEW_OK) {
		std::cout << "Error" << std::endl;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	unsigned int vao;
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));

	{

		float positions[8] = {
			-0.5f,-0.5f,
			0.5f,-0.5f,
			0.5f,0.5f,
			-0.5f,0.5f
		};

		unsigned int indices[] = {
			0,1,2,
			2,3,0
		};


		VertexBuffer vbo(positions, 4 * 2 * sizeof(float));

		IndexBuffer  ibo(indices, 6);

		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));



		ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
		std::cout << source.VertexSource << std::endl;
		std::cout << source.FragmentSource << std::endl;

		unsigned int program = CreateShader(source.VertexSource, source.FragmentSource);
		glUseProgram(program);

		int location = glGetUniformLocation(program, "u_Color");
		ASSERT(location != -1);
		glUniform4f(location, 0.5f, 0.3f, 0.9f, 1.0f);

		float r = 0.0f;
		float increment = 0.05f;


		GLCall(glBindVertexArray(0));
		GLCall(glUseProgram(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));



		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			glClear(GL_COLOR_BUFFER_BIT);
			GLCall(glUseProgram(program));
			glUniform4f(location, r, 0.3f, 0.9f, 1.0f);
			GLCall(glBindVertexArray(vao));
			//GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));


			if (r > 1.0f) {
				increment = -0.05f;
			}
			else if (r < 0.0f) {
				increment = 0.05f;
			}

			r += increment;


			//glDrawArrays(GL_TRIANGLES, 0, 3);
			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
			//glDrawElements(GL_TRIANGLES,3,)
			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
		glDeleteProgram(program);
	}
	glfwTerminate();
	return 0;
}