#pragma once

#include <string>
#include <vector>
#include "lve_device.h"

namespace lve {


		// lets create a structur that will hold our data used to configure our pipeline, it's a helper structure
	struct PipelineConfigInfo
	{ // we will explicitely set the different value of each stage of our pipeline and how it will work
		// this will be initialised in the  defaultPipelineConfigInfo function !
		//REALLY INTERESTING AND IMPORANT TO UNDERSTAND THOSE 
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;  // delete copy constructor
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo() = default;// default constructor
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkViewport viewport;
		VkRect2D scissor;
		//VkPipelineViewportStateCreateInfo viewportInfo; Fixing mistake
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr; // we dont provide any default fir this members and we will set them outside of defaultPipelineConfigInfo
		VkRenderPass renderPass = nullptr;// we dont provide any default fir this members and we will set them outside of defaultPipelineConfigInfo
		uint32_t subpass = 0;// we dont provide any default fir this members and we will set them outside of defaultPipelineConfigInfo



	};
	// The reason it's out of pipeline class is because we want our application layer to be easily able to reach and configure a pipeline compeltely as well as to sahre the configuration between mutliple pilenine



	class LvePipeline {
	public:
		LvePipeline(LveDevice& device,const std::string& vertFilepath, const std::string& fragFilepath , const PipelineConfigInfo& configInfo);//constructor

		~LvePipeline();// destructor

		

		LvePipeline(const LvePipeline&) = delete; // delete the default copy constructor https://www.youtube.com/watch?v=BvR1Pgzzr38&t=590s
		// we want to avoid  duplicating the pointer to our vulkan object by mistake
		void operator=(const LvePipeline&) = delete;



		void bind(VkCommandBuffer commandBuffer);// bind graphic pipeline to a commandbuffer

		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo, uint32_t width, uint32_t height);// static function to create a default  PipelineConficInfo structure

	private:
		static std::vector<char> readFile(const std::string& filepath); // returns a string of char , takes in a ref of a string
		void createGraphicPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule); //takes as input the shader code as a vector of carachter , shaderModule is a pointer to a pointer !

		LveDevice& lveDevice; // private member variable storinng the device reference,  could be memory unsafe if  our device is realesed from memory before the pipeline , we could try to dereference a dangling pointer, would crash the program.
		//This is a reference type member varaible , it should implicitely outlive the class it lives on it .
		VkPipeline graphicsPipeline; // handle to vulkan pipeline object , it's a pointer !
		VkShaderModule vertShaderModule; // shader module variable for vert shader
		VkShaderModule fragShaderModule;// shader module variable for frag shader
	};

}