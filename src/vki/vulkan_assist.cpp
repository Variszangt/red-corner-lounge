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
    assert(command_buffer);
    
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

std::pair<vk::UniqueImage, vk::UniqueDeviceMemory> create_image(
    const DeviceWrapper& device_wrapper,
    const ImageCreateInfo& createinfo)
{
    const auto device = device_wrapper.device.get();
    assert(device);

    // Create image:
    const vk::ImageCreateInfo image_createinfo {
        .imageType  = vk::ImageType::e2D,
        .format     = createinfo.format,
        .extent     = {
            .width  = static_cast<uint32_t>(createinfo.width),
            .height = static_cast<uint32_t>(createinfo.height),
            .depth  = 1
        },
        .mipLevels      = createinfo.mip_levels,
        .arrayLayers    = 1,
        .samples        = createinfo.samples,
        .tiling         = vk::ImageTiling::eOptimal,
        .usage          = createinfo.usage,
        .sharingMode    = vk::SharingMode::eExclusive,
        .initialLayout  = vk::ImageLayout::eUndefined,
    };
    auto image = device.createImageUnique(image_createinfo);

    // Allocate memory:
    const auto memory_requirements = device.getImageMemoryRequirements(image.get());
    const uint32_t memory_type_index = device_wrapper.get_memory_type_index(
        memory_requirements.memoryTypeBits, createinfo.properties);

    const vk::MemoryAllocateInfo allocate_info {
        .allocationSize     = memory_requirements.size,
        .memoryTypeIndex    = memory_type_index,
    };
    auto image_memory = device.allocateMemoryUnique(allocate_info);

    // Bind and return:
    vkBindImageMemory(device, image.get(), image_memory.get(), 0);
    return std::make_pair(std::move(image), std::move(image_memory));
}

void set_image_layout(
    const DeviceWrapper& device_wrapper,
    const ImageLayoutTransitionInfo& transition_info)
{
    auto device         = device_wrapper.device.get();
    auto command_pool   = device_wrapper.command_pools.graphics.get();
    auto graphics_queue = device_wrapper.queues.graphics;

    assert(device);
    assert(command_pool);
    assert(graphics_queue);

    // Create an image-memory-barrier object
    vk::ImageMemoryBarrier barrier {
        .oldLayout          = transition_info.old_layout,
        .newLayout          = transition_info.new_layout,
        .image              = transition_info.image,
        .subresourceRange   = transition_info.subresource_range,
    };

    // Old layout, source access mask:
    switch (transition_info.old_layout)
    {
    case vk::ImageLayout::ePreinitialized:
        barrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
        break;
    case vk::ImageLayout::eColorAttachmentOptimal:
        barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead;
        break;
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
        barrier.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        break;
    case vk::ImageLayout::eTransferSrcOptimal:
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        break;
    case vk::ImageLayout::eTransferDstOptimal:
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        break;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
        barrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
        break;
    default:
        assert(!"unknown source layout");
        break;
    }

    // New layout, destination access mask:
    switch (transition_info.new_layout)
    {
    case vk::ImageLayout::eTransferDstOptimal:
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
        break;
    case vk::ImageLayout::eTransferSrcOptimal:
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
        break;
    case vk::ImageLayout::eColorAttachmentOptimal:
        barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead;
        break;
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
        barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        break;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        break;
    default:
        assert(!"unknown source layout");
        break;
    }

    SingleTimeCommandBuffer cmdbuf { device, command_pool, graphics_queue };
    cmdbuf->pipelineBarrier(
        transition_info.src_stage_mask,
        transition_info.dst_stage_mask,
        vk::DependencyFlags {},
        std::vector<vk::MemoryBarrier> {},
        std::vector<vk::BufferMemoryBarrier> {},
        std::vector<vk::ImageMemoryBarrier> { barrier });
    cmdbuf.submit();
}

}