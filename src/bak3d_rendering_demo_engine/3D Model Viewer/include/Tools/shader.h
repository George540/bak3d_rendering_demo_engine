// =====================================================================================
// This shader class is referenced from https://learnopengl.com by Joey de Vries
// Only changes made were attribute naming format and more modern C++ syntax
// Joe de Vries on twitter: https://twitter.com/JoeyDeVriez
// =====================================================================================

#pragma once

#ifndef SHADER_H
#define SHADER_H
#include <string>
#include <glad/glad.h> 
#include <glm/fwd.hpp>

using namespace std;

class Shader
{
	GLuint m_id;
	GLuint m_index;
	std::string m_name;
	static void check_compile_errors(unsigned int shader, const std::string& type);
public:
	// Constructor
	Shader();
	Shader(const char* vertex_shader_source, const char* fragment_shader_source, string shader_name);
	~Shader();

	// Core functions
	[[nodiscard]] GLuint get_id() const { return m_id; }
	[[nodiscard]] GLuint get_index() const { return m_index; }
	[[nodiscard]] std::string get_name() const { return m_name; }
	void set_index(int i) { m_index = i; }
	void set_name(std::string name) { m_name = name; }
	void use() const;
	void unuse() const;

	// Utility Functions
	void set_bool(const std::string& name, bool value) const;
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