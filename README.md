
# Directus3D Game Engine
<img src="https://doublslash.com/img/assets/Windows8AnimatedLogo.png" width="20" height="20"/>&nbsp;
[![Build status](https://ci.appveyor.com/api/projects/status/p5duow3h4w8jp506?svg=true)](https://ci.appveyor.com/project/PanosK92/directus3d)

Directus3D is a game engine that started as a hobby project and evolved into something greater.
The source code aims to be clean, modern and tackles a lot of aspects of engine development.

The project is at an early development stage and there is a lot experimentation going on, regarding what works best.
This means that the wisest thing to do is to approach it as study material, without expecting to make games with it (yet).

Architectual quality is prioritized over development speed.

![Screenshot](https://raw.githubusercontent.com/PanosK92/Directus3D/master/Assets/screenshot-v0.3_preview4.jpg)

# Features
- 20+ audio file formats support.
- 30+ image file formats support.
- 40+ 3D file formats support.
- Cascaded shadow mapping.
- Entity-component system.
- Engine rendered editor.
- D3D11 rendering backend
- Deferred rendering.
- Frustum culling.
- HDR rendering.
- Multi-threading.
- Physically based shading.
- Physics.
- Post-process effects like FXAA & LumaSharpen.
- Scripting (C/C++).
- Windows support.

# Upcoming features (v0.3)
Feature       		            | Completion | Notes 
------------- 		            | :--: | -
Easy to build               	| 100% | Single click project generation which includes editor and runtime.
New editor               		| 100% | Replace Qt editor with ImGui editor.
Debug rendering    				| 90% | Transformation gizmos, scene grid, bounding boxes, colliders, raycasts, g-buffer visualization etc.
Improved shadows         		| 100% | Sharper shadows with smoother edges and no shimmering.
SSAO         					| 100% | Screen space ambient occlusion.
Bloom         					| 100% | -
SSR								| 100% | Screen space reflections.
TAA								| 100% | Temporal Anti-Aliasing (Based on Uncharted 4).
Per-Pixel motion blur			| 100% |
Velocity Buffer					| 100% | Required for TAA and Motion Blur.
Custom mipchain generation 		| 100% | Higher texture fidelity using Lanczos3 scaling.
Point light support             | 100% | -
XML I/O                         | 100% | -
Architecture improvements       | 100% | Higher quality codebase, allowing for future development.
Font importing and rendering    | 100% | Ability to load any font file.
Optimize & Debug				| 0% | Feature freeze, optimize and debug (once all tasks are done).

# Roadmap
- C# scripting.
- Vulkan rendering backend.
- Dynamic resolution scaling.
- Real-time ray tracing (experimental).
- Draw call batching (static & dynamic).
- Export on Windows.
- Skeletal Animation.
- Volumetric Lighting.
- Global Illumination.
- UI components.

### Download
- [Binaries](https://ci.appveyor.com/api/projects/PanosK92/directus3d/artifacts/Binaries/Release.zip?branch=master) - Binaries from last successful build
- [Compiling](https://github.com/PanosK92/Directus3D/blob/master/Documentation/CompilingFromSource/CompilingFromSource.md) - A guide on how to compile from source

### Dependencies
- [DirectX End-User Runtimes](https://www.microsoft.com/en-us/download/details.aspx?id=8109).
- [Visual C++ 2017 (x64) runtime package](https://go.microsoft.com/fwlink/?LinkId=746572).
- Windows 10.

### License
- Licensed under the MIT license, see [LICENSE.txt](https://github.com/PanosK92/Directus3D/blob/master/LICENSE.txt) for details.
