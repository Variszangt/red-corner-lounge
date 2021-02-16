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
    const vk::MemoryPropertyFlags properties);

void copy_buffer(
    const DeviceWrapper& device_wrapper,
    const vk::Buffer src,
    const vk::Buffer dst,
    const vk::DeviceSize size);

/*------------------------------------------------------------------*/
// Images:

struct ImageCreateInfo
{
    uint32_t width, height;
    uint32_t mip_levels;
    vk::Format format;
    vk::SampleCountFlagBits samples;
    vk::ImageUsageFlags usage;
    vk::MemoryPropertyFlags properties;
    vk::ImageLayout initial_layout = vk::ImageLayout::eUndefined;
};

std::pair<vk::UniqueImage, vk::UniqueDeviceMemory> create_image(
    const DeviceWrapper& device_wrapper,
    const ImageCreateInfo& createinfo);

struct ImageLayoutTransitionInfo
{
    vk::Image image;
    vk::ImageLayout old_layout;
    vk::ImageLayout new_layout;
    vk::ImageSubresourceRange subresource_range;
    vk::PipelineStageFlags src_stage_mask;
    vk::PipelineStageFlags dst_stage_mask;
};

void setImageLayout(
    const DeviceWrapper& device_wrapper,
    const ImageLayoutTransitionInfo& transition_info)
{
    auto device         = device_wrapper.device.get();
    auto command_pool   = device_wrapper.command_pools.graphics.get();
    auto queue          = device_wrapper.queues.graphics;
        
    assert(device);
    assert(command_pool);
    assert(transfer_queue);
    
    // Create an image barrier object
    vk::ImageMemoryBarrier barrier {
        .oldLayout          = transition_info.old_layout,
        .newLayout          = transition_info.new_layout,
        .image              = transition_info.image,
        .subresourceRange   = transition_info.subresource_range,
    };

    // Old layout, source mask:
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

    // New layout, destination mask:
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
        if (!barrier.srcAccessMask)
            barrier.srcAccessMask = vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        break;
    default:
        assert(!"unknown source layout");
        break;
    }

    SingleTimeCommandBuffer cmdbuf { device_wrapper.device, device_wrapper.command_pools.graphics, device_wrapper.queues.graphics };
    
        
    }
    // Put barrier inside setup command buffer
    vkCmdPipelineBarrier(
        cmdbuffer,
        src_stage_mask,
        dstStageMask,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier);
}

void transition_image_layout(
    vk::Image image,
    vk::Format /*format*/,
    vk::ImageLayout old_layout,
    vk::ImageLayout new_layout,
    uint32_t mip_levels)
{
    assert(device);
    assert(command_pool);
    assert(graphics_queue);

    vk::PipelineStageFlags src_stage;
    vk::PipelineStageFlags dst_stage;
    vk::AccessFlags src_access_mask;
    vk::AccessFlags dst_access_mask;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;

        src_access_mask = 0;
        dst_access_mask = VK_ACCESS_TRANSFER_WRITE_BIT;
    }
    else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

        src_access_mask = VK_ACCESS_TRANSFER_WRITE_BIT;
        dst_access_mask = VK_ACCESS_SHADER_READ_BIT;
    }
    else
        THROW_ERROR("unknown image layout transition: {} -> {}", old_layout, new_layout);

    vk::CommandBuffer command_buffer = begin_single_time_commands();

    const vk::ImageMemoryBarrier barrier {
        .sType                  = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask          = src_access_mask,
        .dstAccessMask          = dst_access_mask,
        .oldLayout              = old_layout,
        .newLayout              = new_layout,
        .srcQueueFamilyIndex    = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex    = VK_QUEUE_FAMILY_IGNORED,
        .image                  = image,
        .subresource_range       = {
            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel   = 0,
            .levelCount     = mip_levels,
            .baseArrayLayer = 0,
            .layerCount     = 1
        }
    };

    vkCmdPipelineBarrier(
        command_buffer,
        src_stage, dst_stage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    end_single_time_commands(command_buffer);
}

void copy_buffer_to_image(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
{
    assert(device);
    assert(command_pool);
    assert(graphics_queue);

    vk::CommandBuffer command_buffer = begin_single_time_commands();

    const vk::BufferImageCopy region {
        .bufferOffset       = 0,
        .bufferRowLength    = 0,
        .bufferImageHeight  = 0,
        .imageSubresource   = {
            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel       = 0,
            .baseArrayLayer = 0,
            .layerCount     = 1
        },
        .imageOffset = { 0, 0, 0 },
        .imageExtent = { width, height, 1 }
    };

    vkCmdCopyBufferToImage(
        command_buffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    end_single_time_commands(command_buffer);
}

void create_mipmaps(vk::Image image, vk::Format format, int32_t width, int32_t height, uint32_t mip_levels)
{
    assert(vulkan_physical_device);
    assert(device);
    assert(command_pool);
    assert(graphics_queue);

    vk::FormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(vulkan_physical_device, format, &format_properties);
    if (!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        THROW_ERROR("image format does not support linear blitting");

    /*------------------------------------------------------------------*/

    vk::CommandBuffer command_buffer = begin_single_time_commands();

    vk::ImageMemoryBarrier barrier {
        .sType                  = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcQueueFamilyIndex    = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex    = VK_QUEUE_FAMILY_IGNORED,
        .image                  = image,
        .subresource_range {
            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount     = 1,
            .baseArrayLayer = 0,
            .layerCount     = 1,
        }
    };

    int32_t mip_width   = width;
    int32_t mip_height  = height;

    for (uint32_t i = 1; i < mip_levels; ++i)
    {
        /*------------------------------------------------------------------*/
        // Transition the previous mipmap to be transfered from:

        barrier.subresource_range.baseMipLevel = i - 1;
        barrier.oldLayout       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout       = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask   = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask   = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(
            command_buffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

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
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel       = i - 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
            .srcOffsets = { src_offsets[0], src_offsets[1] },
            .dstSubresource {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel       = i,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
            .dstOffsets = { dst_offsets[0], dst_offsets[1] },
        };

        vkCmdBlitImage(
            command_buffer,
            image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &blit,
            VK_FILTER_LINEAR
        );

        /*------------------------------------------------------------------*/
        // Transition the previous mipmap to be read-by-shader:

        barrier.oldLayout       = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask   = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            command_buffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        /*------------------------------------------------------------------*/

        if (mip_width  > 1) mip_width  /= 2;
        if (mip_height > 1) mip_height /= 2;
    }

    /*------------------------------------------------------------------*/
    // Transition the last mipmap directly to be read-by-shader:

    barrier.subresource_range.baseMipLevel = mip_levels - 1;
    barrier.oldLayout       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask   = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(
        command_buffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    end_single_time_commands(command_buffer);
}
}
