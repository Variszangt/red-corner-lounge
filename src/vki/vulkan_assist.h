#pragma once

#include <vulkan/vulkan.hpp>

#include "vulkan_device.h"

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
    void submit(); // Blocks until finished.

    vk::CommandBuffer* operator->() { return &cmdbuf.get(); }

private:
    vk::Device              device;
    vk::CommandPool         command_pool;
    vk::Queue               queue;
    vk::UniqueCommandBuffer cmdbuf;
};

/*------------------------------------------------------------------*/
// Buffers:

struct BufferWrapper
{
    vk::UniqueBuffer        buffer;
    vk::UniqueDeviceMemory  memory;
    vk::DeviceSize          size;

    vk::Buffer get() const { return buffer.get(); }
};

BufferWrapper create_buffer(
    const DeviceWrapper&            device_wrapper,
    const vk::DeviceSize            size,
    const vk::BufferUsageFlags      usage,
    const vk::MemoryPropertyFlags   mem_properties);

void copy_buffer(
    const DeviceWrapper&    device_wrapper,
    const vk::Buffer        src,
    const vk::Buffer        dst,
    const vk::DeviceSize    size);

/*------------------------------------------------------------------*/
// Images:

vk::ImageSubresourceRange create_ISR(
    const vk::ImageAspectFlags  image_aspect,
    const uint32_t              mip_levels          = 1,
    const uint32_t              layers              = 1,
    const uint32_t              base_mip_level      = 0,
    const uint32_t              base_layer_level    = 0);

struct ImageWrapper
{
    vk::UniqueImage         image;
    vk::UniqueDeviceMemory  memory;
    vk::ImageLayout         layout;
    vk::ImageAspectFlags    aspect;
    vk::Format              format;
    vk::Extent2D            size;
    uint32_t                mip_levels;

    vk::Image get() const { return image.get(); }
};

struct ImageCreateInfo
{
    vk::Format              format;
    vk::Extent2D            size;
    uint32_t                mip_levels;
    vk::SampleCountFlagBits samples;
    vk::ImageUsageFlags     usage; // Usage flags are also used to derive aspect flags.
    vk::MemoryPropertyFlags mem_properties;
    vk::ImageLayout         initial_layout = vk::ImageLayout::eUndefined; // set_image_layout() is immediately called on the created ImageWrapper if this is set to anything other than Undefined.
};
ImageWrapper create_image(const DeviceWrapper& device_wrapper, const ImageCreateInfo& createinfo);

void set_image_layout(
    const DeviceWrapper&            device_wrapper,
    ImageWrapper&                   image_wrapper,
    const vk::ImageLayout           new_layout,
    const vk::PipelineStageFlags    src_stage_mask, 
    const vk::PipelineStageFlags    dst_stage_mask); // Since this is a blocking function, dst_stage_mask is technically redundant, but should nevertheless be provided for completeness and clarity.

void copy_buffer_to_image(
    const DeviceWrapper&    device_wrapper,
    const vk::Buffer        buffer,
    ImageWrapper&           image_wrapper);

// Generates mipmaps for an existing and filled image.
void create_mipmaps(const DeviceWrapper& device_wrapper, ImageWrapper& image_wrapper);

vk::UniqueImageView create_image_view(
    const DeviceWrapper&            device_wrapper,
    const vk::Image                 image,
    const vk::Format                format,
    const vk::ImageSubresourceRange range);

}
