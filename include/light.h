#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shader.h>


class Light {
public:
    glm::vec3 color;
    float intensity;
    Light(): color(1.0f), intensity(1.0f){
    }
public:
    virtual void SetShader(Shader& shader, const char* name) {
        shader.use();
        string res_name(name);
        shader.setFloat(res_name + ".intensity", intensity);
        shader.setVec3(res_name + ".color", color);
    }
};



class DirectionLight : public Light{
public:
    glm::vec3 direction;
    DirectionLight(): Light(), direction(1.0)
    {
    }

    void SetShader(Shader& shader, const char* name) override {
        Light::SetShader(shader, name);
        string res_name(name);
        shader.setVec3(res_name + ".direction", direction);
    }

    void test(Shader& shader, const char* name) {
        shader.use();
        string res_name(name);
        shader.setFloat(res_name + ".intensity", intensity);
        shader.setVec3(res_name + ".color", color);
        shader.setVec3(res_name + ".direction", direction);
    }
};


// attenuation = 1.0f / (constant + linear * distance  + quadratic * (distance * distance))
class PointLight : public Light {
public:
    glm::vec3 position;
    float constant;
    float linear;
    float quadratic;

    PointLight(): position(0.0, 0.0, 0.0), constant(0.1f), linear(0.2f), quadratic(0.3f) {

    }

    void SetShader(Shader& shader, const char* name) override {
        Light::SetShader(shader, name);
        string res_name(name);
        shader.setVec3(res_name + ".position", position);
        shader.setFloat(res_name + ".constant", constant);
        shader.setFloat(res_name + ".linear", linear);
        shader.setFloat(res_name + ".quadratic", quadratic);
    }
};

class SpotLight : public PointLight {
public:
    glm::vec3 direction;

    float inner_angle_cos;
    float outter_angle_cos;

    SpotLight(): direction(1.0f),
                inner_angle_cos(cos(15.0f)),
                outter_angle_cos(cos(25.0f))
    {
    
    }

    void SetShader(Shader& shader, const char* name) override {
        Light::SetShader(shader, name);
        string res_name(name);
        shader.setVec3(res_name + ".direction", direction);
        shader.setFloat(res_name + "inner_angle_cos", inner_angle_cos);
        shader.setFloat(res_name + "outter_angle_cos", outter_angle_cos);
    }
};


#endif