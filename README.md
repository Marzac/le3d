# le3d  
**LightEngine 3D**  
**Version 1.5 - 24/01/2018**

A straightforward C++ 3D software engine for real-time graphics.  
The engine aims to be a minimal and clear implementation of a simplified fixed pipeline.  
Code has been designed for resource constrained platforms.  

Currently supported platforms:
- Microsoft Windows XP, 7, 8 and 10
- Linux based operating systems

Source code / examples / documentation  
Copyright (c) 2015 - 2018 Frédéric Meslin  

**Exceptions:**  
__Mesh textures__  
In the example folder, one or more textures on the 3D models  
have been created with photographs from Textures.com.  
These photographs may not be redistributed by default;  
Please visit http://www.textures.com for more information.  

__Skybox texture__  
In the example folder, the meadow skybox has been created by  
Emil "Humus" Persson and it is licenced under the Creative Commons  
Attribution 3.0 Unported License.  
Please visit http://www.humus.name/index.php?page=Textures for more information.  

**Fred's Lab**  
http://fredslab.net  
fred@fredslab.net, @marzacdev  

This is open source software released under the MIT License, please refer to LICENCE file for more information.  

# Specifications  
The renderer:  
- Renders meshes  
- Renders billboard sets  
- Does simple transformations (rotation, scaling, translation)  
- Performs 3D clipping  
- Performs 2D clipping  
- Performs projection  
- Does backculling  
- Performs Z-sorting  


The rasterizer:  
- Draws textured triangles  
- Handles mipmaping  
- Handles alpha blending  
- Performs perspective correction  
- Applies solid color per triangle  
- Does not perform texture filtering  
- Does not draw anti-aliased edges  
- Align vertex coordinates to nearest pixel coordinates  


The lighting system:
- Handles point lights
- Handles directionnal lights
- Handles ambient lights


# Backends  
window / draw / gamepad / timing  

The backend - Windows version:  
- Handles windows creation / management
- Handles mouse events
- Handles keyboard events
- Handles graphic contexts with GDI
- Handles joysticks with rumble support
  (with Microsoft XInput V1.3 drivers)
- Supports Windows XP / 7 / 8 and 10 OS
- Supports fullscreen mode

The backend - Linux version:  
- Handles windows creation / management (with X11)
- Handles mouse events (with X11)
- Handles keyboard events (with X11)
- Handles graphic contexts (with X11)
- Handles joysticks with rumble support
  (with evdev interface)
- Supports many Linux OS

The backend - MacOS version:  
- Incoming (maybe)

# File formats  
Supported bitmap / texture formats:
- Uncompressed 24bit RGB windows bitmap  
- Uncompressed 32bit RGBA windows bitmap  

Supported 3D model formats:
- Wavefront OBJ  

# Limitations  
- No Z-buffer  
- No lightmaps  
- No shaders  

# Comparison  
It renders graphics a bit better than a Sony Playstation one.  
