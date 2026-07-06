#include "FileSystem.h"
#include "function.h"

// 創建檔案系統
tFileSystem* createFileSystem(int size) {
    tFileSystem* fileSystem = (tFileSystem*)malloc(sizeof(tFileSystem));
    if (fileSystem) {
        fileSystem->root = createDirectory(fileSystem , NULL, "root");
        fileSystem->partition_size = size;
        fileSystem->total_inode = size / 1024;
        fileSystem->used_inode = 0;
        fileSystem->total_block = size / 1024;
        fileSystem->used_block = 0;
        fileSystem->block_size = 1024;
        fileSystem->free_space = size;
    }
    return fileSystem;
}

// 新增目錄
tDirectoryNode* createDirectory(tFileSystem* FileSystem, tDirectoryNode* parent, const char* name) {
    tDirectoryNode* newDir = (tDirectoryNode*)malloc(sizeof(tDirectoryNode));
    if (newDir == NULL) {// 檢查是否成功分配記憶體
        printf("Error: Memory allocation for directory failed.\n");
        return NULL;
    }

    newDir->name = (char*)malloc(strlen(name) + 1);
    if (newDir->name == NULL) {
        printf("Error: Memory allocation for directory name failed.\n");
        free(newDir);
        return NULL;
    }
    strcpy(newDir->name, name);
    newDir->parent = parent;
    newDir->child = NULL;
    newDir->next_sibling = NULL;
    newDir->file = NULL;

    if (parent != NULL) {
        if (parent->child == NULL) {
            parent->child = newDir;
        } 
        else {
            tDirectoryNode* sibling = parent->child;
            while (sibling->next_sibling) {
                sibling = sibling->next_sibling;
            }
            sibling->next_sibling = newDir;
        }
    }

    return newDir;
}

// 新增檔案
tFileNode* createFile(tDirectoryNode* dir, const char* name, const char* content) {
    if (dir == NULL) {
        printf("Error: Directory does not exist.\n");
        return NULL;
    }

    tFileNode* newFile = (tFileNode*)malloc(sizeof(tFileNode));
    if (newFile == NULL) {
        printf("Error: Memory allocation for file failed.\n");
        return NULL;
    }

    newFile->name = (char*)malloc(strlen(name) + 1);
    newFile->content = (char*)malloc(strlen(content) + 1);

    if ((newFile->name == NULL) || (newFile->content == NULL)) {
        printf("Error: Memory allocation for file properties failed.\n");
        free(newFile);
        return NULL;
    }

    strcpy(newFile->name, name);
    strcpy(newFile->content, content);
    newFile->next = NULL;

    if (dir->file == NULL) {
        dir->file = newFile;
    } 
    else {
        tFileNode* current = dir->file;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newFile;
    }

    return newFile;
}

// 刪除目錄
void deleteDirectory(tDirectoryNode* dir) {
    if (dir == NULL) return;

    // 遞迴刪除子目錄
    tDirectoryNode* child = dir->child;
    while (child != NULL) {
        tDirectoryNode* nextChild = child->next_sibling;
        deleteDirectory(child);
        child = nextChild;
    }

    // 刪除檔案
    tFileNode* file = dir->file;
    while (file != NULL) {
        tFileNode* nextFile = file->next;
        free(file->name);
        free(file->content);
        free(file);
        file = nextFile;
    }

    // 刪除目錄本身
    if ((dir->parent != NULL) && (dir->parent->child == dir)) {
        dir->parent->child = dir->next_sibling;
    } 
    else if (dir->parent != NULL) {
        tDirectoryNode* sibling = dir->parent->child;
        while ((sibling != NULL) && (sibling->next_sibling != dir)) {
            sibling = sibling->next_sibling;
        }
        if (sibling != NULL) {
            sibling->next_sibling = dir->next_sibling;
        }
    }

    free(dir->name);
    free(dir);
}

// 刪除檔案
void deleteFile(tDirectoryNode* dir, tFileNode* file) {
    if ((dir == NULL) || (file == NULL)) return;

    if (dir->file == file) {
        dir->file = file->next;
    } else {
        tFileNode* current = dir->file;
        while ((current->next != NULL) && (current->next != file)) {
            current = current->next;
        }
        if (current->next != NULL) {
            current->next = file->next;
        }
    }

    free(file->name);
    free(file->content);
    free(file);
}

// 切換目錄
tDirectoryNode* changeDirectory(tFileSystem* FileSystem, tDirectoryNode* current, const char* name) {
    if (strcmp(name, "..") == 0) {
        return current->parent ? current->parent : current;
    }

    tDirectoryNode* child = current->child;
    while (child != NULL) {
        if (strcmp(child->name, name) == 0) {
            return child;
        }
        child = child->next_sibling;
    }

    printf("Error: Directory '%s' not found.\n", name);
    return current;
}
