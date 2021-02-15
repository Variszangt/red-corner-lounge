#pragma once

#include <vulkan/vulkan.hpp>

#include <vki/vulkan_device.h>

/*------------------------------------------------------------------*/
// The functions in this unit are simple at the cost of performance. Use of these functions should be limited to prototyping only.

namespace vki
{
/*------------------------------------------------------------------*/
// Sync:

// Creates a (signaled) fence.
vk::UniqueFence create_fence(const vk::Device device);

/*------------------------------------------------------------------*/
// SingleTimeCommandBuffer:

// Use to record and submit single time commands.
class SingleTimeCommandBuffer
{
public:
    SingleTimeCommandBuffer(const vk::Device device, const vk::CommandPool command_pool, const vk::Queue queue); // Allocates a new command buffer on the specified command pool and begins recording. Queued (->) commands will be submitted to the specified queue upon calling submit().
    void submit();

    vk::CommandBuffer* operator->() { return &command_buffer.get(); }

private:
    vk::Device device;
    vk::CommandPool command_pool;
    vk::Queue queue;

    vk::UniqueCommandBuffer command_buffer;
};

/*------------------------------------------------------------------*/
// Buffers:

std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory> create_buffer(
    const DeviceWrapper& device_wrapper,
    const vk::DeviceSize size,
    const vk::BufferUsageFlags usage,
    const vk::MemoryPropertyFlags properties)
{
    const auto device = device_wrapper.device.get();
    assert(device);

    // Create buffer:
    const vk::BufferCreateInfo buffer_createinfo {
        .size           = size,
        .usage          = usage,
        .sharingMode    = vk::SharingMode::eExclusive,
    };
    auto buffer = device.createBufferUnique(buffer_createinfo);

    // Allocate required buffer memory:
    const auto memory_requirements = device.getBufferMemoryRequirements(buffer.get());
    const vk::MemoryAllocateInfo allocate_info {
        .allocationSize     = memory_requirements.size,
        .memoryTypeIndex    = device_wrapper.get_memory_type_index(memory_requirements.memoryTypeBits, properties),
    };
    auto buffer_memory = device.allocateMemoryUnique(allocate_info);

    // Bind and return:
    device.bindBufferMemory(buffer.get(), buffer_memory.get(), 0);
    return std::make_pair(std::move(buffer), std::move(buffer_memory));
}

void copy_buffer(
    const DeviceWrapper& device_wrapper,
    const vk::Buffer src,
    const vk::Buffer dst,
    const vk::DeviceSize size)
{
    auto device         = device_wrapper.device.get();
    auto command_pool   = device_wrapper.command_pools.transfer.get();
    auto transfer_queue = device_wrapper.queues.transfer;

    assert(device);
    assert(command_pool);
    assert(transfer_queue);

    SingleTimeCommandBuffer cmdbuf { device, command_pool, transfer_queue };
    cmdbuf->copyBuffer(src, dst, std::vector<vk::BufferCopy> { {.size = size } });
    cmdbuf.submit();
}
}