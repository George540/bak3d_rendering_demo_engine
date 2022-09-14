#ifndef SHADER_H
#define SHADER_H
#include <string>
#include <glad/glad.h> 

using namespace std;

class Shader
{
	unsigned int ID;
	static void check_compile_errors(unsigned int shader, std::string type);
public:
    // Constructor
    Shader(const char* vertexShaderSource, const char* fragmentShaderSource);
    ~Shader() = default;

    // Core functions
    unsigned int get_id() const { return ID; }
    void use() const;

    // Utility Functions
    void set_bool(const std::string& name, bool value) const;
    void set_int(const std::string& name, int value) const;
    void set_float(const std::string& name, GLfloat value) const;
};

#endif