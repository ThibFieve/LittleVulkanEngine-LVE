
#include "lve_model.h"



//std
#include <cassert>
#include <cstring>
namespace lve
{


	// constructor
	LveModel::LveModel(LveDevice& device, const std::vector<Vertex>& vertices) : lveDevice{device} 
	{
		createVertexBuffers(vertices);
	}


	// destructor
	LveModel::~LveModel() 
	{
		vkDestroyBuffer(lveDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(lveDevice.device(), vertexBufferMemory, nullptr);
		//allocation of memory
		// The reason why the allcoation of buffer and memory is separated is taht allcoating memory takes time 
		// Also there is a hard limit on the total number of active allocation at once depending on the GPU
		//The maximum number of valid memory allocations that can exist simultaneously within a VkDevice may be restricted by implementation- or platform-dependent limits. The maxMemoryAllocationCount feature describes the number of allocations that can exist simultaneously before encountering these internal limits
		// https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkAllocateMemory.html
		// Beter then to allcoate bigger chunks of memory and assign pieces for different models
		//Best practice is to use the Vulkan Memory Allocator libray ! VMA https://gpuopen.com/vulkan-memory-allocator/
	}	// How to make a memory allocator by ourselve http://kylehalladay.com/blog/tutorial/2017/12/13/Custom-Allocators-Vulkan.html





	void LveModel::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size()); // cast the number of vertices
		//let's assert that we have minimum of 3 vertices to have minimum a triangle , use cassert
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount; // sizeof gives us the byte number for one vertex , then mutliply by the number of vertices we have in total and we get the buffer size
		//Next call the create buffer function , it's a helper function in the lveDeviceclass
		 // it's a out function , you pass buffer and bufferMemory in and they will be changed

		lveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,// this tells our device that this buffer will be to hold vertex data 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // | is the bitwise or operator,combines the bit flags together, the host is the CPU , device is GPU ,
			//the host visible bit tels vulkan that we awant the allocated memory to be accessible from the CPU, the Coherant part is to keep the memory of GPU and CPU coherant, aka the same!
			// If this coherant property would be absent we would need to call VK flush to propagate any change from the device to the CPU
			vertexBuffer,
			vertexBufferMemory
		);
		void* data; // local varaible void pointer data
		vkMapMemory(lveDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);// 0 is offset ,  other 0 is for not providing any vk flags, and then data
		// this function will set data to point to the memory location of the begining of the vertex buffer memory in the GPU ! So data on the Cpu sides holds that adress
		// //this function actually creates a mapping between the CPU and GPU memory
		//Then use memcopy function
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize)); // takes the vertices data and copy it in the host mapped memory region, becasue we have the host coherant bit VK_MEMORY_PROPERTY_HOST_COHERENT_BIT , the host memory will automatically
		//be flushed into the GPU to update its memory, if this VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit  wasn't there we would need to manually call flush function
		vkUnmapMemory(lveDevice.device(), vertexBufferMemory);


	}



	void LveModel::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertexBuffer }; // create a local variable that is a list 
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets); // This will record to our command buffer to bind one vertrex buffer starting at binding 0 with offset of 0 
		// when we will eventually add mutliple biding we can easily do so by by using the variable offset and buffers !
	}


	void LveModel::draw(VkCommandBuffer commandBuffer)
	{
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}


	std::vector<VkVertexInputBindingDescription> LveModel::Vertex::getBindingDescriptions()
	{//See hand notes 
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);// local variable , vector of VkVertexInputBinding Description of size1 
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex); // Sizeof gives the memory spqce in bytes 
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // other option is for instanced data
		return bindingDescriptions; // this binding descriptions corresponds to a single vertex buffer that will ocupy the first binding at index  
		//the stride is the size of the vertex in bytes

	}


	std::vector<VkVertexInputAttributeDescription> LveModel::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
		attributeDescriptions[0].binding = 0; // This is  for the binding 0
		attributeDescriptions[0].location = 0; // that correspond to the location specific in the vertex shaders => layout(location=0) in vec2 position;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT; // specify the data type , there are 2 component and each of them are 32 bit signed floats
		attributeDescriptions[0].offset = 0;
		return attributeDescriptions;
	}
	


} // namespace lve