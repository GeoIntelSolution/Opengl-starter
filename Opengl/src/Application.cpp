#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
		x;\
		ASSERT(GLLogCall(#x,__FILE__,__LINE__))

static void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}


static bool GLLogCall(const char* function ,const char* file,int line) {
	while (GLenum error = glGetError()) {
		std::cout << "error<<(" << error << "):" <<function
			<<"  "<<file<<":"<<line
			<< std::endl;
		return false;
	}
	return true;
}

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

	
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);


	if (glewInit() != GLEW_OK) {
		std::cout << "Error" << std::endl;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	unsigned int a ;
	glGenBuffers(1, &a);

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

	glBindBuffer(GL_ARRAY_BUFFER, a);
	glBufferData(GL_ARRAY_BUFFER,8*sizeof(float),positions,GL_STATIC_DRAW);
	




	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);


	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	std::cout << source.VertexSource << std::endl;
	std::cout << source.FragmentSource << std::endl;

	unsigned int program = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(program);


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		//glDrawArrays(GL_TRIANGLES, 0, 3);
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr));
		//glDrawElements(GL_TRIANGLES,3,)
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	glDeleteProgram(program);
	glfwTerminate();
	return 0;
}