#include <windows.h>
#include <vulkan/vulkan.h>

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
constexpr uint32_t kWidth = 800;
constexpr uint32_t kHeight = 600;

const std::array<const char*, 1> kRequiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

// Simple passthrough triangle shaders compiled to SPIR-V.
const uint32_t kVertexShaderSpv[] = {
  0x07230203,0x00010000,0x0008000a,0x0000002a,0x00000000,0x00020011,0x00000001,0x00020011,
  0x0000001c,0x0006000b,0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,
  0x00000000,0x00000001,0x0009000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000c,
  0x00000014,0x0000001e,0x00000024,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,
  0x6e69616d,0x00000000,0x00050005,0x0000000a,0x6f506e69,0x69746973,0x00006e6f,0x00060005,
  0x0000000c,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x0000000c,0x00000000,
  0x505f6c67,0x7469736f,0x006e6f69,0x00070006,0x0000000c,0x00000001,0x505f6c67,0x746e696f,
  0x657a6953,0x00000000,0x00070006,0x0000000c,0x00000002,0x435f6c67,0x4470696c,0x61747369,
  0x0065636e,0x00070006,0x0000000c,0x00000003,0x435f6c67,0x446c6c75,0x61747369,0x0065636e,
  0x00030005,0x0000000e,0x00000000,0x00040005,0x00000014,0x6f436e69,0x00726f6c,0x00040005,
  0x0000001e,0x6f436f76,0x00726f6c,0x00030005,0x00000024,0x00786574,0x00040047,0x0000000c,
  0x0000000b,0x0000001c,0x00040047,0x00000014,0x0000001e,0x00000000,0x00040047,0x0000001e,
  0x0000001e,0x00000000,0x00040047,0x00000024,0x0000001e,0x00000001,0x00020013,0x00000002,
  0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,
  0x00000006,0x00000002,0x00040017,0x00000008,0x00000006,0x00000003,0x00040015,0x00000009,
  0x00000020,0x00000001,0x00040020,0x0000000b,0x00000007,0x00000007,0x0006001e,0x0000000c,
  0x00000008,0x00000006,0x00040020,0x0000000d,0x00000003,0x0000000c,0x0004003b,0x0000000d,
  0x0000000e,0x00000003,0x00040015,0x0000000f,0x00000020,0x00000000,0x0004002b,0x0000000f,
  0x00000010,0x00000000,0x00040020,0x00000011,0x00000003,0x00000008,0x00040020,0x00000013,
  0x00000001,0x00000009,0x0004003b,0x00000013,0x00000014,0x00000001,0x0004002b,0x00000006,
  0x00000017,0xbf000000,0x0004002b,0x00000006,0x00000018,0x3f000000,0x0005002c,0x00000007,
  0x00000019,0x00000017,0x00000018,0x0004002b,0x00000006,0x0000001a,0x3f800000,0x0004002b,
  0x00000006,0x0000001b,0x00000000,0x0005002c,0x00000007,0x0000001c,0x0000001b,0xbf000000,
  0x0005002c,0x00000007,0x0000001d,0x0000001a,0x00000018,0x0006002c,0x00000007,0x0000001e,
  0x00000019,0x0000001c,0x0000001d,0x00040020,0x00000021,0x00000003,0x00000006,0x0004002b,
  0x0000000f,0x00000022,0x00000001,0x00040020,0x00000023,0x00000003,0x00000006,0x0004003b,
  0x00000013,0x00000024,0x00000001,0x0004002b,0x00000006,0x00000026,0x3f400000,0x0005002c,
  0x00000008,0x00000027,0x0000001b,0x00000026,0x0000001b,0x0005002c,0x00000008,0x00000028,
  0x0000001a,0x0000001b,0x0000001b,0x0005002c,0x00000008,0x00000029,0x0000001b,0x0000001b,
  0x0000001a,0x0006002c,0x00000008,0x0000002a,0x00000027,0x00000028,0x00000029,0x00050036,
  0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,0x00000005,0x0004003d,0x00000009,
  0x00000015,0x00000014,0x00060041,0x00000011,0x00000016,0x0000000e,0x00000010,0x00000010,
  0x00050051,0x00000006,0x0000001f,0x0000001e,0x00000015,0x00050051,0x00000006,0x00000020,
  0x0000001e,0x00000015,0x00070050,0x00000008,0x00000021,0x0000001f,0x00000020,0x0000001b,
  0x0000001a,0x0003003e,0x00000016,0x00000021,0x0004003d,0x00000009,0x00000025,0x00000024,
  0x00060041,0x00000023,0x00000026,0x0000000e,0x00000010,0x00000022,0x00050051,0x00000006,
  0x0000002b,0x0000002a,0x00000025,0x0003003e,0x00000026,0x0000002b,0x000100fd,0x00010038
};

const uint32_t kFragmentShaderSpv[] = {
  0x07230203,0x00010000,0x0008000a,0x0000000f,0x00000000,0x00020011,0x00000001,0x0006000b,
  0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
  0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000b,0x00030010,
  0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
  0x00000000,0x00050005,0x00000009,0x6f436f76,0x00726f6c,0x00040005,0x0000000b,0x6f436f66,
  0x00726f6c,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000b,0x0000001e,
  0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000005,
  0x00000020,0x00040017,0x00000006,0x00000005,0x00000003,0x00040020,0x00000007,0x00000003,
  0x00000006,0x0004003b,0x00000007,0x00000009,0x00000003,0x00040020,0x0000000a,0x00000001,
  0x00000006,0x0004003b,0x0000000a,0x0000000b,0x00000001,0x00050036,0x00000002,0x00000004,
  0x00000000,0x00000003,0x000200f8,0x00000008,0x0004003d,0x00000006,0x0000000c,0x0000000b,
  0x0003003e,0x00000009,0x0000000c,0x000100fd,0x00010038
};

struct QueueFamilyIndices {
  uint32_t graphicsFamily = UINT32_MAX;
  uint32_t presentFamily = UINT32_MAX;
  bool IsComplete() const { return graphicsFamily != UINT32_MAX && presentFamily != UINT32_MAX; }
};

class VulkanTriangleApp {
 public:
  void Run(HINSTANCE instance) {
    CreateWindowClass(instance);
    InitVulkan();
    MainLoop();
    Cleanup();
  }

 private:
  HWND hwnd_ = nullptr;
  VkInstance instance_ = VK_NULL_HANDLE;
  VkSurfaceKHR surface_ = VK_NULL_HANDLE;
  VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
  VkDevice device_ = VK_NULL_HANDLE;
  VkQueue graphicsQueue_ = VK_NULL_HANDLE;
  VkQueue presentQueue_ = VK_NULL_HANDLE;
  VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
  std::vector<VkImage> swapchainImages_;
  VkFormat swapchainImageFormat_ = VK_FORMAT_B8G8R8A8_UNORM;
  VkExtent2D swapchainExtent_{};
  std::vector<VkImageView> swapchainImageViews_;
  VkRenderPass renderPass_ = VK_NULL_HANDLE;
  VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
  VkPipeline graphicsPipeline_ = VK_NULL_HANDLE;
  std::vector<VkFramebuffer> swapchainFramebuffers_;
  VkCommandPool commandPool_ = VK_NULL_HANDLE;
  std::vector<VkCommandBuffer> commandBuffers_;
  VkSemaphore imageAvailable_ = VK_NULL_HANDLE;
  VkSemaphore renderFinished_ = VK_NULL_HANDLE;
  VkFence inFlightFence_ = VK_NULL_HANDLE;

  static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DESTROY) {
      PostQuitMessage(0);
      return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
  }

  void CreateWindowClass(HINSTANCE instance) {
    WNDCLASS wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = L"VulkanTriangleWindowClass";
    RegisterClass(&wc);

    hwnd_ = CreateWindowEx(0, wc.lpszClassName, L"Vulkan Triangle", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, static_cast<int>(kWidth), static_cast<int>(kHeight),
                           nullptr, nullptr, instance, nullptr);

    ShowWindow(hwnd_, SW_SHOW);
  }

  void InitVulkan() {
    CreateInstance();
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();
    CreateSwapchain();
    CreateImageViews();
    CreateRenderPass();
    CreateGraphicsPipeline();
    CreateFramebuffers();
    CreateCommandPool();
    CreateCommandBuffers();
    CreateSyncObjects();
  }

  void MainLoop() {
    MSG msg{};
    while (msg.message != WM_QUIT) {
      if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      } else {
        DrawFrame();
      }
    }
    vkDeviceWaitIdle(device_);
  }

  void Cleanup() {
    if (device_ != VK_NULL_HANDLE) {
      vkDestroyFence(device_, inFlightFence_, nullptr);
      vkDestroySemaphore(device_, renderFinished_, nullptr);
      vkDestroySemaphore(device_, imageAvailable_, nullptr);
      vkDestroyCommandPool(device_, commandPool_, nullptr);
      for (VkFramebuffer framebuffer : swapchainFramebuffers_) vkDestroyFramebuffer(device_, framebuffer, nullptr);
      vkDestroyPipeline(device_, graphicsPipeline_, nullptr);
      vkDestroyPipelineLayout(device_, pipelineLayout_, nullptr);
      vkDestroyRenderPass(device_, renderPass_, nullptr);
      for (VkImageView view : swapchainImageViews_) vkDestroyImageView(device_, view, nullptr);
      vkDestroySwapchainKHR(device_, swapchain_, nullptr);
      vkDestroyDevice(device_, nullptr);
    }
    if (surface_ != VK_NULL_HANDLE) vkDestroySurfaceKHR(instance_, surface_, nullptr);
    if (instance_ != VK_NULL_HANDLE) vkDestroyInstance(instance_, nullptr);
    if (hwnd_ != nullptr) DestroyWindow(hwnd_);
  }

  void CreateInstance() {
    VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    appInfo.pApplicationName = "Vulkan Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "CreateGameEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    std::array<const char*, 2> exts = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };

    VkInstanceCreateInfo createInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(exts.size());
    createInfo.ppEnabledExtensionNames = exts.data();

    Check(vkCreateInstance(&createInfo, nullptr, &instance_), "vkCreateInstance");
  }

  void CreateSurface() {
    VkWin32SurfaceCreateInfoKHR createInfo{VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    createInfo.hinstance = GetModuleHandle(nullptr);
    createInfo.hwnd = hwnd_;
    Check(vkCreateWin32SurfaceKHR(instance_, &createInfo, nullptr, &surface_), "vkCreateWin32SurfaceKHR");
  }

  static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    QueueFamilyIndices indices;
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
    std::vector<VkQueueFamilyProperties> families(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, families.data());

    for (uint32_t i = 0; i < count; ++i) {
      if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) indices.graphicsFamily = i;
      VkBool32 presentSupport = VK_FALSE;
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
      if (presentSupport) indices.presentFamily = i;
      if (indices.IsComplete()) break;
    }
    return indices;
  }

  void PickPhysicalDevice() {
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance_, &count, nullptr);
    if (count == 0) throw std::runtime_error("No Vulkan GPU found");
    std::vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(instance_, &count, devices.data());

    for (VkPhysicalDevice device : devices) {
      QueueFamilyIndices indices = FindQueueFamilies(device, surface_);
      if (!indices.IsComplete()) continue;
      physicalDevice_ = device;
      return;
    }

    throw std::runtime_error("No suitable Vulkan GPU found");
  }

  void CreateLogicalDevice() {
    QueueFamilyIndices indices = FindQueueFamilies(physicalDevice_, surface_);

    std::vector<VkDeviceQueueCreateInfo> queueInfos;
    std::array<uint32_t, 2> families = { indices.graphicsFamily, indices.presentFamily };
    float priority = 1.0f;
    for (uint32_t family : families) {
      bool exists = false;
      for (auto& info : queueInfos) if (info.queueFamilyIndex == family) exists = true;
      if (exists) continue;
      VkDeviceQueueCreateInfo queueInfo{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
      queueInfo.queueFamilyIndex = family;
      queueInfo.queueCount = 1;
      queueInfo.pQueuePriorities = &priority;
      queueInfos.push_back(queueInfo);
    }

    VkPhysicalDeviceFeatures features{};
    VkDeviceCreateInfo createInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
    createInfo.pQueueCreateInfos = queueInfos.data();
    createInfo.pEnabledFeatures = &features;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(kRequiredDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = kRequiredDeviceExtensions.data();

    Check(vkCreateDevice(physicalDevice_, &createInfo, nullptr, &device_), "vkCreateDevice");
    vkGetDeviceQueue(device_, indices.graphicsFamily, 0, &graphicsQueue_);
    vkGetDeviceQueue(device_, indices.presentFamily, 0, &presentQueue_);
  }

  void CreateSwapchain() {
    VkSurfaceCapabilitiesKHR capabilities{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice_, surface_, &capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice_, surface_, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice_, surface_, &formatCount, formats.data());

    VkSurfaceFormatKHR surfaceFormat = formats[0];
    for (const auto& format : formats) {
      if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        surfaceFormat = format;
      }
    }

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice_, surface_, &presentModeCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice_, surface_, &presentModeCount, presentModes.data());

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto mode : presentModes) {
      if (mode == VK_PRESENT_MODE_MAILBOX_KHR) presentMode = mode;
    }

    swapchainExtent_ = capabilities.currentExtent;
    if (swapchainExtent_.width == UINT32_MAX) {
      swapchainExtent_.width = kWidth;
      swapchainExtent_.height = kHeight;
    }

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
      imageCount = capabilities.maxImageCount;
    }

    QueueFamilyIndices indices = FindQueueFamilies(physicalDevice_, surface_);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

    VkSwapchainCreateInfoKHR createInfo{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    createInfo.surface = surface_;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = swapchainExtent_;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (indices.graphicsFamily != indices.presentFamily) {
      createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      createInfo.queueFamilyIndexCount = 2;
      createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
      createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    Check(vkCreateSwapchainKHR(device_, &createInfo, nullptr, &swapchain_), "vkCreateSwapchainKHR");

    vkGetSwapchainImagesKHR(device_, swapchain_, &imageCount, nullptr);
    swapchainImages_.resize(imageCount);
    vkGetSwapchainImagesKHR(device_, swapchain_, &imageCount, swapchainImages_.data());
    swapchainImageFormat_ = surfaceFormat.format;
  }

  void CreateImageViews() {
    swapchainImageViews_.resize(swapchainImages_.size());
    for (size_t i = 0; i < swapchainImages_.size(); ++i) {
      VkImageViewCreateInfo createInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
      createInfo.image = swapchainImages_[i];
      createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      createInfo.format = swapchainImageFormat_;
      createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      createInfo.subresourceRange.baseMipLevel = 0;
      createInfo.subresourceRange.levelCount = 1;
      createInfo.subresourceRange.baseArrayLayer = 0;
      createInfo.subresourceRange.layerCount = 1;

      Check(vkCreateImageView(device_, &createInfo, nullptr, &swapchainImageViews_[i]), "vkCreateImageView");
    }
  }

  void CreateRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchainImageFormat_;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    Check(vkCreateRenderPass(device_, &renderPassInfo, nullptr, &renderPass_), "vkCreateRenderPass");
  }

  VkShaderModule CreateShaderModule(const uint32_t* code, size_t bytes) {
    VkShaderModuleCreateInfo createInfo{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    createInfo.codeSize = bytes;
    createInfo.pCode = code;

    VkShaderModule shaderModule;
    Check(vkCreateShaderModule(device_, &createInfo, nullptr, &shaderModule), "vkCreateShaderModule");
    return shaderModule;
  }

  void CreateGraphicsPipeline() {
    VkShaderModule vertShader = CreateShaderModule(kVertexShaderSpv, sizeof(kVertexShaderSpv));
    VkShaderModule fragShader = CreateShaderModule(kFragmentShaderSpv, sizeof(kFragmentShaderSpv));

    VkPipelineShaderStageCreateInfo vertStage{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStage.module = vertShader;
    vertStage.pName = "main";

    VkPipelineShaderStageCreateInfo fragStage{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStage.module = fragShader;
    fragStage.pName = "main";

    VkPipelineShaderStageCreateInfo stages[] = { vertStage, fragStage };

    VkPipelineVertexInputStateCreateInfo vertexInput{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapchainExtent_.width);
    viewport.height = static_cast<float>(swapchainExtent_.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapchainExtent_;

    VkPipelineViewportStateCreateInfo viewportState{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    VkPipelineMultisampleStateCreateInfo multisampling{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlending{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    Check(vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipelineLayout_), "vkCreatePipelineLayout");

    VkGraphicsPipelineCreateInfo pipelineInfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = stages;
    pipelineInfo.pVertexInputState = &vertexInput;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = pipelineLayout_;
    pipelineInfo.renderPass = renderPass_;
    pipelineInfo.subpass = 0;

    Check(vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline_),
          "vkCreateGraphicsPipelines");

    vkDestroyShaderModule(device_, fragShader, nullptr);
    vkDestroyShaderModule(device_, vertShader, nullptr);
  }

  void CreateFramebuffers() {
    swapchainFramebuffers_.resize(swapchainImageViews_.size());

    for (size_t i = 0; i < swapchainImageViews_.size(); ++i) {
      VkImageView attachments[] = { swapchainImageViews_[i] };
      VkFramebufferCreateInfo framebufferInfo{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
      framebufferInfo.renderPass = renderPass_;
      framebufferInfo.attachmentCount = 1;
      framebufferInfo.pAttachments = attachments;
      framebufferInfo.width = swapchainExtent_.width;
      framebufferInfo.height = swapchainExtent_.height;
      framebufferInfo.layers = 1;

      Check(vkCreateFramebuffer(device_, &framebufferInfo, nullptr, &swapchainFramebuffers_[i]), "vkCreateFramebuffer");
    }
  }

  void CreateCommandPool() {
    QueueFamilyIndices indices = FindQueueFamilies(physicalDevice_, surface_);

    VkCommandPoolCreateInfo poolInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    poolInfo.queueFamilyIndex = indices.graphicsFamily;

    Check(vkCreateCommandPool(device_, &poolInfo, nullptr, &commandPool_), "vkCreateCommandPool");
  }

  void CreateCommandBuffers() {
    commandBuffers_.resize(swapchainFramebuffers_.size());

    VkCommandBufferAllocateInfo allocInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    allocInfo.commandPool = commandPool_;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers_.size());

    Check(vkAllocateCommandBuffers(device_, &allocInfo, commandBuffers_.data()), "vkAllocateCommandBuffers");

    for (size_t i = 0; i < commandBuffers_.size(); ++i) {
      VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
      Check(vkBeginCommandBuffer(commandBuffers_[i], &beginInfo), "vkBeginCommandBuffer");

      VkRenderPassBeginInfo renderPassInfo{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
      renderPassInfo.renderPass = renderPass_;
      renderPassInfo.framebuffer = swapchainFramebuffers_[i];
      renderPassInfo.renderArea.offset = { 0, 0 };
      renderPassInfo.renderArea.extent = swapchainExtent_;

      VkClearValue clearColor = { {{{0.05f, 0.05f, 0.1f, 1.0f}}} };
      renderPassInfo.clearValueCount = 1;
      renderPassInfo.pClearValues = &clearColor;

      vkCmdBeginRenderPass(commandBuffers_[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
      vkCmdBindPipeline(commandBuffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline_);
      vkCmdDraw(commandBuffers_[i], 3, 1, 0, 0);
      vkCmdEndRenderPass(commandBuffers_[i]);

      Check(vkEndCommandBuffer(commandBuffers_[i]), "vkEndCommandBuffer");
    }
  }

  void CreateSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    Check(vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &imageAvailable_), "vkCreateSemaphore(imageAvailable)");
    Check(vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &renderFinished_), "vkCreateSemaphore(renderFinished)");
    Check(vkCreateFence(device_, &fenceInfo, nullptr, &inFlightFence_), "vkCreateFence");
  }

  void DrawFrame() {
    Check(vkWaitForFences(device_, 1, &inFlightFence_, VK_TRUE, UINT64_MAX), "vkWaitForFences");
    Check(vkResetFences(device_, 1, &inFlightFence_), "vkResetFences");

    uint32_t imageIndex = 0;
    VkResult result = vkAcquireNextImageKHR(device_, swapchain_, UINT64_MAX, imageAvailable_, VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      return;
    }
    Check(result, "vkAcquireNextImageKHR");

    VkSemaphore waitSemaphores[] = { imageAvailable_ };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signalSemaphores[] = { renderFinished_ };

    VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers_[imageIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    Check(vkQueueSubmit(graphicsQueue_, 1, &submitInfo, inFlightFence_), "vkQueueSubmit");

    VkSwapchainKHR swapchains[] = { swapchain_ };
    VkPresentInfoKHR presentInfo{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(presentQueue_, &presentInfo);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR && result != VK_ERROR_OUT_OF_DATE_KHR) {
      Check(result, "vkQueuePresentKHR");
    }
  }

  static void Check(VkResult result, const char* operation) {
    if (result != VK_SUCCESS) {
      throw std::runtime_error(std::string(operation) + " failed with error code " + std::to_string(result));
    }
  }
};

}  // namespace

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int) {
  try {
    VulkanTriangleApp app;
    app.Run(hInstance);
    return 0;
  } catch (const std::exception& e) {
    MessageBoxA(nullptr, e.what(), "Error", MB_OK | MB_ICONERROR);
    return 1;
  }
}
