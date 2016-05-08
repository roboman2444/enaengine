#ifndef VKMANAGERHEADER
#define VKMANAGERHEADER

	int vk_init();
	int vk_mainDraw();
	int vk_resizeViewports(int width, int height);
	unsigned int screenHeight;
	unsigned int screenWidth;
	unsigned int maxMSAASamples;

#endif
