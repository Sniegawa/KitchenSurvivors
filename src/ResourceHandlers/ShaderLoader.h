#pragma once


#include<GL/glew.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include<string>
#include <variant>
#include <unordered_map>
class Shader
{

public:

    //using UniformValue = std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;

	unsigned int ID = 0;
	Shader() {}
	Shader& Use();
	void Compile(const char* vertex_source, const char* fragment_source);

    //void    SetUniform(const char* name, UniformValue value, bool useShader = false);

    void    SetUniform(const char* name, float value, bool useShader = false);
    void    SetUniform(const char* name, int value, bool useShader = false);
    void    SetUniform(const char* name, float x, float y, bool useShader = false);
    void    SetUniform(const char* name, const glm::vec2& value, bool useShader = false);
    void    SetUniform(const char* name, float x, float y, float z, bool useShader = false);
    void    SetUniform(const char* name, const glm::vec3& value, bool useShader = false);
    void    SetUniform(const char* name, float x, float y, float z, float w, bool useShader = false);
    void    SetUniform(const char* name, const glm::vec4& value, bool useShader = false);
    void    SetUniform(const char* name, const glm::mat4& matrix, bool useShader = false);

    void    SetFloat(const char* name, float value, bool useShader = false);
    void    SetInteger(const char* name, int value, bool useShader = false);
    void    SetVector2f(const char* name, float x, float y, bool useShader = false);
    void    SetVector2f(const char* name, const glm::vec2& value, bool useShader = false);
    void    SetVector3f(const char* name, float x, float y, float z, bool useShader = false);
    void    SetVector3f(const char* name, const glm::vec3& value, bool useShader = false);
    void    SetVector4f(const char* name, float x, float y, float z, float w, bool useShader = false);
    void    SetVector4f(const char* name, const glm::vec4& value, bool useShader = false);
    void    SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader = false);

    bool checkCompileErrors(unsigned int object, std::string type);

    bool operator<(const Shader& shader) const noexcept
    {
        return this->ID < shader.ID;
    }

    template<typename T>
    void UpdateSSBO(const std::string& bufferName, const T& data, GLuint bindingPoint);

private:
    std::unordered_map<std::string, GLuint> m_SSBOLocations;
    GLuint GetSSBO(const std::string& name);

    void CreateSSBO(GLuint& ssbo, const std::string& name, GLuint bindingPoint);
};


class ComputeShader 
{
public:
    unsigned int ID;
    ComputeShader() { this->ID = 0; }
    ComputeShader& Use();

    void Compile(const char* compute_source);

    void    SetUniform(const char* name, float value, bool useShader = false);
    void    SetUniform(const char* name, int value, bool useShader = false);
    void    SetUniform(const char* name, float x, float y, bool useShader = false);
    void    SetUniform(const char* name, const glm::vec2& value, bool useShader = false);
    void    SetUniform(const char* name, float x, float y, float z, bool useShader = false);
    void    SetUniform(const char* name, const glm::vec3& value, bool useShader = false);
    void    SetUniform(const char* name, float x, float y, float z, float w, bool useShader = false);
    void    SetUniform(const char* name, const glm::vec4& value, bool useShader = false);
    void    SetUniform(const char* name, const glm::mat4& matrix, bool useShader = false);

    bool checkCompileErrors(unsigned int object, std::string type);
};
