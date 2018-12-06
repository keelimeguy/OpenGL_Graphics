#include "TestTexture2d.h"

#include "Renderer.h"

#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
namespace test {
	TestTexture2d::TestTexture2d()
		: m_translation(0, 0, 0), m_translation_static(320, 240, 0)
	{
		m_r = 0.0f;
		m_increment = 0.05f;

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

		// Vertex Buffer needs to remain in memory!
		// - Or else when it loses scope and deletes itself,
		// 	the GPU will delete its copy of the buffer as well!
		m_vb = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);

		m_va = std::make_unique<VertexArray>();
		m_va->AddBuffer(*m_vb, layout);

		m_ib = std::make_unique<IndexBuffer>(indices, 6);

		m_shader = std::make_unique<Shader>("res/shaders/Basic.shader");
		m_shader->Bind();

		m_texture = std::make_unique<Texture>("res/textures/star.png");
		m_texture->Bind();

		m_shader->SetUniform1i("u_Texture", 0);
	}

	void TestTexture2d::OnUpdate(float deltaTime) {
		m_r += m_increment;
		if (m_r > 1.0f) {
			m_increment = -0.05f;
			m_r = 1.0f;
		} else if (m_r < 0.0f) {
			m_increment = 0.05f;
			m_r = 0.0f;
		}
	}

	void TestTexture2d::OnRender() {
		Renderer renderer;

		// projection: screen transform (converts into -1 to 1 space)
		glm::mat4 proj = glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, -1.0f, 1.0f);
									// -x    x     -y     y     -z     z
		// view: camera transform
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCALL(glClear(GL_COLOR_BUFFER_BIT));

		m_texture->Bind();

		m_shader->SetUniform4f("u_Color", m_r, 1.0f - m_r, 0.0f, 1.0f);
		{
			// model: object transform
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_translation_static);
			glm::mat4 mvp = proj * view * model;
			m_shader->Bind();
			m_shader->SetUniformMat4f("u_MVP", mvp);
			renderer.Draw(*m_va, *m_ib, *m_shader);
		}

		{
			// model: object transform
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_translation);
			glm::mat4 mvp = proj * view * model;
			m_shader->Bind();
			m_shader->SetUniformMat4f("u_MVP", mvp);
			renderer.Draw(*m_va, *m_ib, *m_shader);
		}
	}

	void TestTexture2d::OnImGuiRender() {
		ImGui::SliderFloat3("Translation", &m_translation.x, 0.0f, 480.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}
