// Test1.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include "shader.h"
#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "model.h"
#include "light.h"
using namespace std;

template<typename ... Args>
string string_format(const string& format, Args ... args) {
	size_t size = (size_t)1 + snprintf(nullptr, 0, format.c_str(), args ...);  // Extra space for \0
	// unique_ptr<char[]> buf(new char[size]);
	char* bytes = new char[size];
	snprintf(bytes, size, format.c_str(), args ...);
	string result(bytes);
	delete[] bytes;
	return result;
}
#define resource_path(x) string_format("resources/%s", x).c_str()


const unsigned int SRC_WIDTH = 800;
const unsigned int SRC_HEIGHT = 600;

float last_frame_mouse_x = SRC_WIDTH / 2.0f;
float last_frame_mouse_y = SRC_HEIGHT / 2.0f;

float mouse_dx = 0.0f;
float mouse_dy = 0.0f;


float last_frame_time = 0;
int cur_frame_count = 0;


void framebuffer_size_cb(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, Camera& cam, float dt)
{
	glm::vec3 forward = cam.GetForward();
	float camera_speed = 5.0f;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.position += camera_speed * forward * dt;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.position -= camera_speed * forward * dt;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.position -= glm::normalize(glm::cross(forward, cam.up)) * camera_speed * dt;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.position += glm::normalize(glm::cross(forward, cam.up)) * camera_speed * dt;
}


void reset_dx_dy() {
	mouse_dx = 0;
	mouse_dy = 0;
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	mouse_dx = (float)xpos - last_frame_mouse_x;
	mouse_dy = last_frame_mouse_y - (float)ypos;
	//Log("dx %.2f, dy %.2f\n, xpos %.2f, last mouse x %.2f \n", mouse_dx, mouse_dy, xpos, last_frame_mouse_x);
	last_frame_mouse_x = (float)xpos;
	last_frame_mouse_y = (float)ypos;
//	printf("update mouse frame %d ", cur_frame_count);
}


void update_cam_pitch_yaw(Camera& cam) {
	float sensitivity = 0.05f;
	cam.yaw += mouse_dx * sensitivity;
	cam.pitch += mouse_dy * sensitivity;
//	printf("dx %.5f, dy %.5f  pitch %.5f , yaw %.5f, frame_count %d\n", mouse_dx, mouse_dy, cam.pitch, cam.yaw, cur_frame_count);
	if (cam.pitch > 89.9f)
		cam.pitch = 89.9f;
	if (cam.pitch < -89.9f)
		cam.pitch = -89.9f;
}


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOP", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window " << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_cb);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initalize GLAD" << std::endl;
		return -1;
	}
	stbi_set_flip_vertically_on_load(true);
	glEnable(GL_DEPTH_TEST);


	Shader _shader(resource_path("shader/vs.glsl"), resource_path("shader/ps.glsl"));

	Camera cam;
	cam.position = glm::vec3(0, 0, 3.0f);

	Model ourModel(resource_path("nanosuit/nanosuit.obj"));
	DirectionLight dir_light;
	dir_light.color = glm::vec3(1.0f, 1.0f, 1.0f);
	dir_light.direction = glm::vec3(0, -1.0f, 0);
	dir_light.intensity = 1.0f;
	dir_light.SetShader(_shader, "dir_light");

	last_frame_time = glfwGetTime();
	float frame_rate = 1 / 60.0f;

	while (!glfwWindowShouldClose(window)) {
		float dt = glfwGetTime() - last_frame_time;
		if (dt < frame_rate)
			continue;
		cur_frame_count += 1;
		last_frame_time = glfwGetTime();
		processInput(window, cam, dt);
		update_cam_pitch_yaw(cam);
		reset_dx_dy();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		_shader.use();
		_shader.setMat4("projection", cam.GetProjectionMat());
		_shader.setMat4("view", cam.GetViewMat());

		glm::mat4 model = glm::mat4(1.0f);
		_shader.setMat4("model", model);
		_shader.setVec3("view_pos", cam.position);
		ourModel.Draw(_shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteProgram(_shader.ID);

	glfwTerminate();
    return 0;
}

