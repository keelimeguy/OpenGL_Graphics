// Useful documentation website: docs.gl

#include <GL\glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"

#include <iostream>

int main() {
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(640, 480, "OpenGL_Testing", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		return -1;

	GLCALL(std::cout << glGetString(GL_VERSION) << std::endl);

	// We need to clean up our class objects before glfwTerminate()..
	// This scope is used to do that.
	{
		float positions[] = {
			-0.5f, -0.5f, // 0
			0.5f, -0.5f,  // 1
			0.5f, 0.5f,   // 2
			-0.5f, 0.5f,  // 3
		};

		// Index buffer for square (two triangles)
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		VertexArray va;
		VertexBuffer vb(positions, 4 * 2 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		Shader shader("res/shaders/Basic.shader");

		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		float r = 0.0f;
		float increment = 0.05f;
		while (!glfwWindowShouldClose(window)) {
			GLCALL(glClear(GL_COLOR_BUFFER_BIT));

			shader.Bind();
			shader.SetUniform4f("u_Color", r, 1.0f - r, 0.0f, 1.0f);

			va.Bind();
			ib.Bind();

			GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

			r += increment;
			if (r > 1.0f) {
				increment = -0.05f;
				r = 1.0f;
			}
			else if (r < 0.0f) {
				increment = 0.05f;
				r = 0.0f;
			}

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}
	glfwTerminate();
	return 0;
}
