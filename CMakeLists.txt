cmake_minimum_required(VERSION 3.25)

project(MyVulkan)


function(add_shaders_target TARGET)
  cmake_parse_arguments ("SHADER" "" "" "SOURCES" ${ARGN})
  set(SHADERS_DIR bin/shaders)
  file(GLOB SHADER_SOURCE_FILES ${SHADER_SOURCES})
  add_custom_command(
    OUTPUT ${SHADERS_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_DIR}
  )
  add_custom_command(
    OUTPUT ${SHADERS_DIR}/frag.spv ${SHADERS_DIR}/vert.spv
    COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
    ARGS --target-env vulkan1.0 ${SHADER_SOURCE_FILES} --quiet
    WORKING_DIRECTORY ${SHADERS_DIR}
    DEPENDS ${SHADERS_DIR} ${SHADER_SOURCE_FILES}
    COMMENT "Compiling Shaders"
    VERBATIM
  )
  add_custom_target(${TARGET} DEPENDS ${SHADERS_DIR}/frag.spv ${SHADERS_DIR}/vert.spv)
endfunction()


find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(tinyobjloader REQUIRED)

find_package(PkgConfig)
pkg_get_variable(STB_INCLUDE_DIR stb includedir)

find_package(Vulkan REQUIRED COMPONENTS glslangValidator)

set(MYVULKAN_SRC_FILES
  helpers_debug.h
  helpers_debug.cpp
  helpers_vulkan.h
  Instance.cpp
  Instance.h
  PhysicalDevice.cpp
  PhysicalDevice.h
  Surface.cpp
  Surface.h
  Device.cpp
  Device.h
  Swapchain.cpp
  Swapchain.h
  Image.cpp
  Image.h
  ImageView.cpp
  ImageView.h
  Sampler.cpp
  Sampler.h
  RenderPass.cpp
  RenderPass.h
  Framebuffer.cpp
  Framebuffer.h
  ShaderModule.cpp
  ShaderModule.h
  VertexShader.cpp
  VertexShader.h
  FragmentShader.cpp
  FragmentShader.h
  Pipeline.cpp
  Pipeline.h
  CommandPool.cpp
  CommandPool.h
  CommandBuffer.cpp
  CommandBuffer.h
  DescriptorPool.cpp
  DescriptorPool.h
  DescriptorSet.cpp
  DescriptorSet.h
  DescriptorSetLayout.cpp
  DescriptorSetLayout.h
  Buffer.cpp
  Buffer.h
  StagingBuffer.cpp
  StagingBuffer.h
  VertexBuffer.cpp
  VertexBuffer.h
  IndexBuffer.cpp
  IndexBuffer.h
  UniformBuffer.cpp
  UniformBuffer.h
  DeviceMemory.cpp
  DeviceMemory.h
  Semaphore.cpp
  Semaphore.h
  Fence.cpp
  Fence.h
)

set(SRC_FILES
  testbed.cpp
  ${MYVULKAN_SRC_FILES}
)
set(SHADER_FILES
  shader.vert
  shader.frag
)
set(TEXTURE_FILES
  texture.jpg
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${STB_INCLUDE_DIR}
)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    Vulkan::Vulkan
    glfw
)
target_compile_options(${PROJECT_NAME}
  PRIVATE
    -fmacro-prefix-map=${CMAKE_SOURCE_DIR}=. # make __FILE__ relative to project root
)

set(SHADER_TARGET shader)
add_shaders_target(${SHADER_TARGET} SOURCES ${SHADER_FILES})
add_dependencies(${PROJECT_NAME} ${SHADER_TARGET})

file(COPY ${TEXTURE_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/textures)
