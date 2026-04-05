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

#include "shader.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <glm/glm.hpp>

#include "Core/logger.h"

using namespace std;

Shader::Shader() :
    Shader(filesystem::absolute("shaders/LineShader.vert").string().c_str(),
        filesystem::absolute("shaders/LineShader.frag").string().c_str(),
        "LineShader")
{
    
}

Shader::Shader(const char* vertex_shader_source, const char* fragment_shader_source, string shader_name) : Asset(vertex_shader_source, shader_name)
{
    m_index = 0;
    m_vert_path = vertex_shader_source;
    m_frag_path = fragment_shader_source;
    
    // 1. retrieve the vertex/fragment source code from filePath
	string vertexCode;
    string fragmentCode;
    ifstream vShaderFile;
    ifstream fShaderFile;

    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
    fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

    try
    {
        // open files
        vShaderFile.open(vertex_shader_source);
        fShaderFile.open(fragment_shader_source);
        stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        B3D_LOG_ERROR("Shader file not successfully read: %s", e.what());
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    GLuint vertex, fragment;

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    check_compile_errors(vertex, "VERTEX");

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    check_compile_errors(fragment, "FRAGMENT");

    // shader Program
    m_object_id = glCreateProgram();
    glAttachShader(m_object_id, vertex);
    glAttachShader(m_object_id, fragment);
    glLinkProgram(m_object_id);
    check_compile_errors(m_object_id, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    B3D_LOG_INFO("Shader %s with ID %d has compiled.", m_object_name.c_str(), m_object_id);
}

Shader::Shader(const Shader& otherShader) : Shader(otherShader.get_vert_path(), otherShader.get_frag_path(), otherShader.get_object_name())
{
    
}

Shader::~Shader()
{
    cout << "Shader " << m_object_name << " with ID " << m_object_id << " has been deleted..." << endl;
}

void Shader::use() const
{
    glUseProgram(m_object_id);
}

void Shader::unuse() const
{
    glUseProgram(0);
}

void Shader::set_bool(const string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_object_id, name.c_str()), static_cast<int>(value));
}

void Shader::set_uint(const string& name, unsigned int value) const
{
    glUniform1ui(glGetUniformLocation(m_object_id, name.c_str()), value); // Warning: DO NOT use for texture samplers. Use integer instead
}

void Shader::set_int(const string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_object_id, name.c_str()), value);
}

void Shader::set_float(const std::string& name, GLfloat value) const
{
	glUniform1f(glGetUniformLocation(m_object_id, name.c_str()), value);
}

void Shader::set_vec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(m_object_id, name.c_str()), 1, &value[0]);
}

void Shader::set_vec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(m_object_id, name.c_str()), x, y);
}

void Shader::set_vec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(m_object_id, name.c_str()), 1, &value[0]);
}

void Shader::set_vec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(m_object_id, name.c_str()), x, y, z);
}

void Shader::set_vec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(m_object_id, name.c_str()), 1, &value[0]);
}

void Shader::set_vec4(const std::string& name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(m_object_id, name.c_str()), x, y, z, w);
}

void Shader::set_mat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(m_object_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_object_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_object_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::check_compile_errors(unsigned int shader, const string& type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            B3D_LOG_ERROR("Shader compilation error of type: %s\n %s\n -- --------------------------------------------------- -- ", type.c_str(), infoLog);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            B3D_LOG_ERROR("Shader linking error of type: %s\n %s\n -- --------------------------------------------------- -- ", type.c_str(), infoLog);
        }
    }
}