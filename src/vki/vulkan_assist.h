#pragma once

#include <vulkan/vulkan.hpp>

/*------------------------------------------------------------------*/
// Sync:

// Creates a (signaled) fence.
vk::UniqueFence create_fence(const vk::Device device);

/*------------------------------------------------------------------*/
// SingleTimeCommandBuffer:

// Use to record and submit single time commands. Simple and slow. Use only for non-performant code.
class SingleTimeCommandBuffer
{
public:
    SingleTimeCommandBuffer(const vk::Device device, const vk::CommandPool command_pool, const vk::Queue queue);

    vk::CommandBuffer begin();
    void submit();

private:
    vk::Device device;
    vk::CommandPool command_pool;
    vk::Queue queue;
    
    vk::UniqueCommandBuffer command_buffer;
};

/*------------------------------------------------------------------*/

// std::pair<VkBuffer, VkDeviceMemory> VulkanApp::create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
// {
//     assert(device);

//     VkBuffer buffer              = VK_NULL_HANDLE;
//     VkDeviceMemory buffer_memory = VK_NULL_HANDLE;

//     /*------------------------------------------------------------------*/
//     // Create buffer:

//     const VkBufferCreateInfo buffer_createinfo {
//         .sType          = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
//         .size           = size,
//         .usage          = usage,
//         .sharingMode    = VK_SHARING_MODE_EXCLUSIVE
//     };

//     VkResult result = vkCreateBuffer(device, &buffer_createinfo, nullptr, &buffer);
//     if (result != VK_SUCCESS)
//         THROW_ERROR("buffer could not be created; VkResult: {}", result);

//     /*------------------------------------------------------------------*/
//     // Allocate required buffer memory:

//     VkMemoryRequirements memory_requirements;
//     vkGetBufferMemoryRequirements(device, buffer, &memory_requirements);

//     const VkMemoryAllocateInfo allocate_info {
//         .sType              = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
//         .allocationSize     = memory_requirements.size,
//         .memoryTypeIndex    = get_memory_type_index(memory_requirements.memoryTypeBits, properties)
//     };

//     result = vkAllocateMemory(device, &allocate_info, nullptr, &buffer_memory);
//     if (result != VK_SUCCESS)
//         THROW_ERROR("buffer memory could not be allocated; VkResult: {}", result);

//     /*------------------------------------------------------------------*/
//     // Bind buffer to memory:

//     vkBindBufferMemory(device, buffer, buffer_memory, 0);

//     return { buffer, buffer_memory };
// }

// void VulkanApp::copy_buffer(VkBuffer src, VkBuffer dst, VkDeviceSize size)
// {
//     assert(device);
//     assert(command_pool);
//     assert(graphics_queue);

//     auto command_buffer = begin_single_time_commands();

//     const VkBufferCopy copy_region { .size = size };
//     vkCmdCopyBuffer(command_buffer, src, dst, 1, &copy_region);

//     end_single_time_commands(command_buffer);
// }