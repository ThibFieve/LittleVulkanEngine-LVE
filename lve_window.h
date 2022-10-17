// Little vulkan engine window. hpp
#pragma once


#define GLFW_INCLUDE_VULKAN  // we define  this to aslo include the vulkan headers
#include <GLFW/glfw3.h>
#include <string> // to have window name


namespace lve {

	class LveWindow {


	public:
		LveWindow(int w, int h, std::string name); // constructor
		~LveWindow(); // destructor for when we done with it

		//We will delete the copy construcor and copy destroctor from our window class , bc we use a pointer to the glfw window
		LveWindow(const LveWindow&) = delete; //https://www.youtube.com/watch?v=BvR1Pgzzr38
		LveWindow& operator = (const LveWindow&) = delete;
		// see shallow and deep copies , 
		// PS ALWAYS PASS OBJECT AS CONST REFERENCE ! ALWAYS BETTER FOR OPTIMISATION
		
		// Getter Function
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width),static_cast<uint32_t>(height) }; }
		// VkExtend is just a struct with unassigned widht and height 

		bool shouldClose() { return glfwWindowShouldClose(window); } // in lining is the idea of putting everything here in one line , as it is pretty short

		bool wasWindowResized() { return framebufferResized; };  // Helper Function for resizing
		void resetWindowResizedFlag() { framebufferResized = false; }// Helper Function for resizing

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		// This static means it can only been seen from inside this translation unit (lve_window.h + lve_window.cpp)

		void initWindows();
		int width; // Not const anymore as window becomes rezisable
		int height;
		bool framebufferResized = false; //this is to keep track if the window has been resized
		// and need to create a new SwapChain

		std::string windowname;
		GLFWwindow* window;

};


}// namespace lve