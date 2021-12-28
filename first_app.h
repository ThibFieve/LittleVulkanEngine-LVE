#pragma once


#include "lve_window.h"
#include "lve_pipeline.h"
#include "lve_device.h"
#include "lve_swap_chain.h"
namespace lve {

	class FirstApp {
	public:
		 static constexpr int WIDTH = 800;
		 static constexpr int HEIGHT = 600;


		void run() ;
		
	private:
		LveWindow lveWindow{ WIDTH, HEIGHT,"Hello Vulkan!" }; //https://www.youtube.com/watch?v=1nfuYMXjZsA   , https://www.youtube.com/watch?v=FXhALMsHwEY&list=PLlrATfBNZ98dudnM48yfGUldqGD0S4FFb&index=25
		// not using a pointer or any dynamic memory allocation 
		//// here he put {} instead of () , this is a way to initainalise value with the constructor ! calling a csontrcutor from a header file 
		// is this due to C++ 17 ?  https://en.cppreference.com/w/cpp/language/constructor
		//


		LveSwapChain lveSwapChain{ lveDevice , lveWindow.getExtent() }; // HERE ORDER MATTERS ! 


		LveDevice lveDevice{ lveWindow };

		LvePipeline lvePipeline{lveDevice,"shaders/simple_shader.vert.spv","shaders/simple_shader.frag.spv",LvePipeline::defaultPipelineConfigInfo(WIDTH,HEIGHT) };

	};



}