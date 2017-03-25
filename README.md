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

Supported platforms (by backend):  
- Windows (XP / 7 / 8 and 10)  

# Specifications  
The renderer provides:  
- 3D clipping  
- 2D clipping  
- Projection  
- Backculling  
- Z-sorting  

The rasterizer can draw:  
- Flat lit textured triangles (no filtering)  
- Flat lit alpha textured triangles (no filtering)  

The backend (only Windows platforms supported):  
- Draws on GDI contexts  
- Handle windows  
- Handle joysticks  

# Limitations  
- No perspective correction  
- No Z-buffer  
- No lightmaps  
- No shaders ...  

# Comparison  
It renders graphics very much like a Sony Playstation one.  

