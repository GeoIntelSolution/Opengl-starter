#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include"VertexArray.h"
#include"Shader.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


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
	window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
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


	{

		float positions[16] = {
			100.0f,100.0f,0.0f,0.0f,
			200.0f,100.0f,1.0f,0.0f,
			200.0f,200.0f,1.0f,1.0f,
			100.0f,200.0f,0.0f,1.0f
		};

		unsigned int indices[] = {
			0,1,2,
			2,3,0
		};

		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		GLCall(glEnable(GL_BLEND));

		VertexArray va;

		VertexBuffer vbo(positions,16* sizeof(float));




		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vbo, layout);

		IndexBuffer  ibo(indices, 6);



		//ortho projection,used in  2d
		glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f); //4:3

		glm::vec4 vp(100.0f, 100.f, 0.0f, 1.0f);

		glm::vec4 result = proj * vp;
		//perpsecptive projection


		//glm::mat4 proj = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -1.0f, 1.0f);
		Shader shader("res/shaders/Basic.shader");
		shader.Bind();

		shader.SetUniform4f("u_Color", 0.5f, 0.3f, 0.9f, 1.0f);
		shader.SetUniformMat4("u_MVP", proj);


		float r = 0.0f;
		float increment = 0.05f;

		Texture texture("res/textures/e.png");

		texture.Bind();
		shader.SetUniform1i("u_Texture", 0);


		va.UnBind();
		vbo.UnBind();
		ibo.UnBind();
		shader.UnBind();

		Renderer renderer;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();
			shader.Bind();
			shader.SetUniform4f("u_Color", r, 0.3f, 0.9f, 1.0f);
			if (r > 1.0f) {
				increment = -0.05f;
			}
			else if (r < 0.0f) {
				increment = 0.05f;
			}

			r += increment;

			renderer.Draw(va, ibo, shader);

			//glDrawArrays(GL_TRIANGLES, 0, 3);
			//glDrawElements(GL_TRIANGLES,3,)
			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
		
	}
	glfwTerminate();
	return 0;
}