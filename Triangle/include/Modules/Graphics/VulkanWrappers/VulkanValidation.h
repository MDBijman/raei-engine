#pragma once
#include <vulkan/vulkan.h>
#include <sstream>

class Validation
{
public:
	vk::Bool32 messageCallback(
		vk::DebugReportFlagsEXT flags,
		vk::DebugReportObjectTypeEXT objType,
		uint64_t srcObject,
		size_t location,
		int32_t msgCode,
		const char* pLayerPrefix,
		const char* pMsg,
		void* pUserData)
	{
	std::string message;
            {
                std::stringstream buf;
				
                if (flags & vk::DebugReportFlagBitsEXT::eError) {
                    buf << "ERROR: ";
                } else if (flags & vk::DebugReportFlagBitsEXT::eWarning) {
                    buf << "WARNING: ";
                } else if (flags & vk::DebugReportFlagBitsEXT::ePerformanceWarning) {
                    buf << "PERF: ";
                } else {
                    return false;
                }
                buf << "[" << pLayerPrefix << "] Code " << msgCode << " : " << pMsg;
                message = buf.str();
            }

            std::cout << message << std::endl;	
	}

	static vk::DebugReportCallbackEXT debugCallback;
};

