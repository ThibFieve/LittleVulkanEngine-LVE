#include"first_app.h"


// this tutorial serie https://www.youtube.com/watch?v=_riranMmtvI



// few header files from the standard librrary (std)
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
	lve::FirstApp app{}; // here he put {} , we call the default constructor of FirstApp and put 

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE; // if it doesnt 
	}




	return EXIT_SUCCESS; // if it works
}