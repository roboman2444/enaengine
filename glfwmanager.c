#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "globaldefs.h"
#include "glfwmanager.h"
#include "console.h"
#include "cvarmanager.h"

//temp
extern int glResizeViewports(int width, int height);

extern cvar_t cvar_gl_vulkan;


GLFWwindow * window;


const char ** glfw_extensions = 0;
unsigned int glfw_extensions_count = 0;


int glfwerror = 0;

int winwidth = 0;
int winheight = 0;
cvar_t cvar_gl_vulkan_supported = {CVAR_SAVEABLE, "gl_vulkan_supported", "Is vulkan supported?", "0"};



int glfw_resizeWindow(int width, int height, int bpp, int debugmode){
	if(debugmode) console_printf("DEBUG -- GLFW video resize to: %ix%i width who gives a fuck about bpp i dont handle it\n", width, height);
	if(height <1) height =1;
	if(width <1) width =1;
	glfwSetWindowSize(window, width, height);
	int r = glResizeViewports(width, height);
	if(!r){
		console_printf("ERROR -- GL framebuffers resize failed\n");
		return FALSE;
	}
	winwidth = width;
	winheight = height;
	return TRUE;
}


void glfw_handleError(int error, const char * desc){
	console_printf("ERROR -- GLFW %i-- %s\n", error, desc);
	glfwerror = error;
}


int glfw_init(int width, int height, int bpp, int debugmode){
        cvar_register(&cvar_gl_vulkan_supported);
        cvar_pset(&cvar_gl_vulkan_supported, "0");
	if(debugmode) console_printf("DEBUG -- GLFW window init\n");
	glfwSetErrorCallback(glfw_handleError);
	if(!glfwInit()){
		console_printf("ERROR -- GLFW init failed\n");
		return FALSE;
	}

//	if (glfwVulkanSupported()){
	if (TRUE){
	// Vulkan is available, at least for compute
//		glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);
	        cvar_register(&cvar_gl_vulkan_supported);
	        cvar_pset(&cvar_gl_vulkan_supported, "1");
	}
	if(cvar_gl_vulkan.valueint && cvar_gl_vulkan_supported.valueint){
//		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_CLIENT_API, 0);
	} else {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	}
	window = glfwCreateWindow(width, height, "enaengine", NULL, NULL);
	if(!window){
		glfwTerminate();
		console_printf("ERROR -- GLFW window creation failed\n");
		return FALSE;
	}
	if(cvar_gl_vulkan.valueint && cvar_gl_vulkan_supported.valueint){
	} else {
		glfwMakeContextCurrent(window);
	}
	return glfw_resizeWindow(width, height, bpp, debugmode);
}

void glfw_swapBuffers(void){
	glfwSwapBuffers(window);
}

int glfw_checkEvent(void){
	//todo
	glfwPollEvents();
	if(glfwWindowShouldClose(window)){
		glfwTerminate();
		exit(0);
	}
	int iw, ih;
	glfwGetWindowSize(window, &iw, &ih);
	if(iw != winwidth || ih != winheight) glfw_resizeWindow(iw, ih, 24, 0);
	return TRUE;
}
