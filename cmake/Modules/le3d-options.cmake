# Engine configuration
set(LE3D_RESOX_DEFAULT				640			CACHE STRING "Default horizontal resolution")
set(LE3D_RESOY_DEFAULT				480			CACHE STRING "Default vertical resolution")
set(LE3D_MAX_FILE_EXTENSION			8			CACHE STRING "Maximum file extension string length")
set(LE3D_MAX_FILE_NAME				128			CACHE STRING "Maximum file name string length")
set(LE3D_MAX_FILE_PATH				256			CACHE STRING "Maximum file path string length")
mark_as_advanced(LE3D_MAX_FILE_EXTENSION LE3D_MAX_FILE_NAME LE3D_MAX_FILE_PATH)

# Windows manager
set(LE3D_WINDOW_EXTENDED_KEYS		1			CACHE STRING "Maximum number of mipmaps per bitmap")
mark_as_advanced(LE3D_WINDOW_EXTENDED_KEYS)

# Data caches
set(LE3D_BMPCACHE_SLOTS				1024		CACHE STRING "Maximum number of bitmaps in cache")
set(LE3D_MESHCACHE_SLOTS			1024		CACHE STRING "Maximum number of meshes in cache")
mark_as_advanced(LE3D_BMPCACHE_SLOTS LE3D_MESHCACHE_SLOTS)

# Wavefront object parser
set(LE3D_OBJ_MAX_NAME				256			CACHE STRING "Wavefront object maximum name string length")
set(LE3D_OBJ_MAX_LINE				1024		CACHE STRING "Wavefront object maximum file line length")
set(LE3D_OBJ_MAX_PATH				256			CACHE STRING "Wavefront object maximum path string length")
mark_as_advanced(LE3D_OBJ_MAX_NAME LE3D_OBJ_MAX_LINE LE3D_OBJ_MAX_PATH)

# Bitmap manipulator
set(LE3D_BMP_MIPMAPS				32			CACHE STRING "Maximum number of mipmaps per bitmap")
mark_as_advanced(LE3D_BMP_MIPMAPS)

# Renderer configuration
set(LE3D_RENDERER_FRONT			    -1.0f		CACHE STRING "Front clipping plane")
set(LE3D_RENDERER_BACK			    -32768.0f	CACHE STRING "Back clipping plane")
set(LE3D_RENDERER_FOV				65.0f		CACHE STRING "Default field of view")
option(LE3D_RENDERER_3DFRUSTRUM		"Use a 3D frustrum to clip triangles" On)
option(LE3D_RENDERER_2DFRAME		"Use a 2D frame to clip triangles" Off)

option(LE3D_RENDERER_INTRASTER "Enable fixed point or floating point rasterizing" Off)
option(LE3D_RENDERER_MIPMAPS "Enable mipmapping on textures" On)

set(LE3D_TRILIST_MAX				50000		CACHE STRING "Maximum number of triangles in display list")
set(LE3D_VERLIST_MAX				150000		CACHE STRING "Maximum number of vertexes in transformation buffer")
mark_as_advanced(LE3D_TRILIST_MAX LE3D_VERLIST_MAX)

# Performance optimizations
if(NOT(AMIGA))
    option(LE3D_USE_SIMD "Use SIMD instructions & vectors" On)
    option(LE3D_USE_SSE2 "Use Intel SSE2 instructions" On)
else()
    option(LE3D_USE_AMMX "Use Apollo AMMX instructions" Off)
endif()
