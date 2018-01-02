# le3d  
**LightEngine 3D**  
A straightforward C++ 3D software engine for real-time graphics.  
The engine aims to be a minimal and clear implementation of a simplified fixed pipeline.  
Code has been designed for resource constrained platforms.  

Source code / examples / documentation  
Copyright (c) 2015 - 2018 Frédéric Meslin  

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
The backend - Windows version:  
- Handles windows  
- Handles mouse events  
- Handles keyboard events  
- Handles GDI graphic contexts  
- Handles joysticks with rumble (needs Microsoft XInput V1.3 drivers installed)  
- Supports Windows XP / 7 / 8 and 10 OS  

The backend - Linux version:  
- Incoming  

The backend - MacOS version:  
- Incoming  

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
