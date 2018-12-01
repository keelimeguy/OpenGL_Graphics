// Useful documentation website: docs.gl

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>

int main() {
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
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
			-50.0f, -50.0f, 0.0f, 0.0f, // 0
			50.0f, -50.0f, 1.0f, 0.0f,  // 1
			50.0f, 50.0f, 1.0f, 1.0f,   // 2
			-50.0f, 50.0f, 0.0f, 1.0f,  // 3
		};

		// Index buffer for square (two triangles)
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		GLCALL(glEnable(GL_BLEND));
		GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		VertexArray va;
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		// projection: screen transform (converts into -1 to 1 space)
		glm::mat4 proj = glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, -1.0f, 1.0f);
									// -x    x     -y     y     -z     z
		// view: camera transform
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();

		Texture texture("res/textures/star.png");
		texture.Bind();
		shader.SetUniform1i("u_Texture", 0);

		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();
		texture.Unbind();

		Renderer renderer;

		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		ImGui::StyleColorsDark();

		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		glm::vec3 translation(0, 0, 0);
		glm::vec3 translation_static(320, 240, 0);

		float r = 0.0f;
		float increment = 0.05f;
		while (!glfwWindowShouldClose(window)) {
			renderer.Clear();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			texture.Bind();
			shader.Bind();
			shader.SetUniform4f("u_Color", r, 1.0f - r, 0.0f, 1.0f);

			{
				// model: object transform
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translation_static);
				glm::mat4 mvp = proj * view * model;
				shader.Bind();
				shader.SetUniformMat4f("u_MVP", mvp);

				renderer.Draw(va, ib, shader);
			}

			{
				// model: object transform
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
				glm::mat4 mvp = proj * view * model;
				shader.Bind();
				shader.SetUniformMat4f("u_MVP", mvp);

				renderer.Draw(va, ib, shader);
			}

			r += increment;
			if (r > 1.0f) {
				increment = -0.05f;
				r = 1.0f;
			}
			else if (r < 0.0f) {
				increment = 0.05f;
				r = 0.0f;
			}

			{
				static float f = 0.0f;
				ImGui::Begin("Debug");
				ImGui::SliderFloat3("Translation", &translation.x, 0.0f, 480.0f);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}
