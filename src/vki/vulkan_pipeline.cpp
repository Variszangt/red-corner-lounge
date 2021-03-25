#include "vulkan_pipeline.h"

#include "vulkan_debug.h"
#include "vulkan_assist.h"
#include "vulkan_renderpass.h"

namespace vki
{
vk::UniqueDescriptorSetLayout create_descriptor_set_layout(const DeviceWrapper& device_wrapper)
{
    auto device = device_wrapper.get();
    assert(device);

    const vk::DescriptorSetLayoutBinding ubo_layout_binding {
        .binding            = 0,
        .descriptorType     = vk::DescriptorType::eUniformBuffer,
        .descriptorCount    = 1,
        .stageFlags         = vk::ShaderStageFlagBits::eVertex,
    };

    const std::vector<vk::DescriptorSetLayoutBinding> bindings {
        ubo_layout_binding,
    };

    const vk::DescriptorSetLayoutCreateInfo createinfo {
        .bindingCount   = static_cast<uint32_t>(bindings.size()),
        .pBindings      = bindings.data()
    };
    return device.createDescriptorSetLayoutUnique(createinfo);
}

PipelineWrapper create_world_pipeline(
    const DeviceWrapper&    device_wrapper,
    const vk::Format        color_format,
    const vk::Format        depth_stencil_format,
    const vk::Extent2D      initial_extent)
{
    auto device = device_wrapper.get();
    assert(device);

    /*------------------------------------------------------------------*/
    // Shader stages:

    auto vert_shader_module = create_shader_module(device_wrapper, "assets/shaders/world.vert.spv");
    auto frag_shader_module = create_shader_module(device_wrapper, "assets/shaders/world.frag.spv");
    
    const vk::PipelineShaderStageCreateInfo vert_shader_createinfo {
        .stage  = vk::ShaderStageFlagBits::eVertex,
        .module = vert_shader_module.get(),
        .pName  = "main",
    };

    const vk::PipelineShaderStageCreateInfo frag_shader_createinfo {
        .stage  = vk::ShaderStageFlagBits::eFragment,
        .module = frag_shader_module.get(),
        .pName  = "main",
    };

    const std::vector<vk::PipelineShaderStageCreateInfo> shader_stages {
        vert_shader_createinfo,
        frag_shader_createinfo
    };

    /*------------------------------------------------------------------*/
    // Fixed function state:

    const auto binding_description    = Vertex::get_binding_description();
    const auto attribute_descriptions = Vertex::get_attribute_descriptions();

    const vk::PipelineVertexInputStateCreateInfo vertex_input_createinfo {
        .vertexBindingDescriptionCount      = 1,
        .pVertexBindingDescriptions         = &binding_description,
        .vertexAttributeDescriptionCount    = static_cast<uint32_t>(attribute_descriptions.size()),
        .pVertexAttributeDescriptions       = attribute_descriptions.data(),
    };

    // Our vertices represent a triangle:
    const vk::PipelineInputAssemblyStateCreateInfo input_assembly_createinfo {
        .topology               = vk::PrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = VK_FALSE
    };

    // Viewport and scissor:
    const vk::Viewport viewport {
        .x          = 0.0f,
        .y          = 0.0f,
        .width      = static_cast<float>(initial_extent.width),
        .height     = static_cast<float>(initial_extent.height),
        .minDepth   = 0.f,
        .maxDepth   = 1.f
    };

    const vk::Rect2D scissor {
        .offset = { 0, 0 },
        .extent = initial_extent,
    };

    const vk::PipelineViewportStateCreateInfo viewport_createinfo {
        .viewportCount  = 1,
        .pViewports     = &viewport,
        .scissorCount   = 1,
        .pScissors      = &scissor
    };

    // Rasterizer:
    const vk::PipelineRasterizationStateCreateInfo rasterization_createinfo {
        .depthClampEnable           = VK_FALSE,
        .rasterizerDiscardEnable    = VK_FALSE,
        .polygonMode                = vk::PolygonMode::eFill,
        .cullMode                   = vk::CullModeFlagBits::eBack,
        .frontFace                  = vk::FrontFace::eCounterClockwise,
        .depthBiasEnable            = VK_FALSE,
        .lineWidth                  = 1.f
    };

    // Multisampling:
    const vk::PipelineMultisampleStateCreateInfo multisampling_createinfo {
        .rasterizationSamples   = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable    = VK_FALSE,
        .minSampleShading       = 0.f,
    };

    // Depth and stencil testing:
    const vk::PipelineDepthStencilStateCreateInfo depth_stencil_createinfo {
        .depthTestEnable    = VK_TRUE,
        .depthWriteEnable   = VK_TRUE,
        .depthCompareOp     = vk::CompareOp::eLess,
        .stencilTestEnable  = VK_FALSE,
    };

    // Color blend:
    const vk::PipelineColorBlendAttachmentState color_blend_attachment {
        .blendEnable    = VK_FALSE,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                          vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
    };

    const vk::PipelineColorBlendStateCreateInfo color_blend_createinfo {
        .logicOpEnable      = VK_FALSE,
        .attachmentCount    = 1,
        .pAttachments       = &color_blend_attachment
    };

    /*------------------------------------------------------------------*/

    std::vector<vk::DynamicState> dynamic_states {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
    };
    
    // Dynamic states:
    vk::PipelineDynamicStateCreateInfo dynamic_state_createinfo {
        .dynamicStateCount  = static_cast<uint32_t>(dynamic_states.size()),
        .pDynamicStates     = dynamic_states.data(),
    };

    /*------------------------------------------------------------------*/
    // Pipeline layout:

    auto descriptor_set_layout = create_descriptor_set_layout(device_wrapper);

    const vk::PipelineLayoutCreateInfo pipeline_layout_createinfo {
        .setLayoutCount = 1,
        .pSetLayouts    = &descriptor_set_layout.get(),
    };
    auto pipeline_layout = device.createPipelineLayoutUnique(pipeline_layout_createinfo);

    /*------------------------------------------------------------------*/
    // Renderpass:

    auto renderpass = create_renderpass(
        device_wrapper,
        color_format,
        depth_stencil_format,
        RenderPassType::ColorAndDepthStencil);

    /*------------------------------------------------------------------*/
    // Create

    const vk::GraphicsPipelineCreateInfo createinfo {
        .stageCount             = static_cast<uint32_t>(shader_stages.size()),
        .pStages                = shader_stages.data(),
        .pVertexInputState      = &vertex_input_createinfo,
        .pInputAssemblyState    = &input_assembly_createinfo,
        .pViewportState         = &viewport_createinfo,
        .pRasterizationState    = &rasterization_createinfo,
        .pMultisampleState      = &multisampling_createinfo,
        .pDepthStencilState     = &depth_stencil_createinfo,
        .pColorBlendState       = &color_blend_createinfo,
        .pDynamicState          = &dynamic_state_createinfo,
        .layout                 = pipeline_layout.get(),
        .renderPass             = renderpass.get(),
        .subpass                = 0,
    };
    auto [result, pipeline] = device.createGraphicsPipelineUnique(nullptr, createinfo);
    set_object_name(device_wrapper, pipeline.get(), "WorldPipeline");
    
    if (result == vk::Result::ePipelineCompileRequiredEXT)
        LOG_WARNING("compile required but not requested by application");

    /*------------------------------------------------------------------*/
    // Return:

    return PipelineWrapper {
        .pipeline               = std::move(pipeline),
        .descriptor_set_layout  = std::move(descriptor_set_layout),
        .renderpass             = std::move(renderpass),
    };
}

}