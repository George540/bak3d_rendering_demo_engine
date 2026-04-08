/* ===========================================================================
The MIT License (MIT)

Copyright (c) 2022-2026 George Mavroeidis - GeoGraphics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
=========================================================================== */

#include <filesystem>

#include "renderable_object.h"

#include "Scene/scene.h"

using namespace std;

RenderableObject::RenderableObject(Material* material, const glm::vec3 position, const std::string& name)
	: SceneObject(position, name)
{
	m_material = material;
	m_visible = true;

	m_vao = new VertexArray();
	m_vao->bind_object();

	// Will be handled by derived class
	m_vbo = nullptr;
	m_ebo = nullptr;
}

RenderableObject::~RenderableObject()
{
	delete m_vao;
	delete m_vbo;
	delete m_ebo;
	delete m_material;
}

void RenderableObject::update(float dt)
{
	set_model_matrix(m_position, m_scaling, m_euler_rotation, 0.0f);
}

void RenderableObject::draw() const
{
	Camera* scene_camera = Scene::instance->get_camera();
	if (!m_material || !scene_camera) return;

	m_material->set_mat4("model", m_model_matrix);

	m_material->apply();
}

InstancedObject::InstancedObject(Material* material, const std::string& name) : RenderableObject(material, glm::vec3(0.0f, 0.0f, 0.0f), name), m_ibo(nullptr) {}

InstancedObject::~InstancedObject()
{
	delete m_ibo;
}

void InstancedObject::draw() const
{
	Camera* scene_camera = Scene::instance->get_camera();
	if (!m_material || !scene_camera) return;

	m_material->apply();
}
