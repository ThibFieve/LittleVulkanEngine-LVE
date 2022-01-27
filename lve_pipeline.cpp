
// Create eh fileparse and get the vertex shaders and fragmetn shaders from the files


#include "lve_pipeline.h"
#include "lve_model.h"
//standar library std
#include <fstream>
#include <stdexcept>
#include <iostream>

#include <cassert> // to make assert function that will check something and if not true it will raise an error message in the log

namespace lve {

	LvePipeline::LvePipeline( // constructor
		LveDevice& device,
		const std::string& vertFilepath, 
		const std::string& fragFilepath, 
		const PipelineConfigInfo& configInfo ) : lveDevice{ device }// the varaible lveDevice = the value entered in the constructor as device
	{//constructor 
		createGraphicPipeline(vertFilepath, fragFilepath , configInfo);

	}

	// Destructor to clean up vulkan objects
	LvePipeline::~LvePipeline() {
		vkDestroyShaderModule(lveDevice.device(), vertShaderModule, nullptr); // remvoe the vulkan object of vertex shader
		vkDestroyShaderModule(lveDevice.device(), fragShaderModule, nullptr);// remvoe the vulkan object of frag  shader
		vkDestroyPipeline(lveDevice.device(), graphicsPipeline, nullptr);
	}

	



	std::vector<char> LvePipeline::readFile(const std::string& filepath) {
		// To read our shader file
		std::ifstream file{ filepath, std::ios::ate | std::ios::binary }; // constructor, second argumetn is bit flag , std::ios::ate we seek the end of the file , and we read it as a binary
		if (!file.is_open()) {
			throw std::runtime_error("failed to open file" + filepath);
		}

		rsize_t fileSize = static_cast<size_t>(file.tellg()); // tell g gives the alst position we were , which is already at the end of the file
		std::vector<char> buffer(fileSize); // char buffer
		file.seekg(0); // seek start of the file
		file.read(buffer.data(), fileSize); //read 
		file.close(); // close when finish to read
		return buffer; // return buffer
	}





	void LvePipeline::createGraphicPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo) {

		assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "cannot Create graphics pipeline : no pipelineLayout provided in configInfo"); // from <cassert> include ! , assert is a macro https://www.youtube.com/watch?v=j3mYki1SrKE
		assert(configInfo.renderPass != VK_NULL_HANDLE && "cannot Create graphics pipeline : no renderpass provided in configInfo"); // from <cassert> include !


		auto vertCode = readFile(vertFilepath);
		auto fragCode = readFile(fragFilepath);

		//std::cout << "Vertex Shader Code Size" << vertCode.size() << '\n';    // Gives the code size
		//std::cout << "Fragment Shader Code Size" << fragCode.size() << '\n';


		createShaderModule(vertCode, &vertShaderModule); // vertShaderModule is a member varaible of the class
		createShaderModule(fragCode, &fragShaderModule);// fragment shader

		// create info
		VkPipelineShaderStageCreateInfo shaderStages[2]; // array of size 2 , we create an array shaderStages containing object of type VkPipeline


		//Vertex SHADER
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT; // this sahder stage is for vertex shader
		shaderStages[0].module = vertShaderModule;
		shaderStages[0].pName = "main"; // name of entry function in our vertex shader , the first function that will be run 
		shaderStages[0].flags = 0;
		shaderStages[0].pNext = nullptr;
		shaderStages[0].pSpecializationInfo = nullptr; // mechanism to customize shader functionality 

		//FRAGMENT SHADER
		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT; // this sahder stage is for frag shader
		shaderStages[1].module = fragShaderModule; // the name of our module object
		shaderStages[1].pName = "main"; // name of entry function in our vertex shader , the first function that will be run 
		shaderStages[1].flags = 0;
		shaderStages[1].pNext = nullptr;
		shaderStages[1].pSpecializationInfo = nullptr; // mechanism to customize shader functionality






		// create a new local variable of type
		//This is where the info about the vertex buffer will be stored tutorial 6
		auto bindingDescriptions = LveModel::Vertex::getBindingDescriptions(); // see Model.cpp implementation
		auto attributeDescriptions = LveModel::Vertex::getAttributeDescriptions(); // se model.cpp implementationa nd vertex buffer lesson 
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{}; // sthis struct describe how we interprete our vertex sahder data that is the inital input of the graphjic pipeline
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()); 
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();




		// local viewport info variable 
		//VkPipelineViewportStateCreateInfo viewportInfo{}; // initialise the struct with its argument to either 0 , null or nullptr
		// WE Then combine our viewport and scissor into one strucutre that is called a viewport_state_create_info variable////////////////////////////////////////////////////////////////////
		//this has been moved here from deefaultpipelineconfig  bc we could have issues where the viewportinfo of different pieplines would be pointing to the same scissor and viewport. If we dealocate one pipeline it would
		//delete thaose and we would have crashed https://www.youtube.com/watch?v=IUYH74MqxOA  5:00
		// 
		// Self referencing bug , we will pass config info as an argument
		//viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		//viewportInfo.viewportCount = 1; // on some graphic cards it is possible to use multiple viewport and scissors by enabling gpu features
		//viewportInfo.pViewports = &configInfo.viewport;
		//viewportInfo.scissorCount = 1;
		//viewportInfo.pScissors = &configInfo.scissor;





		VkGraphicsPipelineCreateInfo pipelineInfo{}; // ACTUAL GRAPHIC PIPELINE OBJECT ! /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2; // HOW MANY PROGRAMMABLE STAGES OUR PIPELINE HAS , will use , vertex and fragemtn shader
		pipelineInfo.pStages = shaderStages; // the list of the two shaders
		pipelineInfo.pVertexInputState = &vertexInputInfo; // object created just above  , becasue we separated our pipeline creation from our pipeline configuration we need to link everyhting up , 
		// but we can reuse this code if we want to make different pipelines !"
		pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
		pipelineInfo.pViewportState = &configInfo.viewportInfo;
		pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
		pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
		pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
		pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;// optional setting for changing the pipeline dynamically without needing to recreate the pipeline
		pipelineInfo.pDynamicState = nullptr;

		pipelineInfo.layout = configInfo.pipelineLayout;  // WE HAVE NOT CONFIGURED THOSE YET SO IF WE RUN THE PROGRAM WE WILL GET AN ERROR CRASH ,// now it is done in first app
		pipelineInfo.renderPass = configInfo.renderPass;// WE WILL SEE BY RUNNING THAT THE VALIDATION LAYER HELPS US OUT TO FIND THE ISSUE , toerhwise it would crash we would not know why  , right now assigned to nullptr
		pipelineInfo.subpass = configInfo.subpass;


		pipelineInfo.basePipelineIndex = -1; // FOR Completeness sake , thos can be used to optimize performance
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // create new graphic pipeline from an existing one can be less expensive on the GPU 


		if (vkCreateGraphicsPipelines(lveDevice.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)// we pass it our device , a null handler for our pipeline cache ( possible performance optimisation) , pipelineinfo we made, no callback function , pointer to graphicsPipeline handle 
		{
			throw std::runtime_error("failed to create graphics pipeline");
		}
			 
	}




	void LvePipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) { // CREATE A SHADER MODULE 
		VkShaderModuleCreateInfo createInfo{}; //we create  local Vk shader module 
		//common pattern instead of calling a function with  a bunch of paramter , we conmfigure a strcut and call a function  with a pointer to the struct
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO; // the type of that structure is a VK structure shader module create info
		createInfo.codeSize = code.size();// the code size is the size of the array
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data()); // a uint32 and charcther are not the same size so we gotta be carful with it 
		//,We need to cast the code from being char to being uint32 as vulkan expect it to be uint32
		// becasue our data is stored in a vector , this cast will work , however C style characther array that wouldnt be a valid cast . 


		if (vkCreateShaderModule(lveDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) // LveDevice.device() is a getter to the device handle  that our device classs encapsulate
		{ // pointer to createinfo , nullpointer is function callbacks
			throw std::runtime_error("failed to create shader module");//runtime error
		}

	}





	void LvePipeline::defaultPipelineConfigInfo(
		PipelineConfigInfo& configInfo, uint32_t width, uint32_t height) {
		//REALLY INTERESTING AND IMPORANT TO UNDERSTAND THOSE //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		 

		//https://www.youtube.com/watch?v=ecMcXW6MSYU  ///////////////////////////////////////////////////////////////////////////////








		configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO; // initialise the inputassemblyinfo from the vulkan library
		configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // WE WILL USE TRIANGLE LIST AS INPUT DATA IN THE GRAPHIC PIPELINE , every tree vertices are put together as a triangle , there is also traingle strip !
		configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE; // for strip , if this is true , using a strip topology we can break off strips running a special index value into a index buffer 
		//from more info on topologies check https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkPrimitiveTopology.html





		//configure viewport and scissor////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//viewport describe the transforamtion between our pipleine output and target image
		// transform our GL_position  in our shader to our ouput image
		//basicaaly going from -1 to 1 nromalised coordiantes to the Framebuffer color attachement that is in pixel from (0,0) to (width,height)
		configInfo.viewport.x = 0.0f;
		configInfo.viewport.y = 0.0f;
		configInfo.viewport.width = static_cast<float>(width);  
		configInfo.viewport.height = static_cast<float>(height); //configInfo.viewport.height = static_cast<float>(height) * 0.5 ; will make is squished/compressed vertically ! on top half of display
		configInfo.viewport.minDepth = 0.0f;// depth range for the viewport , similarly linearly transform the z component of Gl position
		configInfo.viewport.maxDepth = 1.0f;

		configInfo.scissor.offset = { 0, 0 }; // everytrianglkke outisde of the scissor rectangle will be discarded , it is culling ,view fustrum
		configInfo.scissor.extent = { width, height };

		//Self referencing bug

		configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		configInfo.viewportInfo.viewportCount = 1;
		configInfo.viewportInfo.pViewports = &configInfo.viewport;
		configInfo.viewportInfo.scissorCount = 1;
		configInfo.viewportInfo.pScissors = &configInfo.scissor;



		//  REMOVED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!////////////////////////////////////////////////////////////////////////////////////////////////////////
		//this has been moved here from deefaultpipelineconfig  bc we could have issues where the viewportinfo of different pieplines would be pointing to the same scissor and viewport. If we dealocate one pipeline it would
		//delete thaose and we would have crashed https://www.youtube.com/watch?v=IUYH74MqxOA  5:00
		//configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		//configInfo.viewportInfo.viewportCount = 1; // on some graphic cards it is possible to use multiple viewport and scissors by enabling gpu features
		//configInfo.viewportInfo.pViewports = &configInfo.viewport;
		//configInfo.viewportInfo.scissorCount = 1;
		//configInfo.viewportInfo.pScissors = &configInfo.scissor;
		//REMOVED ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//Rasterisations stage/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 
		//Transform geometry into fragemtns
		configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO; // struct type member first
		configInfo.rasterizationInfo.depthClampEnable = VK_FALSE; // force the z component og GL positon to be between 0 and 1 , values less than 0 are clamped at 0 , greater than 1 is clamped at 1 , 
		//  usually we dont want that to be true , as something less than 0 should be behind the camera , Z greater than one is too far wawy to see
		configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE; //  controler wheter primitives are discarded immediatly before the rasterizations stage , only used if we jsut want to use the first view stage of the graphic pipeline
		configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;// do we draw corner , the edges of the rangle filled in 
		configInfo.rasterizationInfo.lineWidth = 1.0f;
		configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE; // Cull mode  do we discard triangle facing the wrong way to us ?  what is the wrong way anti-clockwise means they are behind the object
		// Here we need to spoecific which fgace of the triangle we cull , for now we dont cull any so we dont cull our first attempt at doing a triangle 
		configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE; // widing order  
		//Depth biased can be used to alter values of depth
		configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
		configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
		configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
		configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional



		//Multisampling , explain how our rasterizer deals with the edges of gemotry , to do anti aliasing techniques 
		// Without a multisampling a fragment is considered either completely in or completely out of the triangle , based on where the center of the pixel is . we get aliasing
		//MSAA Multi sample anti-alisaing , take mutlisample of fragment on the edge of the geometry to better apprixomate how much of the fragment/pixel is contained by the triangle, based on this shade the 
		// pixel by a variable ammount
		configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
		configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
		configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
		configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

		// Colorblend is when two triangle overlaps then fragment shader will return multiple colours for the same pixel in our frame buffer
		configInfo.colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		configInfo.colorBlendAttachment.blendEnable = VK_FALSE; // we can enable this , determnine how to blend the current output  to the value already in the framebuffer if any  THIS IS FOR TRANSPARANCY !
		configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional



		configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
		configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
		configInfo.colorBlendInfo.attachmentCount = 1;
		configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
		configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional



		//Depth Buffer
		// additional attachement to our frame buffer that stores the values for every pixel just like for color attachment (frame buffer = colorbuffer) . So basically it is a buffer that holds a value of depth of the pixel
		// that pixel has a value that corresponds to the closest object 

		configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
		configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
		configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.front = {};  // Optional
		configInfo.depthStencilInfo.back = {};   // Optional


	
	 }


	 void LvePipeline::bind(VkCommandBuffer commandBuffer) // bind graphic pipeline to a commandbuffer
	 {
		 vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline); // this signals that this is a graphic piepline , there are other types of pipeline , compute pipeline and ray tracing pipelines

	 }



}