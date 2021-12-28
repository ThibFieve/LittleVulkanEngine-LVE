#include "first_app.h"

namespace lve {

	void FirstApp::run() {

		while (!lveWindow.shouldClose()) // see if the windows wants to closeor not
		{
			// while we do not want to close 
			glfwPollEvents();  // check if event like  key stroke  user click to dismiss the window
		}

	}



}