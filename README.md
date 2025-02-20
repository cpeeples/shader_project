# Shader Project

A mesmerizing GLSL shader featuring morphing particle shapes (stars, hearts, spirals) over a swirling plasma background. Created with help from xAI's Grok 3, rendered in 4K on an NVIDIA RTX 3090, and captured as a 10-second video.
Tested on Ubuntu 24.04 with nVidia driver 550 RTX3090

## Files
- `main4k.c`: OpenGL setup with GLFW/GLEW, 4k display resolution.
- `mainFHD.c` : OpenGL setup "" for 1920x1080 resolution
- `shader.vert`: Vertex shader.
- `shader.frag`: Fragment shader with particle morphing.
- `shader_video.mp4`: Rendered output (1080p).

## Build and Run

compile mainFHD.c instead of main4k.c if you want to run at 1080p.

```bash
gcc -o surprise main4k.c -lglfw -lGLEW -lGL -lm
./surprise
```

## Credits

Made by @pyrofrogg with Grok 3 from xAI. 
See it on X: https://x.com/pyrofrogg/status/1892668431549264215
