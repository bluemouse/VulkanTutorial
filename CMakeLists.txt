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
  helpers.h
  helpers.cpp
  Instance.cpp
  Instance.h
  PhysicalDevice.cpp
  PhysicalDevice.h
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
  Pipeline.cpp
  Pipeline.h
  CommandPool.cpp
  CommandPool.h
  CommandBuffer.cpp
  CommandBuffer.h
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
target_include_directories(${PROJECT_NAME} PRIVATE ${STB_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan glfw)

set(SHADER_TARGET shader)
add_shaders_target(${SHADER_TARGET} SOURCES ${SHADER_FILES})
add_dependencies(${PROJECT_NAME} ${SHADER_TARGET})

file(COPY ${TEXTURE_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/textures)
