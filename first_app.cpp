#include "first_app.h"


//Std
#include <stdexcept>
#include <array>
namespace lve {


	FirstApp::FirstApp()
	{
		loadModels();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}


	FirstApp::~FirstApp()
	{
		vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
	}


	void FirstApp::run() 
	{

		while (!lveWindow.shouldClose()) // see if the windows wants to closeor not
		{
			// while we do not want to close 
			glfwPollEvents();  // check if event like  key stroke  user click to dismiss the window
			drawFrame();
		}

		vkDeviceWaitIdle(lveDevice.device()); // with this the CPU will block up until the GPU is done , so that when
		//we close the window , it's not in the middle of GPU calculation

	}



	void FirstApp::sierpinski(

		// THIS SOLUTION IS PROVIDED BY Brendan Galea https://www.youtube.com/watch?v=mnKp501RXDc for testing and exercises purposes.
		std::vector<LveModel::Vertex>& vertices,
		int depth,
		glm::vec2 left,
		glm::vec2 right,
		glm::vec2 top) {
		if (depth <= 0) {
			vertices.push_back({ top });
			vertices.push_back({ right });
			vertices.push_back({ left });
		}
		else {
			auto leftTop = 0.5f * (left + top);
			auto rightTop = 0.5f * (right + top);
			auto leftRight = 0.5f * (left + right);
			sierpinski(vertices, depth - 1, left, leftRight, leftTop);
			sierpinski(vertices, depth - 1, leftRight, right, rightTop);
			sierpinski(vertices, depth - 1, leftTop, rightTop, top);
		}
	}


	// Load models , lesson 6 , vertex buffer
	void FirstApp::loadModels()
	{

		// FOR A SIMPLE TESTING TRIANGLE 
		
		std::vector<LveModel::Vertex> vertices{ // this { initialise the vector
			{{0.0f,-0.5f},{1.0f,0.0f,0.0f}}, // { this is for the struct{ this is a tuple , glm:vec2 position},{glm:vec3 color}}
			{{0.5f,0.5f},{0.0f,1.0f,0.0f}},
			{{-0.5f,0.5f},{0.0f,0.0f,1.0f}},
		};

		/* For  Sierpinski solution
		std::vector<LveModel::Vertex> vertices{};
		sierpinski(vertices, 5, { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.0f, -0.5f });
		*/

		lveModel = std::make_unique<LveModel>(lveDevice, vertices);
	}

	void FirstApp::createPipelineLayout() 
	{ // Empty layout 
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr; // Pipelinesetlayout is used to pass data other than vertex data to our vertex and fragemetn shaders, like texture and uniforms buffer objects
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // a way to very efficeiently send a small data to our shader program ( see tutorial 8)
		if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}


	void FirstApp::createPipeline() 
	{

		assert(lveSwapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		LvePipeline::defaultPipelineConfigInfo(pipelineConfig); // important to get the width and heigh of the swapchain as it doesnt necessary match the window  , for exemple apple retina display of apple , the window in screen coordinate is smaller than actual resolution
		
		pipelineConfig.renderPass = lveSwapChain->getRenderPass();// default render pass creatd from the swapchain code , render pass describe structure and format of our frame buffer object and its attachement , exemple attachement 0 is color buffer , attachement 1 is depth buffer etcc. Render pass is sort of bluepritn telling the pipeline layout waht to expect as output frame buffer .
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline = std::make_unique<LvePipeline>( // Smartpointer  CALL THE CONSTRUCTOR OF lve_pipeline , the constructor launches a function that create the Graphic pipeline.
			lveDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig
			);
	}

	void FirstApp::recreateSwapChain()
	{
		auto extent = lveWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = lveWindow.getExtent();
			glfwWaitEvents(); // this is when minimization , if windows is sizeless  , then glfw sould pause and wait
		}
		vkDeviceWaitIdle(lveDevice.device());// stay idle until we create the new swapchain

		if (lveSwapChain == nullptr)
		{
			lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent);
		}
		else {
			lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent, std::move(lveSwapChain));
		// Smart pointers and move semantics , you cannot copy a unique pointer, but using move we create a copy
		// and automatically assigne lveSwapChain to be a null pointer https://www.learncpp.com/cpp-tutorial/introduction-to-smart-pointers-move-semantics/#:~:text=A%20Smart%20pointer%20is%20a,t%20clean%20up%20after%20themselves
			if (lveSwapChain->imageCount() != commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}



		// If render pass compatible do nothing else
		createPipeline();
	}





	void FirstApp::createCommandBuffers()
	{
		commandBuffers.resize(lveSwapChain->imageCount()); // we decided that each comamnd buffer is going to draw to a different frame buffer
		VkCommandBufferAllocateInfo allocInfo{}; // initilaise the struct of command buffer

		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // there are 2 kinds of command buffers , priamy and secodnary
		//Primary can be submitted to a queue for execution but cannot be called by other command buffers
		// secondary cannot be submitted but can be called by other command buffers
		allocInfo.commandPool = lveDevice.getCommandPool(); // command pool are opaque objcet , we may need to create and destroy command buffer frequently , so to reduce the overhead of creatioon and deletion in memory , we use command pools we initialised memory (that is the expensive step)
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffer !");
		}

		

		
	}

	void FirstApp::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			lveDevice.device(),
			lveDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}

	
	void FirstApp::recordCommandBuffer(int imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{}; // initialised
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer !");

		}

		// Now we create a render pass !
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = lveSwapChain->getRenderPass(); // we get the render pass from the swapchain
		renderPassInfo.framebuffer = lveSwapChain->getFrameBuffer(imageIndex); 	 // we say in which frame buffer this render pass is written in

		//render area , this device the area where the shader loads and stores will take place
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = lveSwapChain->getSwapChainExtent(); // as before the swapchain might be larger than the one of the window




		//Pass the clear value , what value does it have per default in the frame buffer attachement

		std::array<VkClearValue, 2> clearValues{}; // an array of clearValue objects
		//index 0 is the color attachment , index 1 is the depth attachment
		clearValues[0].color = { 0.1f,0.1f,0.1f,1.0f }; // RGBA value
		clearValues[1].depthStencil = { 1.0f,0 }; // Depth value
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();


		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);// lets tell the command buffer
			//to begin the render pass , we choose the command  buffer we recording to , pass the render info , and last argument  VK_SUBPASS_CONTENTS_INLINE signals that the subsequent pass command will be embedded in the primary command buffer , and that no secondary command buffer will be used. https://www.youtube.com/watch?v=_VOR6q3edig
			//Cant have both inline command and secondary cmd buffer simultaneously


		// We feed the dynamic scissors and dynamic viewports

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(lveSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(lveSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, lveSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);



		lvePipeline->bind(commandBuffers[imageIndex]); // we implemented that in lvepipeline
		lveModel->bind(commandBuffers[imageIndex]);
		lveModel->draw(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}

	}

	void FirstApp::drawFrame()
	{
		uint32_t imageIndex;
		auto result = lveSwapChain->acquireNextImage(&imageIndex); // fetch the index of the frame that is rendered next and automatically handles the CPU /GPU synchronisation  for double or triple buffering
		
		if (result == VK_ERROR_OUT_OF_DATE_KHR)  // If the surface has changed in a way that is not comaptible with the swapchain
		{
			recreateSwapChain();// we recreate swap chain
			return;
		}
		
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image  !"); // in the futur we will need to handle this situation as this happens when a window is resized 
		}

		recordCommandBuffer(imageIndex); // as we dont to it at startup anymore

		result = lveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex); // pointer at the commandbuffer at image index and pointer of image index , this fucntion submits the command buffer to the device graphic queue while handling GPU CPU synchronisationa 
		//, then the command buffer will be excuted  , the swap chain will then presented the associated color iamge view at appropriate time based on 
		
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lveWindow.wasWindowResized())
		{ // If the surface doesnt correspond to the swapchain but is still usable , 
			lveWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}
		
		if (result != VK_SUCCESS )
		{
			throw std::runtime_error("failed to acquire swap chain image  !"); // in the futur we will need to handle this situation as this happens when a window is resized 
		}

	}
	

}