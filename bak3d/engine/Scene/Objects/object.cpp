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

#include "object.h"

using namespace std;

Object::Object(Material* material)
{
	m_material = material;

	m_vao = new VertexArray();
	m_vao->bind_object();

	// Will be handled by derived class
	m_vbo = nullptr;
	m_ebo = nullptr;
}

Object::~Object()
{
	delete m_vao;
	delete m_vbo;
	delete m_ebo;
	delete m_material;
	delete m_camera;
}

void Object::update(float dt)
{
	set_model_matrix(m_position, m_scaling, m_euler_rotation, 0.0f);
}

void Object::draw() const
{
	if (!m_material || !m_camera) return;
	
	m_material->set_mat4("projection", m_camera->get_projection_matrix());
	m_material->set_mat4("view", m_camera->get_view_matrix());
	m_material->set_mat4("model", m_model_matrix);

	m_material->apply();
}

InstancedObject::InstancedObject(Material* material) : Object(material), m_ibo(nullptr) {}

InstancedObject::~InstancedObject()
{
	delete m_ibo;
}

void InstancedObject::draw() const
{
	if (!m_material || !m_camera) return;
	
	m_material->set_mat4("projection", m_camera->get_projection_matrix());
	m_material->set_mat4("view", m_camera->get_view_matrix());

	m_material->apply();
}
