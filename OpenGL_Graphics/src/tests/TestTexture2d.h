#pragma once

#include "Test.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include <memory>

namespace test {
	class TestTexture2d : public Test {
	public:
		TestTexture2d();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
    private:
        glm::vec3 m_translation, m_translation_static;
        float m_r, m_increment;

        std::unique_ptr<VertexArray> m_va;
        std::unique_ptr<VertexBuffer> m_vb;
        std::unique_ptr<IndexBuffer> m_ib;
        std::unique_ptr<Shader> m_shader;
        std::unique_ptr<Texture> m_texture;
	};
}
