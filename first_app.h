#pragma once


#include "lve_window.h"
#include "lve_pipeline.h"
#include "lve_device.h"
#include "lve_swap_chain.h"

// include for memory 
//STD
#include <memory> // to access smart pointers
#include <vector>
namespace lve {

	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
		FirstApp(); // constructor
		~FirstApp(); // destructor
		//We will delete the copy construcor and copy destroctor from our window class , bc we use a pointer to the glfw window
		FirstApp(const FirstApp&) = delete; //https://www.youtube.com/watch?v=BvR1Pgzzr38
		FirstApp& operator = (const FirstApp&) = delete;
		// see shallow and deep copies , 

		void run() ;
		
	private:

		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers(); // in vulkan , no directly command with function calls , we use a command buffer that stores instrcution and goes in a device queue to be executed , this allow for sequences of command to be recorded 
		void drawFrame();

		LveWindow lveWindow{ WIDTH, HEIGHT,"Hello Vulkan!" }; //https://www.youtube.com/watch?v=1nfuYMXjZsA   , https://www.youtube.com/watch?v=FXhALMsHwEY&list=PLlrATfBNZ98dudnM48yfGUldqGD0S4FFb&index=25
		// not using a pointer or any dynamic memory allocation 
		//// here he put {} instead of () , this is a way to initainalise value with the constructor ! calling a csontrcutor from a header file 
		// is this due to C++ 17 ?  https://en.cppreference.com/w/cpp/language/constructor
		//
		LveDevice lveDevice{ lveWindow };
		LveSwapChain lveSwapChain{ lveDevice , lveWindow.getExtent() }; // HERE ORDER MATTERS ! varaibles intialised top to bottom
		
		std::unique_ptr<LvePipeline> lvePipeline; // Smart pointer  https://www.youtube.com/watch?v=UOB7-B2MfwA , automamte the new , delete thing to allocate memory on the stack.
		// Unique poitner is a scoped poitner , when we going out of scope this pointer is deleted , you cant copy a unique pointer , stack allocator object
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;





		//LvePipeline lvePipeline{lveDevice,"shaders/simple_shader.vert.spv","shaders/// simple_shader.frag.spv",LvePipeline::defaultPipelineConfigInfo(WIDTH,HEIGHT) };  OLD CODE WE MOVED IT 
	};



}