// Create the window using glfw , and all the handling around it 


#include "lve_window.h"
//std
#include <stdexcept> // for runtime error 

namespace lve {

	LveWindow::LveWindow(int w, int h, std::string name) : width{w},height{h},windowname{name} // https://www.youtube.com/watch?v=1nfuYMXjZsA
	{
		// consturctor 
		// 
		// 
		// this doesnt work as windth and heigh are const
		//width = w;
	   // height = h;
		//windowname = name;

		initWindows();

	}
	LveWindow::~LveWindow()
	{//destructor of the resources we used
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void LveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) // if is not a success
		{
			throw std::runtime_error("failed to create a window surface");  // throw a runtime error
		}
	}

	void LveWindow::initWindows()
	{
		glfwInit(); // initialise the glfw library
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // to force GLFW not to open an OPENGL window has we are using vulkan 
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // to avoid windows resizing the window

		window = glfwCreateWindow(width, height, windowname.c_str(), nullptr, nullptr); // we get the pointer of the window 
	}


}