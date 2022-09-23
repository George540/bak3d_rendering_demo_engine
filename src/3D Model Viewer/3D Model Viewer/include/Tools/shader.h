#ifndef SHADER_H
#define SHADER_H
#include <string>
#include <glad/glad.h> 
#include <glm/fwd.hpp>

using namespace std;

class Shader
{
	unsigned int id;
	static void check_compile_errors(unsigned int shader, std::string type);
public:
    // Constructor
    Shader(const char* vertexShaderSource, const char* fragmentShaderSource);
    ~Shader() = default;

    // Core functions
    unsigned int get_id() const { return id; }
    void use() const;

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