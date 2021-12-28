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
		// PS ALWASY PASS OBJECT SA CONST REFERENCE ! ALWAYS BETTER FOR OPTIMISATION
		
		// Getter Function
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width),static_cast<uint32_t>(height) }; }
		// VkExtend is just a struct with unassigned widht and height 

		bool shouldClose() { return glfwWindowShouldClose(window); } // in lining is the idea of putting everything here in one line , as it is pretty short

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		
		void initWindows();
		const int width;
		const int height;

		std::string windowname;
		GLFWwindow* window;

};


}// namespace lve