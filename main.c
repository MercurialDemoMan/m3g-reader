
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <zlib.h>

#include "Silent.h"
#include "types.h"

#define OBJ_TYPE_HEADER     0x00
#define OBJ_TYPE_APPEARANCE 0x03
#define OBJ_TYPE_CMODE      0x06
#define OBJ_TYPE_PMODE      0x08
#define OBJ_TYPE_GROUP      0x09
#define OBJ_TYPE_IMAGE2D    0x0A
#define OBJ_TYPE_TSA        0x0B
#define OBJ_TYPE_MATERIAL   0x0D
#define OBJ_TYPE_TEXT2D     0x11
#define OBJ_TYPE_VARRAY     0x14
#define OBJ_TYPE_VBUFFER    0x15

#define OBJ_TYPE_EXT        0xff

#define BUFFER_SIZE         32768

#define OBJ_CONVERT

static bool decrypting = false;
static bool decrypt    = false;
static bool dumping    = false;
static bool printing   = false;

//magic header
static const u8 HEADER[] = { 0xAB, 0x4A, 0x53, 0x52, 0x31, 0x38, 0x34, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };

//object types
static const char* OBJECT_TYPES[] = {
    "HEADER",
    "ANIMATION_CONTROLLER",
    "ANIMATION_TRACK",
    "APPEARANCE",
    "BACKGROUND",
    "CAMERA",
    "COMPOSITING_MODE",
    "FOG",
    "POLYGON_MODE",
    "GROUP",
    "IMAGE_2D",
    "TRIANGLE_STRIP_ARRAY",
    "LIGHT",
    "MATERIAL",
    "MESH",
    "MORPHING_MESH",
    "SKINNED_MESH",
    "TEXTURE_2D",
    "SPRITE",
    "KEYFRAME_SEQUENCE",
    "VERTEX_ARRAY",
    "VERTEX_BUFFER",
    "WORLD"
    //255 - external references
};

//fishlabs encryption/decryption
void fish_crypt(char* buffer, u32 size) {
    unsigned j;
    if(size < 100) {
        j = 10 + size % 10;
    } else if(size < 200) {
        j = 50 + size % 20;
    } else if(size < 300) {
        j = 80 + size % 20;
    } else {
        j = 100 + size % 50;
    }
    
    for(u32 i = 0; i < j; i++) {
        u8 temp = buffer[i];
        buffer[i] = buffer[size - i - 1];
        buffer[size - i - 1] = temp;
    }
}

//zlib decompression
//returns number of objects in section
//file seeker must be already set at the right position
u32 decompress_section(FILE* in, Section* source) {
    
    //create z stream
    z_stream dec_str;
    
    dec_str.zalloc = Z_NULL;
    dec_str.zfree  = Z_NULL;
    dec_str.opaque = Z_NULL;
    dec_str.avail_in = 0;
    dec_str.next_in  = Z_NULL;
    
    if(inflateInit(&dec_str) != Z_OK) {
        printf("decompress_section error: couldn't init inflation\n");
    }
    
    //compressed buffer
    u8* comp_buffer = malloc(source->total_section_length - SECTION_SIZE);
    
    fread(comp_buffer, source->total_section_length - SECTION_SIZE, 1, in);
    
    //decompressed buffer
    u8* uncp_buffer = malloc(source->uncompressed_length);
    
    //decompression setup
    dec_str.avail_in  = source->total_section_length - SECTION_SIZE;
    dec_str.next_in   = comp_buffer;
    
    dec_str.avail_out = source->uncompressed_length;
    dec_str.next_out  = uncp_buffer;
    
    //decompress
    int error = inflate(&dec_str, Z_NO_FLUSH);
    
    if(error != Z_OK && error != Z_STREAM_END) {
        printf("decompress_section error: couldn't inflate stream \"%i\"\n", error);
    }
    
    source->object_array = uncp_buffer;
    
    inflateEnd(&dec_str);
    free(comp_buffer);
    
    return 0;
}

//read section
bool read_section(FILE* in, Section* sec) {
    
    //read section
    if(fread(&sec->compression, sizeof(u8), 1, in) == 0) {
        return false;
    }
    fread(&sec->total_section_length, sizeof(u32), 1, in);
    fread(&sec->uncompressed_length, sizeof(u32), 1, in);
    
    sec->current_object = 0;
    
    //read objects
    if(sec->compression) {
        decompress_section(in, sec);
    } else {
        sec->object_array = malloc(sec->uncompressed_length);
        fread(sec->object_array, 1, sec->uncompressed_length, in);
    }
    
    //read checksum
    fread(&sec->checksum, sizeof(u32), 1, in);
    
    printf("\n/*SECTION*/: [compression: %s] [total section length: %06u] [uncompressed length: %06u] [checksum: 0x%08x]\n",
           sec->compression ? "true" : "false", sec->total_section_length, sec->uncompressed_length, sec->checksum);
    
    return true;
}

//read object
void read_object(Object* obj, Section* sec) {
    //set data offset
    //fseek(in, sec->current_object, SEEK_SET);
    
    obj->type   = *(u8*)(sec->object_array + sec->current_object);
    obj->length = *(u32*)(sec->object_array + sec->current_object + 1);
    obj->data   = (u8*)(sec->object_array + sec->current_object + 5);
    
    //read object
    //fread(&obj->type, sizeof(u8), 1, in);
    //fread(&obj->length, sizeof(u32), 1, in);
    //obj->data = malloc(obj->length);
    //fread(obj->data, obj->length, 1, in);
    
    //next object
    sec->current_object = sec->current_object + 5 + obj->length;
    
    //return file data pointer
    //fseek(in, original_offset, SEEK_SET);
    
    
    printf("\n    /*OBJECT*/: [type: %s] [length: %06u]\n",
           OBJECT_TYPES[obj->type], obj->length);
}

//print group object
/*void print_group_object(Object* obj) {
    
    for(u32 i = 0; i < obj->length; i++) {
        printf("[%s%x], ", *(obj->data + i) < 0xf ? "0x0" : "0x" , *(obj->data + i));
        if(i != 0 && i % 8 == 0) {
            printf("\n");
        }
    }
    
}*/

void print_object(Object* obj) {
    
    switch(obj->type) {
            
        case OBJ_TYPE_HEADER:
            printf("\n        /*HEADER OBJECT*/: [version: %s, %s] [has external references: %s] [total file size: %06u] [approximate content size: %06u] [authoring field: %s]\n",
                   *(obj->data) ? "1" : "0",
                   *(obj->data + 1) ? "1" : "0",
                   *(obj->data + 2) ? "true" : "false",
                   *(u32*)(obj->data + 3),
                   *(u32*)(obj->data + 7),
                   (obj->data + 11)
                   );
            break;
            
        case OBJ_TYPE_EXT:
            printf("\n        /*EXTERNAL REFERENCES OBJECT*/: [URI: %s]\n", obj->data);
            break;
            
        case OBJ_TYPE_VARRAY:
            
            //TODO: print different types
            
            printf("\n        /*VERTEX ARRAY OBJECT*/: [component size: 0x%02x] [component count: 0x%02x] [encoding: 0x%02x] [vertex count: %hu]\n", *(obj->data + 12), *(obj->data + 13), *(obj->data + 14), *(u16*)(obj->data + 15));
            u8 comp_size   = *(obj->data + 12);
            u8 comp_count  = *(obj->data + 13);
            u8 encoding    = *(obj->data + 14);
            u16 vert_count = *(u16*)(obj->data + 15);
            void* data     = obj->data + 17;
#ifdef OBJ_CONVERT
            printf("o mesh\n");
#endif
            
            for(u32 i = 0; i < vert_count; i++) {
#ifdef OBJ_CONVERT
                printf("v ");
#else
                printf("            /*VERTEX*/: ");
#endif
                for(u32 j = 0; j < comp_count; j++) {
                    
                    if(comp_size == 1) {
                        
                        if(encoding == 0) {
                            
                            printf("[0x%02x]", *(u8*)(data + i * comp_count + j));
                        } else if(encoding == 1) {
                            
                            printf("[D 0x%02x]", *(u8*)(data + i * comp_count + j));
                        } else {
                            
                            printf("Error encoding 0x%x\n", encoding);
                        }
                    } else {
                        
                        if(encoding == 0) {
#ifdef OBJ_CONVERT
                            printf("%f ", (float)*(s16*)(data + i * comp_count + j * sizeof(s16)) / 100.0f);
#else
                            printf("[%06i]", *(s16*)(data + i * comp_count + j * sizeof(s16)));
#endif
                        } else if(encoding == 1) {
#ifdef OBJ_CONVERT
                            printf("%f ", (float)*(s16*)(data + i * comp_count + j * sizeof(s16)) / 100.0f);
#else
                            printf("[D %06i]", *(s16*)(data + i * comp_count + j * sizeof(s16)));
#endif
                        } else {
                            
                            printf("Error encoding 0x%x\n", encoding);
                        }
                    }
                }
                printf("\n");
            }
            break;
            
        case OBJ_TYPE_IMAGE2D:
            
            printf("\n        /*IMAGE 2D OBJECT*/: [format: 0x%x] [is mutable: %s] [width: %u] [height: %u]\n", *(obj->data + 12), *(obj->data + 13) ? "true" : "false", *(u32*)(obj->data + 14), *(u32*)(obj->data + 18));
            break;
            
        case OBJ_TYPE_VBUFFER:
            
            printf("\n        /*VERTEX BUFFER OBJECT*/: [default color: 0x%x, 0x%x, 0x%x, 0x%x] [positions: %uI] [position bias: %f, %f, %f] [position scale: %f] [normals: %uI] [colors: %uI] [texcoord array count: %u]\n", *(obj->data + 12), *(obj->data + 13), *(obj->data + 14), *(obj->data + 15), *(u32*)(obj->data + 16), *(float*)(obj->data + 20), *(float*)(obj->data + 24), *(float*)(obj->data + 28), *(float*)(obj->data + 32), *(u32*)(obj->data + 36), *(u32*)(obj->data + 40), *(u32*)(obj->data + 44));
            break;
            
        //TODO: better
        case OBJ_TYPE_TSA:
            
            printf("\n        /*TRIANGLE STRIP ARRAY*/: [encoding: 0x%x]", *(obj->data + 12));
            
            switch(*(obj->data + 12)) {
                case 0:
                    printf("[start index: %u]\n", *(u32*)(obj->data + 13));
                    break;
                case 1:
                    printf("[start index: 0x%x]\n", *(obj->data + 13));
                    break;
                case 2:
                    printf("[start index: %hu]\n", *(u16*)(obj->data + 13));
                    break;
                default:
                    printf("hol up!\n");
                    break;
            }
            break;
            
        case OBJ_TYPE_CMODE:
            printf("\n        /*COMPOSITING MODE*/: [depth test enabled: %s] [depth write enabled: %s] [color write enabled: %s] [alpha write enabled: %s] [blending: 0x%x] [alpha threshold: 0x%x] [depth offset factor: %f] [depth offset units: %f]\n", *(obj->data + 12) ? "true" : "false", *(obj->data + 13) ? "true" : "false", *(obj->data + 14) ? "true" : "false", *(obj->data + 15) ? "true" : "false", *(obj->data + 16), *(obj->data + 17), *(float*)(obj->data + 18), *(float*)(obj->data + 22));
            break;
            
        case OBJ_TYPE_PMODE:
            printf("\n        /*POLYGON MODE*/: [culling: 0x%x] [shading: 0x%x] [winding: 0x%x] [two sided lighting enabled: %s] [local camera lighting enabled: %s] [perspective correction enabled: %s]\n", *(obj->data + 12), *(obj->data + 13), *(obj->data + 14), *(obj->data + 15) ? "true" : "false", *(obj->data + 16) ? "true" : "false", *(obj->data + 17) ? "true" : "false");
            break;
            
        case OBJ_TYPE_MATERIAL:

            printf("\n        /*MATERIAL*/: [ambient color: 0x%x, 0x%x, 0x%x] [diffuse color: 0x%x, 0x%x, 0x%x, 0x%x] [emissive color: 0x%x, 0x%x, 0x%x] [specular color: 0x%x, 0x%x, 0x%x] [shininess: %f] [vertex color tracking enabled: %s]", *(obj->data + 12), *(obj->data + 13), *(obj->data + 14), *(obj->data + 15), *(obj->data + 16), *(obj->data + 17), *(obj->data + 18), *(obj->data + 19), *(obj->data + 20), *(obj->data + 21), *(obj->data + 22), *(obj->data + 23), *(obj->data + 24), *(float*)(obj->data + 25), *(obj->data + 29) ? "true" : "false");
            break;
            
        case OBJ_TYPE_TEXT2D:
            printf("\n        /*TEXTURE2D*/: ");
            if(*(obj->data + 12)) { //has component transform
                printf("[translation: %f, %f, %f] [scale: %f, %f, %f] [oriantation angle: %f] [oriantation axis: %f %f %f] ", *(float*)(obj->data + 13), *(float*)(obj->data + 17), *(float*)(obj->data + 21), *(float*)(obj->data + 25), *(float*)(obj->data + 29), *(float*)(obj->data + 33), *(float*)(obj->data + 37), *(float*)(obj->data + 41), *(float*)(obj->data + 45), *(float*)(obj->data + 49));
            }
            
            //has general transform
            if(*(obj->data + 12) && *(obj->data + 53)) {
                printf("[elements: %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f] ", *(float*)(obj->data + 54), *(float*)(obj->data + 58), *(float*)(obj->data + 62), *(float*)(obj->data + 66), *(float*)(obj->data + 70), *(float*)(obj->data + 74), *(float*)(obj->data + 78), *(float*)(obj->data + 82), *(float*)(obj->data + 86), *(float*)(obj->data + 90), *(float*)(obj->data + 94), *(float*)(obj->data + 98), *(float*)(obj->data + 102), *(float*)(obj->data + 106), *(float*)(obj->data + 110), *(float*)(obj->data + 114));
            }
            
            if(!*(obj->data + 12) && *(obj->data + 13)) {
                printf("[elements: %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f] ", *(float*)(obj->data + 14), *(float*)(obj->data + 18), *(float*)(obj->data + 22), *(float*)(obj->data + 26), *(float*)(obj->data + 30), *(float*)(obj->data + 34), *(float*)(obj->data + 38), *(float*)(obj->data + 42), *(float*)(obj->data + 46), *(float*)(obj->data + 50), *(float*)(obj->data + 54), *(float*)(obj->data + 58), *(float*)(obj->data + 62), *(float*)(obj->data + 66), *(float*)(obj->data + 70), *(float*)(obj->data + 74));
            }
            
            //rest of the data
            if(*(obj->data + 12)) {
                
                if(*(obj->data + 53)) {
                    
                    printf("[image: %uI] [blend color: 0x%x, 0x%x, 0x%x] [blending: 0x%x] [wrapping s: 0x%x] [wrapping t: 0x%x] [level filter: 0x%x] [image filter: 0x%x]\n", *(u32*)(obj->data + 118), *(obj->data + 122), *(obj->data + 123), *(obj->data + 124), *(obj->data + 125), *(obj->data + 126), *(obj->data + 127), *(obj->data + 128), *(obj->data + 129));
                } else {
                    
                    printf("[image: %uI] [blend color: 0x%x, 0x%x, 0x%x] [blending: 0x%x] [wrapping s: 0x%x] [wrapping t: 0x%x] [level filter: 0x%x] [image filter: 0x%x]\n", *(u32*)(obj->data + 54), *(obj->data + 58), *(obj->data + 59), *(obj->data + 60), *(obj->data + 61), *(obj->data + 62), *(obj->data + 63), *(obj->data + 64), *(obj->data + 65));
                }
            } else {
                
                if(*(obj->data + 13)) {
                    
                    printf("[image: %uI] [blend color: 0x%x, 0x%x, 0x%x] [blending: 0x%x] [wrapping s: 0x%x] [wrapping t: 0x%x] [level filter: 0x%x] [image filter: 0x%x]\n", *(u32*)(obj->data + 78), *(obj->data + 82), *(obj->data + 83), *(obj->data + 84), *(obj->data + 85), *(obj->data + 86), *(obj->data + 87), *(obj->data + 88), *(obj->data + 89));
                } else {
                    
                    printf("[image: %uI] [blend color: 0x%x, 0x%x, 0x%x] [blending: 0x%x] [wrapping s: 0x%x] [wrapping t: 0x%x] [level filter: 0x%x] [image filter: 0x%x]\n", *(u32*)(obj->data + 14), *(obj->data + 18), *(obj->data + 19), *(obj->data + 20), *(obj->data + 21), *(obj->data + 22), *(obj->data + 23), *(obj->data + 24), *(obj->data + 25));
                }
            }
            
            break;
            
        case OBJ_TYPE_APPEARANCE:
            
            printf("\n        /*APPEARANCE*/: [layer: 0x%x] [compositing mode: %uI] [fog: %uI] [polygon mode: %uI] [material: %uI]\n", *(obj->data + 12), *(u32*)(obj->data + 13), *(u32*)(obj->data + 17), *(u32*)(obj->data + 21), *(u32*)(obj->data + 25));
            
            //TODO: iterate to the end of the object
            for(u32 i = 29; i < obj->length; i += sizeof(u32)) {
                
                printf("            /*TEXTURE ID*/: %u\n", *(u32*)(obj->data + i));
            }
            
            break;
            
        default:
            //printf("Unimplemented object type: [%s]\n", OBJECT_TYPES[obj->type]);
            break;
    }
}

//main program
int main(int argc, char* argv[]) {
    
    if(argc < 2) { printf("usage: prog [input] [switches]\n"); return 1; }
    
    if(argc > 2) {
        for(u32 i = 2; i < argc; i++) {
            
            if(!strcmp(argv[i], "-decrypting")) {
                decrypting = true;
            } else if(!strcmp(argv[i], "-dumping")) {
                dumping = true;
            } else if(!strcmp(argv[i], "-printing")) {
                printing = true;
            } else if(!strcmp(argv[i], "-decrypt")) {
                decrypt = true;
            }
        }
    }
    
    //decryption
    if(decrypt || decrypting) {
        
        //open file
        FILE* in = fopen(argv[1], "rb");
        
        //check argument validity
        if(in == NULL) { printf("cannot open the file\n"); return 1; }
        
        //get file size
        u32 f_size = fsize(in);
        
        //create buffer that will hold file content
        char* buffer = malloc(f_size);
        fread(buffer, f_size, 1, in);
        
        //decrypt
        fish_crypt(buffer, f_size);
        
        fclose(in);
        
        //overwrite the same file
        FILE* out = fopen(argv[1], "wb");
        fwrite(buffer, 1, f_size, out);
        
        fclose(out);
        
        //deallocate memory
        free(buffer);
    }
    
    //decrypt only
    if(!decrypt) {

        //open file
        FILE* in = fopen(argv[1], "rb");
        
        if(in == NULL) { printf("cannot open the file\n"); return 1; }
        
        //allocate buffer
        u8* buffer = malloc(BUFFER_SIZE);
        
        //check header
        fread(buffer, sizeof(HEADER), sizeof(u8), in);
        
        for(u32 i = 0; i < sizeof(HEADER); i++) {
            if(buffer[i] != HEADER[i]) {
                printf("invalid header\n");
                goto ESCAPE;
            }
        }
        
        //buffers
        Section sec;
        Object  obj;
        
        //read section 0 - file header object
        read_section(in, &sec);
        
        //read header object
        read_object(&obj, &sec);
        
        if(obj.type == OBJ_TYPE_HEADER) {
            //print header object
            print_object(&obj);
        } else {
            printf("Wait what!?\n");
        }
        
        u32 current_section = 1;
        u32 current_object  = 0;
        
        //read all other sections
        while(read_section(in, &sec)) {
            
            //TODO: read all objects in section
            //      probably done?
            //TODO: decompress section is all wrong!!!
            //read all objects in section
            while(sec.current_object < sec.uncompressed_length) {
                
                read_object(&obj, &sec);
                
                //print dat shit
                if(printing) { print_object(&obj); }
                //dump dat shit
                if(dumping)  {
                    
                    char file_path[PATH_MAX] = { 0 };
                    
                    mkdir("DUMP", 0777);
                    
                    strcat(file_path, "DUMP/");
                    strcat(file_path, "sec");
                    sprintf(file_path + 8, "%u", current_section);
                    strcat(file_path, "obj");
                    sprintf(file_path + 12, "%u", current_object);
                    strcat(file_path, ".");
                    strcat(file_path, OBJECT_TYPES[obj.type]);
                    
                    FILE* out = fopen(file_path, "wb");
                    
                    if(out == NULL) { printf("WHAT"); }
                    
                    fwrite(obj.data, obj.length, 1, out);
                    
                    fclose(out);
                }
                
                current_object++;
                
            }
            
            current_section++;
        }
        
        ESCAPE:
        
            fclose(in);
            free(buffer);

    }
}








