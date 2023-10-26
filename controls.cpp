// Include GLFW
#include <glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}

glm::vec3 origin = glm::vec3(0, 0, 0);

// Initial horizontal angle : toward -Z
float phi = glm::radians(0.0f);
// Initial vertical angle : none
float theta = glm::radians(90.0f);
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second

float r = 10.0f;


//glm::vec3 position = glm::vec3( 0, 0, 5 ); 


void computeMatricesFromInputs() {

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// 'w' key is pressed, move the camera closer to the origin
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		// Move closer to origin (decrease r)
		r -= deltaTime * speed;
		if (r < 0.1f) {
			r = 0.1f;  // Prevent getting too close to the origin
		}
	}
	// 's' key is pressed, move the camera farther from the origin
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		// Move away from origin (increase r)
		r += deltaTime * speed;
	}
	// Rotate left, maintaining the radial distance from the origin
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		phi -= deltaTime * speed;
		if (phi < 0.0f) {
			phi += glm::two_pi<float>();
		}
		theta = glm::radians(90.0f); // Reset vertical angle to 90 degrees
	}
	// Rotate right, maintaining the radial distance from the origin
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		phi += deltaTime * speed;
		if (phi > glm::two_pi<float>()) {
			phi -= glm::two_pi<float>();
		}
		theta = glm::radians(90.0f); // Reset vertical angle to 90 degrees
	}

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		// Move up (decrease theta)
		theta -= deltaTime * speed;
		//if (theta < 0.0f) {
		//	theta = 0.0f;  // Ensure theta stays within bounds
		//}
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		theta += deltaTime * speed;
		//if (theta > glm::radians(180.0f)) {
		//	theta = glm::radians(180.0f);  
		//}
	}



	// camera position based on r, theta, and phi
	glm::vec3 position(
		r * sin(theta) * cos(phi), //x
		r * sin(theta) * sin(phi), //y
		r * cos(theta) //z
	);

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,           // Camera is here
		origin, // Always look at the origin
		glm::vec3(0, 0, 1) // Head is up (set to 0,-1,0 to look upside-down)
	);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}