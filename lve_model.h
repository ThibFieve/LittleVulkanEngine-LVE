#pragma once


#include "lve_device.h"

#define GLM_FORCE_RADIANS  // ensure that no matter the OS GLM will be in radians , not degrees 
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // force the Z value to be from 0 to 1
#include <glm/glm.hpp>



//std

#include <vector>

namespace lve 
{
	class LveModel
	{
		//The purpose of this class it to take vertex data created by and read from a file from the CPU 
		//and then allocate the memory on the GPU and copy the data over there for efficicent reading

	public:

		struct Vertex
		{
			glm::vec2 posistion;
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		LveModel(LveDevice& device, const std::vector<Vertex>& vertices); // constructor
		~LveModel(); // destructor


		//We will delete the copy construcor and copy destructor from our LveModelclass , as it deals with memory of vulkan , if we 
		//make copies it might corrupt the memory
		LveModel(const LveModel&) = delete; //https://www.youtube.com/watch?v=BvR1Pgzzr38
		LveModel& operator = (const LveModel&) = delete;
		// see shallow and deep copies , 
		
		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
		

	private:

		void createVertexBuffers(const std::vector<Vertex>& vertices);


		LveDevice& lveDevice;
		VkBuffer vertexBuffer; // The buffer
		VkDeviceMemory vertexBufferMemory; // The assgigned memory of the buffer , 2 separate objects !
		// The memory is not automatically asssigned , so us , the programmer is in controll of memory management 
		uint32_t vertexCount;

	}; // namespace lve


}