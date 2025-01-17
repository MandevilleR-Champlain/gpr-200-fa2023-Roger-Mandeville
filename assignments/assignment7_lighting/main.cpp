#include <stdio.h>
#include <math.h>
#include <string>
#include <sstream>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <ew/camera.h>
#include <ew/cameraController.h>

#define _USE_MATH_DEFINES

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

constexpr int MAX_LIGHTS = 4;

struct Light
{
	ew::Vec3 positon;
	ew::Vec3 color = ew::Vec3(1.f);
};

void renderLight(const Light& light, ew::Shader& emissiveShader, const ew::Mesh& lightMesh)
{
	ew::Transform lightTransform;
	lightTransform.position = light.positon;

	emissiveShader.setMat4("_Model", lightTransform.getModelMatrix());
	emissiveShader.setVec3("_lightColor", light.color);
	lightMesh.draw();
}

float prevTime;
ew::Vec3 bgColor = ew::Vec3(0.1f);

ew::Camera camera;
ew::CameraController cameraController;

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Global settings
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("assets/defaultLit.vert", "assets/defaultLit.frag");
	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg",GL_REPEAT,GL_LINEAR);

	ew::Shader emissiveShader("assets/emissive.vert", "assets/emissive.frag");

	//Create cube
	ew::Mesh cubeMesh(ew::createCube(1.0f));
	ew::Mesh planeMesh(ew::createPlane(5.0f, 5.0f, 10));
	ew::Mesh sphereMesh(ew::createSphere(0.5f, 64));
	ew::Mesh cylinderMesh(ew::createCylinder(0.5f, 1.0f, 32));

	//Initialize transforms
	ew::Transform cubeTransform;
	ew::Transform planeTransform;
	ew::Transform sphereTransform;
	ew::Transform cylinderTransform;
	planeTransform.position = ew::Vec3(0, -1.0, 0);
	sphereTransform.position = ew::Vec3(-1.5f, 0.0f, 0.0f);
	cylinderTransform.position = ew::Vec3(1.5f, 0.0f, 0.0f);

	ew::Mesh lightMesh(ew::createSphere(0.3f, 12));

	bool animateLights = true;
	int activeLights = MAX_LIGHTS;
	float lightOrbitRadius = 3.f;
	float lightOrbitSpeed = 1.f;
	float lightHeight = 3.f;
	Light lights[MAX_LIGHTS] =
	{
		Light{ew::Vec3(0.f, lightHeight, lightOrbitRadius), ew::Vec3(1.f, 0.f, 0.f)},
		Light{ew::Vec3(lightOrbitRadius, lightHeight, 0.f), ew::Vec3(0.f, 1.f, 0.f)},
		Light{ew::Vec3(0.f, lightHeight, -lightOrbitRadius), ew::Vec3(0.f, 0.f, 1.f)},
		Light{ew::Vec3(-lightOrbitRadius, lightHeight, 0.f), ew::Vec3(1.f, 1.f, 0)}
	};

	//Material properties
	float ambientK = 0.2f;
	ew::Vec3 ambientColor = ew::Vec3(0.341f, 0.365f, 0.51f);
	float diffuseK = 0.4f;
	float specularK = 0.5f;
	float shininess = 10.f;


	resetCamera(camera,cameraController);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		//Update camera
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		cameraController.Move(window, &camera, deltaTime);

	
		glClearColor(bgColor.x, bgColor.y,bgColor.z,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glBindTexture(GL_TEXTURE_2D, brickTexture);
		shader.setInt("_Texture", 0);
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

	
		shader.setInt("_activeLights", activeLights);
		for (int i = 0; i < activeLights; i++)
		{
			std::stringstream lightPositionStream;
			lightPositionStream << "_lights[" << i << "].position";
			std::stringstream lightColorStream;
			lightColorStream << "_lights[" << i << "].color";

			shader.setVec3(lightPositionStream.str(), lights[i].positon);
			shader.setVec3(lightColorStream.str(), lights[i].color);
		}


		shader.setMat4("_Model", cubeTransform.getModelMatrix());
		cubeMesh.draw();

		shader.setMat4("_Model", planeTransform.getModelMatrix());
		planeMesh.draw();

		shader.setMat4("_Model", sphereTransform.getModelMatrix());
		sphereMesh.draw();

		shader.setMat4("_Model", cylinderTransform.getModelMatrix());
		cylinderMesh.draw();

		//Set material/light props
		shader.setFloat("_material.ambientK", ambientK);
		shader.setVec3("_ambientColor", ambientColor);
		shader.setFloat("_material.diffuseK", diffuseK);
		shader.setFloat("_material.specularK", specularK);
		shader.setFloat("_material.shininess", shininess);

		//Render point lights
		//Setup emissive shader
		emissiveShader.use();
		emissiveShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
		//Render all lights
		for (int i = 0; i < activeLights; i++)
		{
			renderLight(lights[i], emissiveShader, lightMesh);
		}

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("Position", &camera.position.x, 0.1f);
				ImGui::DragFloat3("Target", &camera.target.x, 0.1f);
				ImGui::Checkbox("Orthographic", &camera.orthographic);
				if (camera.orthographic) {
					ImGui::DragFloat("Ortho Height", &camera.orthoHeight, 0.1f);
				}
				else {
					ImGui::SliderFloat("FOV", &camera.fov, 0.0f, 180.0f);
				}
				ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Move Speed", &cameraController.moveSpeed, 0.1f);
				ImGui::DragFloat("Sprint Speed", &cameraController.sprintMoveSpeed, 0.1f);
				if (ImGui::Button("Reset")) {
					resetCamera(camera, cameraController);
				}
			}

			ImGui::ColorEdit3("BG color", &bgColor.x);
			ImGui::End();
			
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 5);
	camera.target = ew::Vec3(0);
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}


