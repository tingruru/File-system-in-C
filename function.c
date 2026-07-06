#include "function.h"

void ls(tFileSystem* FileSystem, tDirectoryNode* target) {
    if (target == NULL) {
        target = FileSystem->root;
    }
    printf("Directories:\n");
    tDirectoryNode* child = target->child;
    while (child) {
        printf("%s\n", child->name);
        child = child->next_sibling;
    }

    printf("Files:\n");
    tFileNode* file = target->file;
    while (file) {
        printf("%s\n", file->name);
        file = file->next;
    }
    printf("ls done\n");
}

tDirectoryNode* cd(tFileSystem* FileSystem, tDirectoryNode* current, const char* name) {
    current = changeDirectory(FileSystem, current, name);
}

void rm(tDirectoryNode* target, const char* name) {
    tFileNode* file = target->file;
    tFileNode* prev = NULL;

    while (file) {
        if (strcmp(file->name, name) == 0) {
            if (prev) {
                prev->next = file->next;
            } else {
                target->file = file->next;
            }
            free(file->name);
            free(file->content);
            free(file);
            printf("rm done\n");
            return;
        }
        prev = file;
        file = file->next;
    }

    printf("No such file\n");
}

void mkdir(tFileSystem* FileSystem, tDirectoryNode* target, const char* name) {
    if (target == NULL) {
        target = FileSystem->root;
    }
    tDirectoryNode* newDir = createDirectory(FileSystem, target, name);
    if (newDir != NULL) {
        printf("mkdir done\n");
    } else {
        printf("mkdir failed\n");
    }
}


void rmdir(tDirectoryNode* target, const char* name) {
    tDirectoryNode* child = target->child;
    tDirectoryNode* prev = NULL;

    while (child != NULL) {
        if (strcmp(child->name, name) == 0) {
            if (prev) {
                prev->next_sibling = child->next_sibling;
            } 
            else {
                target->child = child->next_sibling;
            }
            deleteDirectory(child);
            printf("rmdir done\n");
            return;
        }
        prev = child;
        child = child->next_sibling;
    }

    printf("No such directory\n");
}

void put(tDirectoryNode* target, const char* name) {
     if (target == NULL) {
        printf("Error: No target directory specified.\n");
        return;
    }

    // 打開檔案
    FILE* file = fopen(name, "r");
    if (file == NULL) {
        printf("Error: Failed to open file '%s'.\n", name);
        return;
    }
    char content[2048];
    // 準備讀取檔案內容
    size_t bytesRead = fread(content, sizeof(char), sizeof(content) - 1, file);
    fclose(file); // 關閉檔案

    if (bytesRead == 0) {
        printf("Error: File '%s' is empty or could not be read.\n", name);
        return;
    }

    content[bytesRead] = '\0'; // 確保內容以 '\0' 結尾，成為有效字串

    // 將檔案內容加入虛擬檔案系統
    tFileNode* newFile = createFile(target, name, content);
    if (newFile == NULL) {
        printf("Error: Failed to create file '%s' in the virtual file system.\n", name);
    } 
    else {
        printf("File '%s' added successfully to directory '%s'.\n", name, target->name);
        printf("put done\n");
    }
}

void get(tDirectoryNode* target, const char* name) {
    // 確保當前目錄有效
    if (target == NULL) {
        printf("Error: Current directory does not exist.\n");
        return;
    }
    // 查找檔案
    tFileNode* file = target->file;
    while (file != NULL) {
        if (strcmp(file->name, name) == 0) {
            // 構立檔案路徑：dump/<filename>
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "dump/%s", name);
            // 檢查是否已存在同名檔案
            FILE* localFile = fopen(filepath, "r");
            if (localFile != NULL) {
                fclose(localFile);
                printf("Warning: File '%s' already exists in 'dump'. Overwrite? (y/n): ", name);
                char choice;
                scanf(" %c", &choice);
                getchar(); // 清理緩衝區
                if (choice != 'y' && choice != 'Y') {
                    printf("Export cancelled.\n");
                    return;
                }
            }
            // 將檔案內容寫入 dump 資料夾
            FILE* newFile = fopen(filepath, "w");
            if (newFile == NULL) {
                printf("Error: Failed to create file in 'dump' folder.\n");
                return;
            }
            fprintf(newFile, "%s", file->content);
            fclose(newFile);
            printf("File '%s' saved successfully in 'dump/%s'.\n", name, name);
            printf("get done\n");
            return;
        }
        file = file->next;
    }

    // 檔案未找到
    printf("No such file\n");
}

void cat(tDirectoryNode* target, const char* name) {
    tFileNode* file = target->file;
    while (file != NULL) {
        if (strcmp(file->name, name) == 0) {
            printf("%s\n", file->content);
            printf("cat done\n");
            return;
        }
        file = file->next;
    }

    printf("No such file\n");
}

void status(tFileSystem* fileSystem) {
    printf("total inode: %d\n", fileSystem->total_inode);
    printf("used inode: %d\n", fileSystem->used_inode);
    printf("total block: %d\n", fileSystem->total_block);
    printf("used block: %d\n", fileSystem->used_block);
    printf("block size: %d\n", fileSystem->block_size);
    printf("free space: %d\n", fileSystem->free_space);
    printf("status done\n");
}

void help() {
    printf("list of commands\n");
    printf("'ls' list directory\n");
    printf("'cd' change directory\n");
    printf("'rm' remove file\n");
    printf("'mkdir' make directory\n");
    printf("'rmdir' remove directory\n");
    printf("'put' put file into the space\n");
    printf("'get' get file from the space\n");
    printf("'cat' show content\n");
    printf("'status' show status\n");
    printf("'help' show this help message\n");
    printf("'exit' exit program\n");
    printf("help done\n");
}

void exit_and_img(tFileSystem* fileSystem) {
    char password[256];
    printf("Enter password: ");
    scanf("%s", password);
    getchar(); // 清理緩衝區
    printf("Creating dump file...\n");
    storeDumpFile("dump.img", password, fileSystem);
    printf("exit done\n");
}

void storeDumpFile(char* dumpFileName, const char* password, tFileSystem* fileSystem) {
    FILE* dumpFile = fopen(dumpFileName, "w");
    if (dumpFile == NULL) {
        printf("Error: Failed to create dump file.\n");
        return;
    }

    fprintf(dumpFile, "%d\n", fileSystem->total_inode);
    fprintf(dumpFile, "%d\n", fileSystem->used_inode);
    fprintf(dumpFile, "%d\n", fileSystem->total_block);
    fprintf(dumpFile, "%d\n", fileSystem->used_block);
    fprintf(dumpFile, "%d\n", fileSystem->block_size);
    fprintf(dumpFile, "%d\n", fileSystem->free_space);

    tDirectoryNode* root = fileSystem->root;
    tDirectoryNode* current = root;
    while (current != NULL) {
        fprintf(dumpFile, "%s\n", current->name);
        tDirectoryNode* child = current->child;
        while (child != NULL) {
            fprintf(dumpFile, "%s\n", child->name);
            child = child->next_sibling;
        }
        tFileNode* file = current->file;
        while (file != NULL) {
            fprintf(dumpFile, "%s\n", file->name);
            fprintf(dumpFile, "%s\n", file->content);
            file = file->next;
        }
        current = current->next_sibling;
    }

    fclose(dumpFile);
    printf("Dump file '%s' created successfully.\n", dumpFileName);
}

void loadDumpFile(char* dumpFileName, const char* password, tFileSystem* fileSystem) {
    FILE* dumpFile = fopen(dumpFileName, "r");
    if (dumpFile == NULL) {
        printf("Error: Failed to open dump file.\n");
        return;
    }

    fscanf(dumpFile, "%d\n", &fileSystem->total_inode);
    fscanf(dumpFile, "%d\n", &fileSystem->used_inode);
    fscanf(dumpFile, "%d\n", &fileSystem->total_block);
    fscanf(dumpFile, "%d\n", &fileSystem->used_block);
    fscanf(dumpFile, "%d\n", &fileSystem->block_size);
    fscanf(dumpFile, "%d\n", &fileSystem->free_space);

    char buffer[256];
    while (fscanf(dumpFile, "%s\n", buffer) != EOF) {
        if (strcmp(buffer, "root") == 0) {
            continue;
        }
        tDirectoryNode* current = createDirectory(fileSystem, fileSystem->root, buffer);
        while (fscanf(dumpFile, "%s\n", buffer) != EOF) {
            if (buffer[0] == '/') {
                break;
            }
            createDirectory(fileSystem, current, buffer);
        }
        while (fscanf(dumpFile, "%s\n", buffer) != EOF) {
            if (buffer[0] == '/') {
                break;
            }
            char content[2048];
            strcpy(content, buffer);
            createFile(current, buffer, content);
        }
    }

    fclose(dumpFile);
    printf("Dump file '%s' loaded successfully.\n", dumpFileName);
}

void createTextFile(tDirectoryNode* target, const char* name, const char* content) {
    tFileNode* newFile = createFile(target, name, content);
    if (newFile != NULL) {
        printf("Text file '%s' created successfully in directory '%s'.\n", name, target->name);
    } else {
        printf("Error: Failed to create text file '%s' in directory '%s'.\n", name, target->name);
    }
}

void editTextFile(tDirectoryNode* target, const char* name) {
    tFileNode* file = target->file;
    while (file != NULL) {
        if (strcmp(file->name, name) == 0) {
            printf("Enter new content for file '%s' (type 'EOF' to finish):\n", name);
            
            char content[2048];
            char buffer[256];
            content[0] = '\0'; // 初始化內容為空字串
            
            while (1) {
                fgets(buffer, sizeof(buffer), stdin);
                buffer[strcspn(buffer, "\n")] = '\0'; // 移除換行符
                
                if (strcmp(buffer, "EOF") == 0) {
                    break; // 如果輸入 "EOF"，結束輸入
                }
                
                // 確保總內容不超過限制
                if (strlen(content) + strlen(buffer) + 1 >= sizeof(content)) {
                    printf("Error: Content exceeds maximum size.\n");
                    break;
                }
                strcat(content, buffer); // 添加當前輸入行
                strcat(content, "\n");   // 添加換行符
            }
            // 更新檔案內容
            free(file->content);
            file->content = (char*)malloc(strlen(content) + 1);
            strcpy(file->content, content);
            
            printf("Text file '%s' edited successfully.\n", name);
            return;
        }
        file = file->next;
    }

    printf("No such file\n");
}
