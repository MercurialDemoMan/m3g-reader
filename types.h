
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef signed short s16;

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#define IMAGE_2D_ALPHA           96
#define IMAGE_2D_LUMINANCE       97
#define IMAGE_2D_LUMINANCE_ALPHA 98
#define IMAGE_2D_RGB             99
#define IMAGE_2D_RGBA            100

#define CMODE_ALPHA      64
#define CMODE_ALPHA_ADD  65
#define CMODE_MODULATE   66
#define CMODE_MODULATE2X 67
#define CMODE_REPLACE    68

#define PMODE_CULL_BACK    160
#define PMODE_CULL_FRONT   161
#define PMODE_CULL_NONE    162
#define PMODE_SHADE_FLAT   164
#define PMODE_SHADE_SMOOTH 165
#define PMODE_WINDING_CCW  168
#define PMODE_WINDING_CW   169

#define TEXTURE_2D_FILTER_BASE_LEVEL 208
#define TEXTURE_2D_FILTER_LINEAR     209
#define TEXTURE_2D_FILTER_NEAREST    210
#define TEXTURE_2D_FUNC_ADD          224
#define TEXTURE_2D_FUNC_BLEND        225
#define TEXTURE_2D_FUNC_DECAL        226
#define TEXTURE_2D_FUNC_MODULATE     227
#define TEXTURE_2D_FUNC_REPLACE      228
#define TEXTURE_2D_WRAP_CLAMP        240
#define TEXTURE_2D_WRAP_REPEAT       241


typedef u32 ObjectIndex;



//object
typedef struct Object {
    
    u8  type;
    /*
     0 - header object
     14  - mesh
     255 - external reference
    */
    u32 length;
    u8* data;
    
} Object;



//section
#define SECTION_SIZE 13
typedef struct Section {
    
    u8      compression;          //0 - uncompressed
    //1 - zlib compressed, 32k buffer size, compressed are only objects
    u32     total_section_length; //including section handlers
    u32     uncompressed_length;  //only size of objects
    u8*     object_array;
    u32     checksum;             //adler32 checksum
    
    u32     current_object;       //current object file offset
    
} Section;



//submesh
typedef struct SubMesh {
    
    ObjectIndex index_buffer;
    ObjectIndex appearance;
    
} SubMesh;

//3d vector
typedef struct Vector3D {
    float x, y, z;
} Vector3D;

//rgba
typedef struct ColorRGBA {
    u8 r, g, b, a;
} ColorRGBA;

//rgb
typedef struct ColorRGB {
    u8 r, g, b;
} ColorRGB;

//user parameter
typedef struct UserParameter {
    
    u32 parameterID;
    u8* parameter_value; //array
    
} UserParameter;

//mesh
typedef struct Mesh {
    
    //Object3D class
    u32          userID;
    ObjectIndex* animation_tracks;          //array
    u32          user_parameter_count;
    
    UserParameter* user_parameters; //array
    
    //Transformable class
    bool has_component_transform;
    
    /*if has component transform*/
    Vector3D translation;
    Vector3D scale;
    float    orientation_angle;
    Vector3D orientation_axis;
    /****************************/
    
    
    bool has_general_transform;
    
    /*if has general transform*/
    float elements[16];
    /**************************/
    
    //Node class
    bool enable_rendering;
    bool enable_picking;
    u8   alpha_factor;
    u32  scope;
    
    bool has_alignment;
    
    /*if has alignment*/
    u8   z_target;
    u8   y_target;
    
    ObjectIndex  z_reference;
    ObjectIndex  y_reference;
    /******************/
    
    //Mesh class
    ObjectIndex vertex_buffer;
    u32         submesh_count;
    
    SubMesh* submesh_array;
    
} Mesh;



//vertex array
typedef struct VertexArray {
    
    //Object3D class
    u32          userID;
    ObjectIndex* animation_tracks;          //array
    u32          user_parameter_count;
    
    UserParameter* user_parameters; //array
    
    u8 component_size;  //2 - 4
    u8 component_count; //1 - 2
    u8 encoding;        //0 - 1
    
    u16 vertex_count;   //1 - 65535
    
    void* comp;
    /*
     *if component_size == 1 then
     *
     *   if encoding == 0 then
     *
     *       u8 comp[component_count]
     *
     *   else if encoding == 1 then
     *
     *       u8 comp_deltas[component_count]
     *
     *else
     *
     *   if encoding == 0 then
     *
     *       s16 comp[component_count]
     *
     *   else if encoding == 1 then
     *
     *       s16 comp_deltas[component_count]
     */
    
} VertexArray;



//texture coordinate
typedef struct TexCoord {
    
    ObjectIndex   tex_coords;
    float         tex_coord_bias[3];
    float         tex_coord_scale;
    
} TexCoord;

//vertex buffer
typedef struct VertexBuffer {
    
    //Object3D class
    u32          userID;
    ObjectIndex* animation_tracks;          //array
    u32          user_parameter_count;
    
    UserParameter* user_parameters;         //array
    
    //VertexBuffer class
    ColorRGBA    default_color;
    
    ObjectIndex  positions;
    float        position_bias[3];
    float        position_scale;
    
    ObjectIndex  normals;
    ObjectIndex  colors;
    
    u32          texcoord_array_count;
    TexCoord*    texcoord_array;    //array
    
    
} VertexBuffer;



//group
typedef struct Group {
    
    //Object3D class
    u32          userID;
    ObjectIndex* animation_tracks;          //array
    u32          user_parameter_count;
    
    UserParameter* user_parameters; //array
    
    //Transformable class
    bool has_component_transform;
    
    /*if has component transform*/
    Vector3D translation;
    Vector3D scale;
    float    orientation_angle;
    Vector3D orientation_axis;
    /****************************/
    
    
    bool has_general_transform;
    
    /*if has general transform*/
    float elements[16];
    /**************************/
    
    //Node class
    bool enable_rendering;
    bool enable_picking;
    u8   alpha_factor;
    u32  scope;
    
    bool has_alignment;
    
    /*if has alignment*/
    u8   z_target;
    u8   y_target;
    
    ObjectIndex  z_reference;
    ObjectIndex  y_reference;
    /******************/
    
    ObjectIndex* children;
    
} Group;



//world
typedef struct World {
    
    //Object3D class
    u32          userID;
    ObjectIndex* animation_tracks;          //array
    u32          user_parameter_count;
    
    UserParameter* user_parameters; //array
    
    //Transformable class
    bool has_component_transform;
    
    /*if has component transform*/
    Vector3D translation;
    Vector3D scale;
    float    orientation_angle;
    Vector3D orientation_axis;
    /****************************/
    
    
    bool has_general_transform;
    
    /*if has general transform*/
    float elements[16];
    /**************************/
    
    //Node class
    bool enable_rendering;
    bool enable_picking;
    u8   alpha_factor;
    u32  scope;
    
    bool has_alignment;
    
    /*if has alignment*/
    u8   z_target;
    u8   y_target;
    
    ObjectIndex  z_reference;
    ObjectIndex  y_reference;
    /******************/
    
    ObjectIndex* children;
    
    //World class
    ObjectIndex active_camera;
    ObjectIndex background;
    
} World;



//image 2d
typedef struct Image2D {
    
    //Object3D class
    u32          userID;
    ObjectIndex* animation_tracks;          //array
    u32          user_parameter_count;
    
    UserParameter* user_parameters;         //array
    
    //Image2D class
    u8   format;
    bool is_mutable;
    u32  width;
    u32  height;
    
    //if is_mutable == false then
    u8* palette;
    u8* pixels;
    //end
    
} Image2D;



//triangle strip array
typedef struct TriangleStripArray {
    
    //Object3D class
    u32          userID;
    ObjectIndex* animation_tracks;          //array
    u32          user_parameter_count;
    
    UserParameter* user_parameters;         //array
    
    //TriangleStripArray class
    u8 encoding;
    
    //IF encoding == 0, THEN
        //u32  startIndex;
    //ELSE IF encoding == 1, THEN
        //u8   startIndex;
    //ELSE IF encoding == 2, THEN
        //u16  startIndex;
    //ELSE IF encoding == 128, THEN
        //u32* indices; //array
    //ELSE IF encoding == 129, THEN
        //u8*  indices;  //array
    //ELSE IF encoding == 130, THEN
        //u16* indices; //array
    //END
    
    u32* stripLengths; //array
    
} TriangleStripArray;



//compositing mode
typedef struct CompositingMode {
    
    //Object3D class
    u32          userID;
    ObjectIndex* animation_tracks;          //array
    u32          user_parameter_count;
    
    UserParameter* user_parameters;         //array
    
    //CompositingMode class
    bool       depthTestEnabled;
    bool       depthWriteEnabled;
    bool       colorWriteEnabled;
    bool       alphaWriteEnabled;
    u8         blending;
    u8         alphaThreshold;
    float      depthOffsetFactor;
    float      depthOffsetUnits;
    
} CompositingMode;



//polygon mode
typedef struct PolygonMode {
    
    //Object3D class
    u32          userID;
    ObjectIndex* animation_tracks;          //array
    u32          user_parameter_count;
    
    UserParameter* user_parameters;         //array
    
    //PolygonMode class
    u8         culling;
    u8         shading;
    u8         winding;
    bool       two_sided_lighting_enabled;
    bool       local_camera_lighting_enabled;
    bool       perspective_correction_enabled;
    
} PolygonMode;



//material
typedef struct Material {
   
    //Object3D class
    u32          userID;
    ObjectIndex* animation_tracks;          //array
    u32          user_parameter_count;
    
    UserParameter* user_parameters;         //array
    
    ColorRGB      ambient_color;
    ColorRGBA     diffuse_color;
    ColorRGB      emissive_color;
    ColorRGB      specular_color;
    float         shininess;
    bool          vertex_color_tracking_enabled;
    
} Material;



//texture 2D
typedef struct Texture2D {
    
    //Object3D class
    u32          userID;
    ObjectIndex* animation_tracks;          //array
    u32          user_parameter_count;
    
    UserParameter* user_parameters;         //array
    
    //Transformable class
    bool has_component_transform;
    
    /*if has component transform*/
    Vector3D translation;
    Vector3D scale;
    float    orientation_angle;
    Vector3D orientation_axis;
    /****************************/
    
    
    bool has_general_transform;
    
    /*if has general transform*/
    float elements[16];
    /**************************/
    
    //texture2D class
    ObjectIndex image;
    
    ColorRGB    blend_color;
    u8          blending;
    
    u8          wrapping_s;
    u8          wrapping_t;
    
    u8          level_filter;
    u8          image_filter;
    
} Texture2D;



//Appearance
typedef struct Appearance {
    
    //Object3D class
    u32          userID;
    ObjectIndex* animation_tracks;          //array
    u32          user_parameter_count;
    
    UserParameter* user_parameters;         //array
    
    //Appearance class
    u8            layer;
    ObjectIndex   compositing_mode;
    ObjectIndex   fog;
    ObjectIndex   polygon_mode;
    ObjectIndex   material;
    ObjectIndex*  textures;
    
} Appearance;









