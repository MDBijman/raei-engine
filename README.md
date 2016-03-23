# VulkanTriangle

This is a derivative of Sascha Willems Vulkan repository. I recommend checking out his work.


Some Notes:
  - There are wrappers for most Vulkan objects/structs used in this example (anything else not used is probably not wrapped)
  - You can chain commands, as a wrapper setters return references to themselves
  - Initialization can be found inside VulkanBaseContext.cpp/h
  - More specific triangle-related code can be found inside VulkanTriangle.cpp/h
  
