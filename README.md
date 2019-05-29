# Vulkanizer
This is a students project for a masters degree at Faculty of Technical Sciences, Novi Sad.
Vulkanizer is a mini graphics engine that will help me learn about the Vulkan API, GLSL and glTF. 
It is really modest right now and is only to LOAD .gltf and .glb files, draw simple geometrical shapes and record a video. 

## TODO-s:
1. Write PBR shaders that can draw GLTF models. *In progress*
2. Implement simple physics.
3. Animate loaded objects.

## How to use
1. Instal VulkanSDK from official site: https://www.lunarg.com/vulkan-sdk/
2. Install GLFW.
3. Install GLM, a linear algebra library. https://glm.g-truc.net/0.9.9/index.html
4. Include STB library. Have a look at this repo: https://github.com/nothings/stb
5. Install ImageMagick and FFMpeg for recording. Visit these links: https://www.imagemagick.org/, http://ffmpeg.org/
6. Install Microsoft glTF-SDK using NuGet. Visit this repo: https://github.com/Microsoft/glTF-SDK