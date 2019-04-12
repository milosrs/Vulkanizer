# Vulkanizer
This is a students project for a masters degree at Faculty of Technical Sciences, Novi Sad.
Vulkanizer is a mini video game engine that will help me create a video game. 
It is really modest right now and is able to read .OBJ files and draw simple geometrical shapes. 
You can record what is happening in your window by pressing CTRL + S, and stop recording by pressing the same combination again.

## TODO-s:
1. Implement loading of glTF assets. *In progress
2. Implement simple physics.
3. Make a device that isn't a keyboard or mouse a controller. This isn't defined yet and may be a simple face tracking software.
4. Create a video game using this same engine

## How to use
1. Instal VulkanSDK from official site: https://www.lunarg.com/vulkan-sdk/
2. Install GLFW, which deletes HORRORS of Win32 programming.
3. Install GLM, a linear algebra library. https://glm.g-truc.net/0.9.9/index.html
4. Include STB library. Have a look at this repo: https://github.com/nothings/stb
5. Install ImageMagick and FFMpeg for recording. Visit these links: https://www.imagemagick.org/, http://ffmpeg.org/
6. Install Microsoft glTF-SDK using NuGet. Visit this repo: https://github.com/Microsoft/glTF-SDK
