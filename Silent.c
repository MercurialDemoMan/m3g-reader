#include "Silent.h"

//reads the header and creates the silent interface
SilentInterface* new_interface (char* input) {
    
    //open input file
    FILE* in = fopen(input, "rb");
    if(in == NULL) {
        perror("Error: Cannot open file"); return NULL;
    }
    
    //check header
    char header[3]; eread(header, 3, false, in);
    if(header[0] != 'S' || header[1] != 'P' || header[2] != 'C') {
        printf("Error: file is not silent package\n"); fclose(in); return NULL;
    }
    
    //create interface
    SilentInterface* interface = my_malloc(sizeof(SilentInterface));
    interface->file            = in;
    interface->nodes           = newLL();
    
    //read encryption switch
    eread((char*)&interface->encryption, 1, false, in);
    //read key
    eread((char*)&interface->key[0], 4, interface->encryption, in);
    
    //read number of files and directories
    u32 files_num = 0;
    u32 dirs_num  = 0;
    
    eread((char*)&files_num, sizeof(u32), interface->encryption, in);
    eread((char*)&dirs_num,  sizeof(u32), interface->encryption, in);
    
#ifdef DEBUG
    printf("Number of files: %u\n", files_num);
#endif
    
    //read file names lengths
    u16 file_names_length[files_num];
    for(u32 i = 0; i < files_num; i++) {
        eread((char*)&file_names_length[i], sizeof(u16), interface->encryption, in);
#ifdef DEBUG
        printf("File [index: %u, name size: %hu]\n", i, file_names_length[i]);
#endif
    }
    
    //read files sizes
    u64 file_sizes[files_num];
    for(u32 i = 0; i < files_num; i++) {
        eread((char*)&file_sizes[i], sizeof(u64), interface->encryption, in);
#ifdef DEBUG
        printf("File [index: %u, size: %llu]\n", i, file_sizes[i]);
#endif
    }
    
    //read dir names lengths
    u16 dir_names_length[dirs_num];
    for(u32 i = 0; i < dirs_num; i++) {
        eread((char*)&dir_names_length[i], sizeof(u16), interface->encryption, in);
#ifdef DEBUG
        printf("Dir [index: %u, name size: %hu]\n", i, dir_names_length[i]);
#endif
    }
    
    //read dir names
    char* dir_names[dirs_num];
    for(u32 i = 0; i < dirs_num; i++) {
        dir_names[i] = my_malloc(dir_names_length[i] + 1);
        eread(dir_names[i], dir_names_length[i], interface->encryption, in);
        dir_names[i][dir_names_length[i]] = 0;
#ifdef DEBUG
        printf("Dir [index: %u, name: %s]\n", i, dir_names[i]);
#endif
    }
    
    
    //get files information
    for(u32 i = 0; i < files_num; i++) {
        
        //read name
        char  file_name[PATH_MAX];
        
        eread(file_name, file_names_length[i], interface->encryption, in);
        file_name[file_names_length[i]] = 0;
        
        //get attributes
        u16 attributes = 0;
        eread((char*)&attributes, sizeof(u16), interface->encryption, in);
        
        //create silent node
        pushLL(interface->nodes, newSNode(file_name, ftell(in), file_sizes[i], attributes));
        
        //move read pointer to the next file
        fseek(in, ftell(in) + file_sizes[i], SEEK_SET);
        
    }
    
    //reset file offset
    fseek(in, 0, SEEK_SET);
    
    //cleaning
    for(u32 i = 0; i < dirs_num; i++) {
        my_free(dir_names[i]);
    }
    
    //return
    return interface;
}
//unpacks whole silent package
void unsilence          (char* input, const char* output, char* password) {
    
    printf("Start unsilencing...\n");
    
    FILE* in = fopen(input, "rb");
    if(in == NULL) { printf("Error: connot open input file\n"); return; }
    
    bool encryption = false;
    
    //check header
    char header[3]; eread(header, 3, false, in);
    if(header[0] != 'S' || header[1] != 'P' || header[2] != 'C') {
        printf("Error: file is not silent package\n"); fclose(in); return;
    }
    
    //read encryption switch
    eread((char*)&encryption, 1, false, in);
    
    //check argument validity
    if(encryption && password == NULL) {
        printf("File is encrypted. Please provide a password\n");
        fclose(in);
        return;
    }
    
    //read the key
    u8 key[4] = { 0 };
    eread((char*)&key[0], 4, encryption, in);
    
    //check password validity
    u16 password_size = 0;
    if(password != NULL) {
        password_size = strlen(password);
        u32 k         = lookup3hash(password, password_size);
        
        if(*(u32*)&key != k) {
            printf("Invalid password\n");
            fclose(in);
            return;
        }
        
    }
    
    //read number of files and directories
    u32 files_num = 0;
    u32 dirs_num  = 0;
    
    eread((char*)&files_num, sizeof(u32), encryption, in);
    eread((char*)&dirs_num,  sizeof(u32), encryption, in);
    
#ifdef DEBUG
    printf("Number of files: %u\n", files_num);
#endif
    
    //read file names lengths
    u16 file_names_length[files_num];
    for(u32 i = 0; i < files_num; i++) {
        eread((char*)&file_names_length[i], sizeof(u16), encryption, in);
#ifdef DEBUG
        printf("File [index: %u, name size: %hu]\n", i, file_names_length[i]);
#endif
    }
    
    //read files sizes
    u64 file_sizes[files_num];
    for(u32 i = 0; i < files_num; i++) {
        eread((char*)&file_sizes[i], sizeof(u64), encryption, in);
#ifdef DEBUG
        printf("File [index: %u, size: %llu]\n", i, file_sizes[i]);
#endif
    }
    
    //read dir names lengths
    u16 dir_names_length[dirs_num];
    for(u32 i = 0; i < dirs_num; i++) {
        eread((char*)&dir_names_length[i], sizeof(u16), encryption, in);
#ifdef DEBUG
        printf("Dir [index: %u, name size: %hu]\n", i, dir_names_length[i]);
#endif
    }
    
    //read dir names
    char* dir_names[dirs_num];
    for(u32 i = 0; i < dirs_num; i++) {
        dir_names[i] = my_malloc(dir_names_length[i] + 1);
        eread(dir_names[i], dir_names_length[i], encryption, in);
        dir_names[i][dir_names_length[i]] = 0;
#ifdef DEBUG
        printf("Dir [index: %u, name: %s]\n", i, dir_names[i]);
#endif
    }
    
    //create directories
    makedir(output);
    
    for(u32 i = 0; i < dirs_num; i++) {
        
        char path[PATH_MAX];
        strcpy(path, output);
        strcat(path, "/");
        strcat(path, dir_names[i]);
        
        makedir(path);
#ifdef DEBUG
        printf("Created folder: %s\n", path);
#endif
    }
    
    //create files
    for(u32 i = 0; i < files_num; i++) {
        
        //read name
        char  file_name[PATH_MAX];
        char* file_name_buffer = my_malloc(file_names_length[i]);
        
        strcpy(file_name, output);
        strcat(file_name, "/");
        
        eread(file_name_buffer, file_names_length[i], encryption, in);
        file_name_buffer[file_names_length[i]] = 0;
        
        strcat(file_name, file_name_buffer);
        
        FILE* out = fopen(file_name, "wb");
        if(out == NULL) { printf("Error: cannot open output file %s ", file_name); perror(" "); return; }
        
        //get attributes
        u16 attributes = 0;
        eread((char*)&attributes, sizeof(u16), encryption, in);
        
        //write the data
        for(u64 j = 0; j < file_sizes[i]; j += SLURP_SIZE) {
            
            u32   slurp  = j + SLURP_SIZE > file_sizes[i] ? (u32)(file_sizes[i] - j) : SLURP_SIZE;
            
            char* buffer = my_malloc(slurp);
            eread(buffer, slurp, encryption, in);
            
            if(encryption) {
                for(u32 k = 0; k < slurp; k++) {
                    buffer[k] ^= password[k % password_size]++;
                }
            }
            
            ewrite(buffer, slurp, false, out);
            
            my_free(buffer);
        }
        
        fclose(out);
        
        set_attributes(file_name, attributes);
        
        my_free(file_name_buffer);
    }
    
    printf("Unsilencing done!\nCleaning...\n");
    
    //cleaning
    for(u32 i = 0; i < dirs_num; i++) {
        my_free(dir_names[i]);
    }
    
    fclose(in);
    
    printf("Done!\n");
}
//unpacks certain file from silent package
void unsilence_specific (SilentInterface* interface, u64 index, char* folder, char* password) {
    
    printf("Start unsilencing...\n");
    
    if(interface == NULL) {
        printf("Error: interface is not allocated\n"); return;
    }
    
    //check argument validity
    if(interface->encryption && password == NULL) {
        printf("File is encrypted. Please provide a password\n");
        return;
    }
    
    //check password validity
    u16 password_size = 0;
    if(password != NULL) {
        password_size = strlen(password);
        u32 k         = lookup3hash(password, password_size);
        
        if(*(u32*)&interface->key != k) {
            printf("Invalid password\n");
            return;
        }
        
    }
    
    if(index > sizeLL(interface->nodes) - 1) {
        printf("Error: index out of range, range: %lu, index: %llu\n", sizeLL(interface->nodes) - 1, index); return;
    }
    
    //get file information
    SilentNode* node = get_valueLL(interface->nodes, index);
    
    //create path to file
    size_t folder_length     = strlen(folder);
    char file_path[PATH_MAX] = { 0 };
    
    strcpy(file_path, folder);
    
    //add slash on the back if needed
    if(file_path[folder_length - 1] != '/') {
        file_path[folder_length]     = '/';
        file_path[folder_length + 1] =  0;
    }
    
    strcat(file_path, node->name);
    
    //create file
    FILE* out = fopen(file_path, "wb");
    
    if(out == NULL) {
        printf("Error: cannot open file %s\n", node->name);
        perror(" ");
        return;
    }
    
    fseek(interface->file, node->offset, SEEK_SET);
    
    //TODO: optimize
    //manipulate key to it's right value
    if(index > 0) {
        
        for(u32 i = 0; i < index; i++) {

            SilentNode* prev_node = get_valueLL(interface->nodes, i);
            
            for(u64 j = 0; j < prev_node->size; j += SLURP_SIZE) {
                
                u32 slurp = j + SLURP_SIZE > prev_node->size ? (u32)(prev_node->size - j) : SLURP_SIZE;
                
                for(u32 k = 0; k < slurp; k++) { password[k % password_size]++; }
                
            }
        }
    }
    
    //read and write the data
    for(u64 i = 0; i < node->size; i += SLURP_SIZE) {
        
        u32 slurp = i + SLURP_SIZE > node->size ? (u32)(node->size - i) : SLURP_SIZE;
        
        char* buffer = my_malloc(slurp);
        
        eread(buffer, slurp, interface->encryption, interface->file);
        
        if(interface->encryption) {
            for(u32 j = 0; j < slurp; j++) {
                buffer[j] ^= password[j % password_size]++;
            }
        }
        
        ewrite(buffer, slurp, false, out);
        
        my_free(buffer);
    }
    
    fclose(out);
    
    set_attributes(file_path, node->attributes);
    
    printf("Unsilencing done!\n");
}
//creates silent package + optional ecryption
void silence            (char* folder, const char* output, bool encryption, char* password) {
    
    //check argument validity
    if(encryption && password == NULL) {
        printf("Error cannot encrypt with empty password\n");
        return;
    }
    
    //set seed for rand()
    srand((u32)time(NULL));
    
    printf("Start silencing...\n");
    
    FILE* out = fopen(output, "wb");
    if(out == NULL) { printf("Error: cannot create output file\n"); return; }
    
    LinkedList* files = newLL();
    
    getEntries(folder, NULL, files);
    
    //write header
    ewrite("SPC", 3, false, out);
    
    //write encryption info
    ewrite((char*)&encryption, 1, false, out);
    
    //generate key
    u8 key[4] = {
        rand(),
        rand(),
        rand(),
        rand()
    };
    
    //if encryption generate hash of the password
    u16 password_size = 0;
    if(encryption) {
        password_size = strlen(password);
        u32 k         = lookup3hash(password, password_size);
        memcpy(&key[0], &k, sizeof(u32));
    }
    
    //write key
    ewrite((char*)&key[0], 4, encryption, out);
    
    //split entries to directories and files
    LinkedList* file_buffer = newLL();
    LinkedList* dir_buffer  = newLL();
    DirEntry*   dir_reference;
    
    for(size_t i = 0; i < sizeLL(files); i++) {
        
        dir_reference = get_valueLL(files, i);
        
        if(dir_reference->type == DT_REG) {
            
            pushLL(file_buffer, dir_reference);
            
#ifdef DEBUG
            printf("Entry [type: %u, path: %s]\n", dir_reference->type, dir_reference->path);
#endif
            
        } else if(dir_reference->type == DT_DIR) {
            
            pushLL(dir_buffer, dir_reference);
            
#ifdef DEBUG
            printf("Entry [type: %u, path: %s]\n", dir_reference->type, dir_reference->path);
#endif
        }
    }
    
    //write number of files and directories
    u32 files_size = (u32)sizeLL(file_buffer);
    ewrite((char*)(&files_size), sizeof(u32), encryption, out);
#ifdef DEBUG
    printf("Number of files: %u\n", files_size);
#endif
    
    u32 dir_size   = (u32)sizeLL(dir_buffer);
    ewrite((char*)(&dir_size), sizeof(u32), encryption, out);
#ifdef DEBUG
    printf("Number of dirs : %u\n", dir_size);
#endif
    
    //write sizes of file names
    for(size_t i = 0; i < files_size; i++) {
        DirEntry*      e         = get_valueLL(file_buffer, i);
        char*          file_name = e->path;
        unsigned short name_size = (u16)(strlen(file_name));
        ewrite((char*)&name_size, sizeof(u16), encryption, out);
        
#ifdef DEBUG
        printf("Written file name size [%hu]\n", name_size);
#endif
    }
    
    //write sizes of files
    for(size_t i = 0; i < files_size; i++) {
        DirEntry* e = get_valueLL(file_buffer, i);
        
        char current_in[PATH_MAX];
        strcpy(current_in, folder);
        strcat(current_in, "/");
        strcat(current_in, e->path);
        
        FILE* in       = fopen(current_in, "rb");
        u64   in_size  = fsize(in);
        
        ewrite((char*)&in_size, sizeof(u64), encryption, out);
        
#ifdef DEBUG
        printf("Written file size [%llu]\n", in_size);
#endif
        
        fclose(in);
    }
    
    //write sizes of folder names
    for(size_t i = 0; i < dir_size; i++) {
        DirEntry* e           = get_valueLL(dir_buffer, i);
        char*     folder_name = e->path;
        u16       name_size   = (u16)(strlen(folder_name));
        ewrite((char*)&name_size, sizeof(u16), encryption, out);
        
#ifdef DEBUG
        printf("Written folder name size [%hu]\n", name_size);
#endif
    }
    
    //write folder names
    for(size_t i = 0; i < dir_size; i++) {
        DirEntry* e       = get_valueLL(dir_buffer, i);
        char* folder_name = e->path;
        ewrite(folder_name, strlen(folder_name), encryption, out);
        
#ifdef DEBUG
        printf("Written folder name  [%s]\n", folder_name);
#endif
    }
    
    //write files
    for(size_t i = 0; i < files_size; i++) {
        
        DirEntry* e = get_valueLL(file_buffer, i);
        
        char current_in[PATH_MAX];
        strcpy(current_in, folder);
        strcat(current_in, "/");
        strcat(current_in, e->path);
        
        FILE* in      = fopen(current_in, "rb");
        u64   in_size = fsize(in);
        
        //write file path
        char* f_name  = e->path;
        ewrite(f_name, strlen(f_name), encryption, out);
        
        //write attributes
        u16 attributes = get_attributes(current_in);
        ewrite((char*)&attributes, sizeof(u16), encryption, out);
        
        //write data
        for(u64 j = 0; j < in_size; j += SLURP_SIZE) {
            
            u32 buffer_size = j + SLURP_SIZE > in_size ? (u32)(in_size - j) : SLURP_SIZE;
            
            //load part of file into memory
            char* buffer = my_malloc(buffer_size);
            fread(buffer, 1, buffer_size, in);
            
            //1st encryption using key
            if(encryption) {
                for(u32 k = 0; k < buffer_size; k++) {
                    buffer[k] ^= password[k % password_size]++;
                }
            }
            
            //write that shit
            ewrite(buffer, buffer_size, encryption, out);
            
            my_free(buffer);
        }
        
#ifdef DEBUG
        printf("File written [name: %s size: %llu]\n", current_in, in_size);
#endif
        
        fclose(in);
    }
    
    printf("Silencing done!\nCleaning...\n");
    
    deleteLL(files);
    lazy_deleteLL(file_buffer);
    lazy_deleteLL(dir_buffer);
    fclose(out);
    
    printf("Done\n");
}
//ecrypted write
void ewrite             (char* buffer, u64 size, bool encryption, FILE* f) {
    
    if(encryption) {
        
        char*  new_buffer = my_malloc(size);
        memcpy(new_buffer, buffer, size);
        
        for(u64 i = 0; i < size; i++) {
            new_buffer[i] = SILENT_ENCRYPTION(new_buffer[i]);
        }
        
#ifdef DEBUG
        printf("written: %zu B\n", fwrite(new_buffer, 1, size, f));
#else
        fwrite(new_buffer, 1, size, f);
#endif
        
        my_free(new_buffer);
        
    } else {
#ifdef DEBUG
        printf("written: %zu B\n", fwrite(buffer, 1, size, f));
#else
        fwrite(buffer, 1, size, f);
#endif
        
    }
}
//decrypted read
void eread              (char* buffer, u64 size, bool encryption, FILE* f) {
    
    fread(buffer, 1, size, f);
    
    if(encryption) {
        for(u64 i = 0; i < size; i++) {
            buffer[i] = SILENT_DECRYPTION(buffer[i]);
        }
    }
}
//get directory entries
int getEntries          (char* dir, char* super_folder, LinkedList* files) {
    DIR*           dp;
    struct dirent* dirp;
    
    static const char* FILE_TYPES_STRING[] = {
        "DT_UNKNOWN",
        "DT_FIFO",
        "DT_CHR",
        NULL,
        "DT_DIR",
        NULL,
        "DT_BLK",
        NULL,
        "DT_REG",
        NULL,
        "DT_LNK",
        NULL,
        "DT_SOCK",
        NULL,
        "DT_WHT"
    };
    
    //open directory
    if((dp = opendir(dir)) == NULL) {
        perror("Error: Cannot open dir "); return 1;
    }
    
    //iterate through entries
    while((dirp = readdir(dp)) != NULL) {
        //ignore system files
        if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..") || !strcmp(dirp->d_name, ".DS_Store")) {
            continue;
        }
        
        //save entry type + name
        DirEntry* de = my_malloc(sizeof(DirEntry));
        if(super_folder == NULL) {
            strcpy(de->path, dirp->d_name);
        } else {
            strcpy(de->path, super_folder);
            strcat(de->path, "/");
            strcat(de->path, dirp->d_name);
        }
        
        de->type = dirp->d_type;
        
        if(!(de->type == DT_REG || de->type == DT_DIR)) {
            
            printf("Unsuported file type [%s] [%s]\n", de->path, FILE_TYPES_STRING[de->type]);
            my_free(de);
            continue;
        }
        
        pushLL(files, de);
        
        //if entry is directory recursively get entries in that directory
        if(dirp->d_type == DT_DIR) {
            
            char dir_p[PATH_MAX];
            char sup_p[PATH_MAX];
            
            strcpy(dir_p, dir);
            strcat(dir_p, "/");
            strcat(dir_p, dirp->d_name);
            
            if(super_folder == NULL) {
                strcpy(sup_p, dirp->d_name);
            } else {
                strcpy(sup_p, super_folder);
                strcat(sup_p, "/");
                strcat(sup_p, dirp->d_name);
            }
            
            getEntries(dir_p, sup_p, files);
        }
    }
    
    closedir(dp);
    
    return 0;
}
//create directory
#ifdef _WIN32
int makedir             (const char* path) { return mkdir(path); }
#else
int makedir             (const char* path) { return mkdir(path, 0777); }
#endif
//deallocate silent interface
void free_interface     (SilentInterface* interface) {
    
    if(interface == NULL) {
        printf("Error: interface is already deallocated"); return;
    }
    
    fclose(interface->file);
    deleteLL(interface->nodes);
    
    my_free(interface);
}
//create new silent node
SilentNode* newSNode    (char* name, u64 offset, u64 size, u16 attributes) {
    SilentNode* node = my_malloc(sizeof(SilentNode));
    node->size       = size;
    node->offset     = offset;
    node->attributes = attributes;
    strcpy(node->name, name);
    return node;
}
//get file size
u64 fsize               (FILE* f) {
    fseek  (f, 0, SEEK_END);
    u64    size = ftell(f);
    fseek  (f, 0, SEEK_SET);
    return size;
}
//get file attributes
u16 get_attributes      (char* path) {
    
    struct stat f_attribute;
    
    if (stat(path, &f_attribute) < 0) {
        printf("Error: Cannot get file attributes %s\n", path); perror(" "); return 0;
    } else {
        return f_attribute.st_mode & 0777;
    }
}
//set file attributes
void set_attributes     (char* path, u32 flags) {
    chmod(path, flags);
}
//reverse bits in byte
u8 reverse              (u8 x) {
    x = (x & 0xF0) >> 4 | (x & 0x0F) << 4;
    x = (x & 0xCC) >> 2 | (x & 0x33) << 2;
    x = (x & 0xAA) >> 1 | (x & 0x55) << 1;
    return x;
}
//perform a lookup 3 hash for password correction
#define lookup3mix(a,b,c)         \
{                                 \
a -= c;  a ^= rol(c, 4);  c += b; \
b -= a;  b ^= rol(a, 6);  a += c; \
c -= b;  c ^= rol(b, 8);  b += a; \
a -= c;  a ^= rol(c,16);  c += b; \
b -= a;  b ^= rol(a,19);  a += c; \
c -= b;  c ^= rol(b, 4);  b += a; \
}

#define lookup3final(a,b,c) \
{                           \
c ^= b; c -= rol(b,14);     \
a ^= c; a -= rol(c,11);     \
b ^= a; b -= rol(a,25);     \
c ^= b; c -= rol(b,16);     \
a ^= c; a -= rol(c, 4);     \
b ^= a; b -= rol(a,14);     \
c ^= b; c -= rol(b,24);     \
}

u32 lookup3(void* data, u32 length, u32 init) {
    
    u32 a, b, c;
    char* key;
    u32*  data32 = data;
    
    a = b = c = 0xdeadbeef + ((length) << 2) + init;
    
    while(length > 12) {
        a += *(data32++);
        b += *(data32++);
        c += *(data32++);
        lookup3mix(a, b, c);
        length -= 12;
    }
    
    key = (char*)data32;
    switch(length) {
        case 12: c += ((u32)key[11]) << 24;
        case 11: c += ((u32)key[10])<<16;
        case 10: c += ((u32)key[9])<<8;
        case 9 : c += key[8];
        case 8 : b += ((u32)key[7])<<24;
        case 7 : b += ((u32)key[6])<<16;
        case 6 : b += ((u32)key[5])<<8;
        case 5 : b += key[4];
        case 4 : a += ((u32)key[3])<<24;
        case 3 : a += ((u32)key[2])<<16;
        case 2 : a += ((u32)key[1])<<8;
        case 1 : a += key[0];
            break;
        case 0 : return c;
    }
    
    lookup3final(a, b, c);
    
    return c;
}
//easier access to lookup3 hash function
u32 lookup3hash(char* data, u32 length) {
    return lookup3(data, length, 0xfab99bae);
}
