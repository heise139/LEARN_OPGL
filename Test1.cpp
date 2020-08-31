// Test1.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include "shader.h"
#include "camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

template<typename ... Args>
string string_format(const string& format, Args ... args) {
	size_t size = 1 + snprintf(nullptr, 0, format.c_str(), args ...);  // Extra space for \0
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
	float camera_speed = 2.5f;
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
	mouse_dx = xpos - last_frame_mouse_x;
	mouse_dy = last_frame_mouse_y - ypos;
	//Log("dx %.2f, dy %.2f\n, xpos %.2f, last mouse x %.2f \n", mouse_dx, mouse_dy, xpos, last_frame_mouse_x);
	last_frame_mouse_x = xpos;
	last_frame_mouse_y = ypos;
//	printf("update mouse frame %d ", cur_frame_count);
}

unsigned int load_texture(const char* path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	int width, height, nrComponents;
	std::string rel_path;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path" << path << std::endl;
		stbi_image_free(data);
	}
	return textureID;
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
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initalize GLAD" << std::endl;
		return -1;
	}
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};
	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	int num_of_elements = 8;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, num_of_elements * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, num_of_elements * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, num_of_elements * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, num_of_elements * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	unsigned int diffuse_tex = load_texture(
									resource_path("container2.png"));
	unsigned int specular_tex = load_texture(
									resource_path("container2_specular.png"));

	float timeValue = glfwGetTime();
	Shader _shader(resource_path("vs.glsl"), resource_path("ps.glsl"));
	Shader light_shader(resource_path("lightvs.glsl"), resource_path("lightps.glsl"));

	glm::mat4 model(1.0f);

	Camera cam;
	cam.position = glm::vec3(0, 0, 3.0f);

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)
	};

	glm::vec3 point_light_positions[] = {
		glm::vec3(0.7f, 0.2f, 2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f, 2.0f, -12.0f),
		glm::vec3(0.0f, 0.0f, -3.0f),
	};

	_shader.use();
	_shader.setInt("material.diffuse", 0);
	_shader.setInt("material.specular", 1);
	_shader.setFloat("material.shininess", 32.0f);

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
		glEnable(GL_DEPTH_TEST);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuse_tex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specular_tex);

		_shader.use();
		_shader.setMat4("projection", cam.GetProjectionMat());
		_shader.setMat4("view", cam.GetViewMat());
		_shader.setVec3("view_pos", cam.position);

		// directional light
		_shader.setVec3("dir_light.direction", -0.2f, -1.0f, -0.3f);
		_shader.setVec3("dir_light.ambient", 0.05f, 0.05f, 0.05f);
		_shader.setVec3("dir_light.diffuse", 0.4f, 0.4f, 0.4f);
		_shader.setVec3("dir_light.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		_shader.setVec3("point_light[0].position", point_light_positions[0]);
		_shader.setVec3("point_light[0].ambient", 0.05f, 0.05f, 0.05f);
		_shader.setVec3("point_light[0].diffuse", 0.8f, 0.8f, 0.8f);
		_shader.setVec3("point_light[0].specular", 1.0f, 1.0f, 1.0f);
		_shader.setFloat("point_light[0].constant", 1.0f);
		_shader.setFloat("point_light[0].linear", 0.09);
		_shader.setFloat("point_light[0].quadratic", 0.032);
		// point light 2
		_shader.setVec3("point_light[1].position", point_light_positions[1]);
		_shader.setVec3("point_light[1].ambient", 0.05f, 0.05f, 0.05f);
		_shader.setVec3("point_light[1].diffuse", 0.8f, 0.8f, 0.8f);
		_shader.setVec3("point_light[1].specular", 1.0f, 1.0f, 1.0f);
		_shader.setFloat("point_light[1].constant", 1.0f);
		_shader.setFloat("point_light[1].linear", 0.09);
		_shader.setFloat("point_light[1].quadratic", 0.032);
		// point light 3
		_shader.setVec3("point_light[2].position", point_light_positions[2]);
		_shader.setVec3("point_light[2].ambient", 0.05f, 0.05f, 0.05f);
		_shader.setVec3("point_light[2].diffuse", 0.8f, 0.8f, 0.8f);
		_shader.setVec3("point_light[2].specular", 1.0f, 1.0f, 1.0f);
		_shader.setFloat("point_light[2].constant", 1.0f);
		_shader.setFloat("point_light[2].linear", 0.09);
		_shader.setFloat("point_light[2].quadratic", 0.032);
		// point light 4
		_shader.setVec3("point_light[3].position", point_light_positions[3]);
		_shader.setVec3("point_light[3].ambient", 0.05f, 0.05f, 0.05f);
		_shader.setVec3("point_light[3].diffuse", 0.8f, 0.8f, 0.8f);
		_shader.setVec3("point_light[3].specular", 1.0f, 1.0f, 1.0f);
		_shader.setFloat("point_light[3].constant", 1.0f);
		_shader.setFloat("point_light[3].linear", 0.09);
		_shader.setFloat("point_light[3].quadratic", 0.032);
		// spot_light
		_shader.setVec3("spot_light.position", cam.position);
		_shader.setVec3("spot_light.direction", cam.GetForward());
		_shader.setVec3("spot_light.ambient", 0.0f, 0.0f, 0.0f);
		_shader.setVec3("spot_light.diffuse", 1.0f, 1.0f, 1.0f);
		_shader.setVec3("spot_light.specular", 1.0f, 1.0f, 1.0f);
		_shader.setFloat("spot_light.constant", 1.0f);
		_shader.setFloat("spot_light.linear", 0.09);
		_shader.setFloat("spot_light.quadratic", 0.032);
		_shader.setFloat("spot_light.inner_angle_cos", glm::cos(glm::radians(12.5f)));
		_shader.setFloat("spot_light.outter_angle_cos", glm::cos(glm::radians(35.0f)));
		
		glBindVertexArray(VAO);
		for (unsigned int i = 0; i < 10; i++) {
			glm::mat4 model(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
			_shader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(lightVAO);
		light_shader.use();
		light_shader.setMat4("projection", cam.GetProjectionMat());
		light_shader.setMat4("view", cam.GetViewMat());
		for (unsigned int i = 0; i < 4; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, point_light_positions[i]);
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
			light_shader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(_shader.ID);

	glfwTerminate();
    return 0;
}

