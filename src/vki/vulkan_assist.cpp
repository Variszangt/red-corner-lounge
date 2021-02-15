#include "vulkan_assist.h"

#include "error.h"

namespace vki
{
vk::UniqueFence create_fence(const vk::Device device)
{
    vk::FenceCreateInfo createinfo {
        .flags = vk::FenceCreateFlagBits::eSignaled
    };
    return device.createFenceUnique(createinfo);
}

SingleTimeCommandBuffer::SingleTimeCommandBuffer(
    const vk::Device device,
    const vk::CommandPool command_pool,
    const vk::Queue queue
) :
    device { device },
    command_pool { command_pool },
    queue { queue }
{
    assert(device);
    assert(command_pool);
    assert(queue);

    const vk::CommandBufferAllocateInfo allocate_info {
        .commandPool        = command_pool,
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1,
    };
    auto command_buffers = device.allocateCommandBuffersUnique(allocate_info);
    command_buffer = std::move(command_buffers.front());

    const vk::CommandBufferBeginInfo begin_info {
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
    };
    command_buffer->begin(begin_info);
}

void SingleTimeCommandBuffer::submit()
{
    assert(device);
    assert(command_pool);
    assert(queue);

    command_buffer->end();

    const vk::SubmitInfo submit_info {
        .commandBufferCount = 1,
        .pCommandBuffers    = &command_buffer.get(),
    };
    const auto fence = create_fence(device);
    queue.submit(std::vector<vk::SubmitInfo> { submit_info }, fence.get());

    const auto res = device.waitForFences(std::vector<vk::Fence>{ fence.get() }, VK_TRUE, UINT64_MAX);
    if (res != vk::Result::eSuccess)
        THROW_ERROR("unexpected lack of success: {}", res);
}
}