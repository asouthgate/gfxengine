#include "window.hpp"
#include <iostream>

Window::Window(int WIN_WIDTH_, int WIN_HEIGHT_, Camera* camera_ptr_) :  WIN_WIDTH(WIN_WIDTH_), WIN_HEIGHT(WIN_HEIGHT_), camera_ptr(camera_ptr_) {
	glfwInit();
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        //***** Window init
        window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "",  NULL, NULL);

        if (window == NULL) {
                std::cout << "Error: GLFW window init failed. Terminating." << std::endl;
                glfwTerminate();
        }
	else {
		std::cout << "GLFW window initialized." << std::endl;
	}
        glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::swap_buffers() {
	glfwSwapBuffers(window);
	glfwPollEvents();
}

bool Window::should_close() {
	return glfwWindowShouldClose(window);
}

void Window::close() {
	glfwTerminate();
}

void Window::process_input(float dT) {
	float val = 0.01f;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {

        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                camera_ptr->ProcessKeyboard(FORWARD, dT);

        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                camera_ptr->ProcessKeyboard(LEFT, dT);
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                camera_ptr->ProcessKeyboard(BACKWARD, dT);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                camera_ptr->ProcessKeyboard(RIGHT, dT);
        }
	glfwGetCursorPos(window, &mouse_x, &mouse_y);
	camera_ptr->ProcessMouseMovement(mouse_x, mouse_y);
}
