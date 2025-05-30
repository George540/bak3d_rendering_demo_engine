// =====================================================================================
// This shader class is referenced from https://learnopengl.com by Joey de Vries
// Only changes made were attribute naming format and more modern C++ syntax
// Joe de Vries on twitter: https://twitter.com/JoeyDeVriez
// =====================================================================================

#pragma once

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> 
#include <glm/fwd.hpp>
#include <string>

#include "asset.h"

class Shader : public Asset
{
private:
	const char* m_vert_path;
	const char* m_frag_path;
	
	GLuint m_index;
	static void check_compile_errors(unsigned int shader, const std::string& type);
public:
	// Constructor
	Shader();
	Shader(const char* vertex_shader_source, const char* fragment_shader_source, std::string shader_name);
	Shader(const Shader& otherShader);
	~Shader() override;

	// Core functions
	[[nodiscard]] GLuint get_index() const { return m_index; }
	[[nodiscard]] const char* get_vert_path() const { return m_vert_path; }
	[[nodiscard]] const char* get_frag_path() const { return m_frag_path; }
	void set_index(int i) { m_index = i; }
	void use() const;
	void unuse() const;

	// Utility Functions
	void set_bool(const std::string& name, bool value) const;
	void set_uint(const std::string& name, unsigned int value) const;
	void set_int(const std::string& name, int value) const;
	void set_float(const std::string& name, GLfloat value) const;

	void set_vec2(const std::string& name, const glm::vec2& value) const;
	void set_vec2(const std::string& name, float x, float y) const;
	void set_vec3(const std::string& name, const glm::vec3& value) const;
	void set_vec3(const std::string& name, float x, float y, float z) const;
	void set_vec4(const std::string& name, const glm::vec4& value) const;
	void set_vec4(const std::string& name, float x, float y, float z, float w) const;
	void set_mat2(const std::string& name, const glm::mat2& mat) const;
	void set_mat3(const std::string& name, const glm::mat3& mat) const;
	void set_mat4(const std::string& name, const glm::mat4& mat) const;
};

#endif