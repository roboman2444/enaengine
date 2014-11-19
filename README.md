enaengine
=========
A simple engine to power our upcoming game: Drive to Survive.
Will (might) have basic shaders and effects, editable worlds, skeletal animation with gpu skinning, either Bullet or ODE physics, networked multiplayer, etc.
By the way, this engine is primarily focused on outdoor areas, and will perform rather poorly in indoor environments. Frankly, it will just perform rather poorly in general. The entire culling process might just consist of viewport and backface culling, with a depth prepass to give a slight boost.
uses stb_image.h


Requires sdl1.2(will be changed to sdl2), glew (might be changed to glloadgen soon), and a working opengl 3.3 environment.
Has some weird shader compilation bugs on certian intel GPUs.
