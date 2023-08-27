#include "Image.h"

using namespace Vulkan;

Image::Image(VkImage image, VkFormat format, VkExtent2D extent)
: _image(image), _format(format), _extent(extent) {
}

Image::~Image() {
}
