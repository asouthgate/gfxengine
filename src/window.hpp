#ifndef WINDOW_HPP
#define WINDOW_HPP
#include <GLFW/glfw3.h>
#include "camera.hpp"

class Window {
	public:
		Window(int WIN_WIDTH, int WIN_HEIGHT, Camera* camera_ptr);
		void close();
		bool should_close();
		void swap_buffers();	
                void process_input(float dT);

		//***** Temporary camera pointer; to be delegated elsewhere
		Camera* camera_ptr;
	
	private:
		const unsigned int WIN_WIDTH;
		const unsigned int WIN_HEIGHT;
		GLFWwindow* window;
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

		//***** Input variables
                bool first_mouse = true;
		double mouse_x = 0.0f;
		double mouse_y = 0.0f;
};

#endif
