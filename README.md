# le3d  
**LightEngine 3D**  
A straightforward C++ 3D software engine for real-time graphics.  
The engine aims to be a minimal and clear implementation of a simplified fixed pipeline.  
Code has been designed for resource constrained platforms.  

Source code / examples / documentation  
Copyright (c) 2015 - 2017 Frédéric Meslin  

**Fred's Lab**  
http://fredslab.net  
fred@fredslab.net, @marzacdev  

This is open source software released under the MIT License, please refer to LICENCE file for more information.  

# Specifications  
The renderer provides:  
- Basic transformations  
- 3D clipping  
- 2D clipping  
- Projection  
- Backculling  
- Z-sorting  

The rasterizer:  
- Draw flat lit textured triangles  
- Draw flat lit textured (with alpha channel) triangles  
- Has basic texture sampling (no filtering)  
- Has texture perspective correction  

The backend :  
- Handle native graphic contexts  
- Handle windows  
- Handle joysticks  

Platform supported (by backend):  
- Windows (XP / 7 / 8 and 10)  

# Limitations  
- No Z-buffer  
- No lightmaps  
- No shaders  

# Comparison  
It renders graphics very much like a Sony Playstation one.  
