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
    const vk::Device        device,
    const vk::CommandPool   command_pool,
    const vk::Queue         queue) :
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
    auto buffers = device.allocateCommandBuffersUnique(allocate_info);
    cmdbuf = std::move(buffers.front());

    const vk::CommandBufferBeginInfo begin_info {
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
    };
    cmdbuf->begin(begin_info);
}

void SingleTimeCommandBuffer::submit()
{
    assert(device);
    assert(command_pool);
    assert(queue);
    assert(cmdbuf);

    cmdbuf->end();

    const vk::SubmitInfo submit_info {
        .commandBufferCount = 1,
        .pCommandBuffers    = &cmdbuf.get(),
    };
    const auto fence = create_fence(device);
    queue.submit(std::vector<vk::SubmitInfo> { submit_info }, fence.get());

    const auto res = device.waitForFences(std::vector<vk::Fence>{ fence.get() }, VK_TRUE, UINT64_MAX);
    if (res != vk::Result::eSuccess)
        THROW_ERROR("unexpected lack of success: {}", res);

    cmdbuf.reset();
}

BufferWrapper create_buffer(
    const DeviceWrapper&            device_wrapper,
    const vk::DeviceSize            size,
    const vk::BufferUsageFlags      usage,
    const vk::MemoryPropertyFlags   mem_properties)
{
    const auto device = device_wrapper.get();
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
        .memoryTypeIndex    = device_wrapper.get_memory_type_index(memory_requirements.memoryTypeBits, mem_properties),
    };
    auto memory = device.allocateMemoryUnique(allocate_info);

    // Bind:
    device.bindBufferMemory(buffer.get(), memory.get(), 0);

    // Return:
    return BufferWrapper {
        .buffer = std::move(buffer),
        .memory = std::move(memory),
        .size   = size,
    };
}

void copy_buffer(
    const DeviceWrapper&    device_wrapper,
    const vk::Buffer        src,
    const vk::Buffer        dst,
    const vk::DeviceSize    size)
{
    auto device         = device_wrapper.get();
    auto command_pool   = device_wrapper.command_pools.transfer.get();
    auto transfer_queue = device_wrapper.queues.transfer;

    assert(device);
    assert(command_pool);
    assert(transfer_queue);

    SingleTimeCommandBuffer cmdbuf { device, command_pool, transfer_queue };
    cmdbuf->copyBuffer(src, dst, std::vector<vk::BufferCopy> { {.size = size } });
    cmdbuf.submit();
}

vk::ImageSubresourceRange create_ISR(
    const vk::ImageAspectFlags  image_aspect,
    const uint32_t              mip_levels,
    const uint32_t              layers,
    const uint32_t              base_mip_level,
    const uint32_t              base_layer_level)
{
    return vk::ImageSubresourceRange {
        .aspectMask     = image_aspect,
        .baseMipLevel   = base_mip_level,
        .levelCount     = mip_levels,
        .baseArrayLayer = base_layer_level,
        .layerCount     = layers,
    };
}

ImageWrapper create_image(const DeviceWrapper& device_wrapper, const ImageCreateInfo& createinfo)
{
    const auto device = device_wrapper.get();
    assert(device);

    // Create image:
    const vk::ImageCreateInfo image_createinfo {
        .imageType  = vk::ImageType::e2D,
        .format     = createinfo.format,
        .extent     = {
            .width  = static_cast<uint32_t>(createinfo.size.width),
            .height = static_cast<uint32_t>(createinfo.size.height),
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
        memory_requirements.memoryTypeBits, createinfo.mem_properties);

    const vk::MemoryAllocateInfo allocate_info {
        .allocationSize     = memory_requirements.size,
        .memoryTypeIndex    = memory_type_index,
    };
    auto memory = device.allocateMemoryUnique(allocate_info);

    // Derive aspect flags:
    vk::ImageAspectFlags aspect_flags;

    if (createinfo.usage | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled)
        aspect_flags |= vk::ImageAspectFlagBits::eColor;
    if (createinfo.usage | vk::ImageUsageFlagBits::eDepthStencilAttachment)
        aspect_flags |= vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;

    // Bind:
    vkBindImageMemory(device, image.get(), memory.get(), 0);
    ImageWrapper image_wrapper {
        .image      = std::move(image),
        .memory     = std::move(memory),
        .layout     = vk::ImageLayout::eUndefined,
        .aspect     = aspect_flags,
        .format     = createinfo.format,
        .size       = createinfo.size,
        .mip_levels = createinfo.mip_levels,
    };

    // Transfer layout if specified:
    if (createinfo.initial_layout != vk::ImageLayout::eUndefined)
        set_image_layout(device_wrapper, image_wrapper, createinfo.initial_layout,
        vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eAllCommands);

    // Return:
    return std::move(image_wrapper);
}

void set_image_layout(
    const DeviceWrapper&            device_wrapper,
    ImageWrapper&                   image_wrapper,
    const vk::ImageLayout           new_layout,
    const vk::PipelineStageFlags    src_stage_mask,
    const vk::PipelineStageFlags    dst_stage_mask)
{
    auto device         = device_wrapper.get();
    auto command_pool   = device_wrapper.command_pools.graphics.get();
    auto graphics_queue = device_wrapper.queues.graphics;
    auto image          = image_wrapper.get();

    assert(device);
    assert(command_pool);
    assert(graphics_queue);
    assert(image);

    const auto old_layout = image_wrapper.layout;

    vk::ImageMemoryBarrier barrier {
        .oldLayout          = old_layout,
        .newLayout          = new_layout,
        .image              = image,
        .subresourceRange   = create_ISR(image_wrapper.aspect, image_wrapper.mip_levels),
    };

    // Old layout, source access mask:
    switch (old_layout)
    {
    case vk::ImageLayout::eUndefined:
        barrier.srcAccessMask = {};
        break;
    case vk::ImageLayout::ePreinitialized:
        barrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
        break;
    case vk::ImageLayout::eColorAttachmentOptimal:
        barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
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
    switch (new_layout)
    {
    case vk::ImageLayout::eTransferDstOptimal:
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
        break;
    case vk::ImageLayout::eTransferSrcOptimal:
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
        break;
    case vk::ImageLayout::eColorAttachmentOptimal:
        barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
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
        src_stage_mask,
        dst_stage_mask,
        vk::DependencyFlags {},
        std::vector<vk::MemoryBarrier> {},
        std::vector<vk::BufferMemoryBarrier> {},
        std::vector<vk::ImageMemoryBarrier> { barrier });
    cmdbuf.submit();

    image_wrapper.layout = new_layout;
}

void copy_buffer_to_image(
    const DeviceWrapper&    device_wrapper,
    const vk::Buffer        buffer,
    ImageWrapper&           image_wrapper)
{
    auto device         = device_wrapper.get();
    auto command_pool   = device_wrapper.command_pools.graphics.get();
    auto graphics_queue = device_wrapper.queues.graphics;
    auto image          = image_wrapper.get();

    assert(device);
    assert(command_pool);
    assert(graphics_queue);
    assert(image);

    // Assure correct layout:
    const auto previous_layout = image_wrapper.layout;
    if (image_wrapper.layout != vk::ImageLayout::eTransferDstOptimal)
        set_image_layout(device_wrapper, image_wrapper, vk::ImageLayout::eTransferDstOptimal,
                         vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eTransfer);

    // Copy:
    const vk::BufferImageCopy region {
        .imageSubresource   = {.layerCount = 1 },
        .imageExtent        = { image_wrapper.size.width, image_wrapper.size.height, 1 }
    };
    SingleTimeCommandBuffer cmdbuf { device, command_pool, graphics_queue };
    cmdbuf->copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, { region });
    cmdbuf.submit();

    // Reverse layout if necessary:
    if (image_wrapper.layout != previous_layout)
        set_image_layout(device_wrapper, image_wrapper, previous_layout,
                         vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eAllCommands);
}

void create_mipmaps(const DeviceWrapper& device_wrapper, ImageWrapper& image_wrapper)
{
    auto device         = device_wrapper.device.get();
    auto command_pool   = device_wrapper.command_pools.graphics.get();
    auto graphics_queue = device_wrapper.queues.graphics;
    auto image          = image_wrapper.get();

    assert(device);
    assert(command_pool);
    assert(graphics_queue);
    assert(image);

    const auto format_properties = device_wrapper.physical_device.getFormatProperties(image_wrapper.format);
    if (!(format_properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
        THROW_ERROR("image format does not support linear blitting");

    vk::ImageMemoryBarrier barrier {
        .srcQueueFamilyIndex    = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex    = VK_QUEUE_FAMILY_IGNORED,
        .image                  = image,
        .subresourceRange       = create_ISR(vk::ImageAspectFlagBits::eColor),
    };

    int32_t mip_width   = static_cast<int32_t>(image_wrapper.size.width);
    int32_t mip_height  = static_cast<int32_t>(image_wrapper.size.height);

    SingleTimeCommandBuffer cmdbuf { device, command_pool, graphics_queue };

    for (uint32_t i = 1; i < image_wrapper.mip_levels; ++i)
    {
        /*------------------------------------------------------------------*/
        // Transition the previous mipmap to be transfered from:

        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout       = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout       = vk::ImageLayout::eTransferSrcOptimal;
        barrier.srcAccessMask   = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask   = vk::AccessFlagBits::eTransferRead;

        cmdbuf->pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eTransfer,
            vk::DependencyFlags {},
            std::vector<vk::MemoryBarrier> {},
            std::vector<vk::BufferMemoryBarrier> {},
            std::vector<vk::ImageMemoryBarrier> { barrier }
        );

        /*------------------------------------------------------------------*/
        // Blit previous mipmap to the next at 1/2 extent:

        const vk::Offset3D src_offsets[2] {
            { 0, 0, 0 },
            { mip_width, mip_height, 1 },
        };

        const vk::Offset3D dst_offsets[2] {
            { 0, 0, 0 },
            { mip_width > 1 ? mip_width / 2 : 1, mip_height > 1 ? mip_height / 2 : 1, 1 }
        };

        const vk::ImageBlit blit {
            .srcSubresource {
                .aspectMask     = vk::ImageAspectFlagBits::eColor,
                .mipLevel       = i - 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
            .srcOffsets = std::array<vk::Offset3D, 2> { src_offsets[0], src_offsets[1] },
            .dstSubresource {
                .aspectMask     = vk::ImageAspectFlagBits::eColor,
                .mipLevel       = i,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
            .dstOffsets = std::array<vk::Offset3D, 2> { dst_offsets[0], dst_offsets[1] },
        };

        cmdbuf->blitImage(
            image, vk::ImageLayout::eTransferSrcOptimal,
            image, vk::ImageLayout::eTransferDstOptimal,
            { blit },
            vk::Filter::eLinear
        );

        /*------------------------------------------------------------------*/
        // Transition the previous mipmap to be read-by-shader:

        barrier.oldLayout       = vk::ImageLayout::eTransferSrcOptimal;
        barrier.newLayout       = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcAccessMask   = vk::AccessFlagBits::eTransferRead;
        barrier.dstAccessMask   = vk::AccessFlagBits::eShaderRead;

        cmdbuf->pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eFragmentShader,
            vk::DependencyFlags {},
            std::vector<vk::MemoryBarrier> {},
            std::vector<vk::BufferMemoryBarrier> {},
            std::vector<vk::ImageMemoryBarrier> { barrier }
        );

        /*------------------------------------------------------------------*/

        if (mip_width  > 1) mip_width  /= 2;
        if (mip_height > 1) mip_height /= 2;
    }

    /*------------------------------------------------------------------*/
    // Transition the last mipmap directly to be read-by-shader:

    barrier.subresourceRange.baseMipLevel = image_wrapper.mip_levels - 1;
    barrier.oldLayout       = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout       = vk::ImageLayout::eShaderReadOnlyOptimal;
    barrier.srcAccessMask   = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask   = vk::AccessFlagBits::eShaderRead;

    cmdbuf->pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader,
        vk::DependencyFlags {},
        std::vector<vk::MemoryBarrier> {},
        std::vector<vk::BufferMemoryBarrier> {},
        std::vector<vk::ImageMemoryBarrier> { barrier }
    );

    cmdbuf.submit();
}

vk::UniqueImageView create_image_view(
    const DeviceWrapper&            device_wrapper,
    const vk::Image                 image,
    const vk::Format                format,
    const vk::ImageSubresourceRange range)
{
    auto device = device_wrapper.get();
    assert(device);

    const vk::ImageViewCreateInfo createinfo {
        .image              = image,
        .viewType           = vk::ImageViewType::e2D,
        .format             = format,
        .subresourceRange   = range,
    };
    return device.createImageViewUnique(createinfo);
}

}