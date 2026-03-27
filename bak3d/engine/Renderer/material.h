#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include <unordered_map>
#include <string>

#include <glad/glad.h> 
#include <glm/fwd.hpp>
#include <glm/glm.hpp> 

class Shader;
class Texture2D;

class Material
{
    Shader* m_shader;

    // Single value-type uniforms
    std::unordered_map<std::string, bool> m_uniform_flags;
    std::unordered_map<std::string, GLint> m_int_uniforms;
    std::unordered_map<std::string, GLuint> m_uint_uniforms;
    std::unordered_map<std::string, GLfloat> m_float_uniforms;

    // Multi vector-type uniforms
    std::unordered_map<std::string, glm::vec2> m_vec2_uniforms;
    std::unordered_map<std::string, glm::vec3> m_vec3_uniforms;
    std::unordered_map<std::string, glm::vec4> m_vec4_uniforms;
    std::unordered_map<std::string, glm::mat2> m_mat2_uniforms;
    std::unordered_map<std::string, glm::mat3> m_mat3_uniforms;
    std::unordered_map<std::string, glm::mat4> m_mat4_uniforms;

public:
    Material() = default;
    Material(Shader* shader);

    Shader* get_shader() const { return m_shader; }

    void apply();

    void set_bool(const std::string& key_name, const bool value) { m_uniform_flags[key_name] = value; }
    void set_uint(const std::string& key_name, const unsigned int value) { m_uint_uniforms[key_name] = value; }
    void set_int(const std::string& key_name, const int value) { m_int_uniforms[key_name] = value; }
    void set_float(const std::string& key_name, const float value) { m_float_uniforms[key_name] = value; }

    void set_vec2(const std::string& key_name, const glm::vec2& value) { m_vec2_uniforms[key_name] = value; }
    void set_vec3(const std::string& key_name, const glm::vec3& value) { m_vec3_uniforms[key_name] = value; }
    void set_vec4(const std::string& key_name, const glm::vec4& value) { m_vec4_uniforms[key_name] = value; }

    void set_mat2(const std::string& key_name, const glm::mat2& value) { m_mat2_uniforms[key_name] = value; }
    void set_mat3(const std::string& key_name, const glm::mat3& value) { m_mat3_uniforms[key_name] = value; }
    void set_mat4(const std::string& key_name, const glm::mat4& value) { m_mat4_uniforms[key_name] = value; }

    void set_texture_id(const std::string& key_name, const unsigned int value) { m_uint_uniforms[key_name] = value; }
};

#endif