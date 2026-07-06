#ifndef __FILESYSTEM__
#define __FILESYSTEM__

typedef struct FileNode {
    char* name;               // 檔名
    char* content;            // 檔案內容
    struct FileNode* next;    // 下一個檔案
} tFileNode;

typedef struct DirectoryNode {
    char* name;                         // 目錄名稱
    int level;                          // 目錄層級
    struct DirectoryNode* parent;       // 父目錄指標
    struct DirectoryNode* child;        // 子目錄鏈表的頭指標
    struct DirectoryNode* next_sibling; // 同層下一個目錄
    struct FileNode* file;              // 該目錄中的檔案鏈表
} tDirectoryNode;

typedef struct FileSystem {
    tDirectoryNode* root;               // 根目錄指標
    int partition_size;                 // 分區大小
    int total_inode;                    // 總 inode 數量
    int used_inode;                     // 已使用 inode 數量
    int total_block;                    // 總區塊數量
    int used_block;                     // 已使用區塊數量
    int block_size;                     // 區塊大小
    int free_space;                     // 剩餘空間
} tFileSystem;

tFileSystem* createFileSystem(int size);
tDirectoryNode* createDirectory(tFileSystem* FileSystem, tDirectoryNode* parent, const char* name);
tFileNode* createFile(tDirectoryNode* dir, const char* name, const char* content);

void deleteDirectory(tDirectoryNode* dir);
void deleteFile(tDirectoryNode* dir, tFileNode* file);

tDirectoryNode* changeDirectory(tFileSystem* FileSystem, tDirectoryNode* current, const char* name);

#endif
