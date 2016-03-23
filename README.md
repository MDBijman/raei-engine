# VulkanTriangle

This is a derivative of Sascha Willems Vulkan repository. I recommend checking out his work.

There are wrappers for most Vulkan objects used in this example, to allow for command chaining. Basic Vulkan initialization code is contained with the VulkanBaseContext files. Specific code to create a triangle is inside the VulkanTriangle files.

Some Notes
  - There are wrappers for most Vulkan objects/structs used in this example (anything else not used is probably not wrapped)
  - You can chain commands, as a wrapper setters return references to themselves
  - Initialization can be found inside VulkanBaseContext.cpp/h
  - More specific triangle-related code can be found inside VulkanTriangle.cpp/h
  
