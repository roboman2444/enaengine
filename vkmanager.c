#ifdef VULKAN_COMPILE

#include <vulkan/vulkan.h>
#include "globaldefs.h"
#include "vkmanager.h"


#include "glfwmanager.h"

#include "console.h"
#include "cvarmanager.h"

#define NUMEXT 1
const char *ext[] = {VK_KHR_SURFACE_EXTENSION_NAME, /*VK_KHR_XCB_SURFACE_EXTENSION_NAME,*/ 0};
#define NUMEXT2 1
const char *ext2[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, /*VK_KHR_XCB_SURFACE_EXTENSION_NAME,*/ 0};
VkInstance  vkinstance;
unsigned int vkdevices_count;
VkPhysicalDevice * vkdevices;
VkPhysicalDevice vkactivedevice;
VkDevice vkdevice;
VkPhysicalDeviceProperties vkdeviceprop;
VkQueueFamilyProperties *vkdevicesqp = 0;
VkPhysicalDeviceFeatures vkdevicefeat;

int vk_init(){
	unsigned int i;
	unsigned int qi = 0;
	unsigned int qc = 0;
	VkResult res;
	VkApplicationInfo ainfo = {0};
	VkInstanceCreateInfo iinfo = {0};

	ainfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	ainfo.pApplicationName = "enaengine";
	ainfo.pEngineName = "enaengine";
	ainfo.apiVersion = VK_API_VERSION;


	iinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	iinfo.pApplicationInfo = &ainfo;
	iinfo.enabledExtensionCount = NUMEXT;
	iinfo.ppEnabledExtensionNames = ext;
//	iinfo.enabledExtensionCount = glfw_extensions_count;
//	iinfo.ppEnabledExtensionNames = glfw_extensions;

	res = vkCreateInstance(&iinfo, NULL, &vkinstance);
	if(res){
		//TODO ERROR
		return FALSE;
	}


	res = vkEnumeratePhysicalDevices(vkinstance, &vkdevices_count, 0);
	if(vkdevices_count < 1 || res){
		//TODO ERROR
		return FALSE;
	}
	vkdevices = malloc(vkdevices_count * sizeof(VkPhysicalDevice));
	res = vkEnumeratePhysicalDevices(vkinstance, &vkdevices_count, vkdevices);
	if(res){
		//TODO ERROR
		return FALSE;
	}
	//find first dev with graphics capabilities
	for(i = 0; i < vkdevices_count; i++){
		vkactivedevice = vkdevices[i];
		qc = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(vkactivedevice, &qc, NULL);
		if(!qc) continue;
		vkdevicesqp = malloc(qc * sizeof(VkQueueFamilyProperties));
		vkGetPhysicalDeviceQueueFamilyProperties(vkactivedevice, &qc, vkdevicesqp);
		for(qi = 0; qi < qc; qi++){
			if(vkdevicesqp[qi].queueFlags & VK_QUEUE_GRAPHICS_BIT) break;
		}
		if(qi != qc) break;

		if(vkdevicesqp) free(vkdevicesqp);
		vkdevicesqp = 0;
	}
	//didnt find one
	if(i == vkdevices_count){
		return FALSE;
	}
	//create dev (finally)
	float qp[] = {0.0f};
	VkDeviceQueueCreateInfo qci = {0};
	qci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	qci.queueFamilyIndex = qi;
	qci.queueCount = 1;
	qci.pQueuePriorities = qp;



	VkDeviceCreateInfo dci = {0};
	dci.queueCreateInfoCount = 1;
	dci.pQueueCreateInfos = &qci;
	dci.enabledExtensionCount = NUMEXT2;
	dci.ppEnabledExtensionNames = ext2;
	res = vkCreateDevice(vkactivedevice, &dci, 0, &vkdevice);
	if(res){
		//TODO error
		return FALSE;
	}


	vkGetPhysicalDeviceProperties(vkactivedevice, &vkdeviceprop);
	vkGetPhysicalDeviceFeatures(vkactivedevice, &vkdevicefeat);

	return TRUE;
}
int vk_mainDraw(){
	return TRUE;
}
int vk_resizeViewports(int width, int height){
	return TRUE;
}
#endif
