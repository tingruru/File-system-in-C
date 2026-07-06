# Customized Command-Line Virtual File System

## 專案目標

實作一個自訂虛擬檔案系統，模擬 Unix/Linux 檔案系統的基本操作，讓使用者透過命令列介面執行以下功能：

- 建立/刪除資料夾與檔案
- 瀏覽目錄內容
- 將本機檔案上傳或下載
- 顯示/編輯檔案內容
- 儲存與載入虛擬磁碟映像（支援密碼保護）
- 提供簡易文字檔案建立與修改功能

---

## 系統架構

- **檔案系統核心 (`FileSystem.h/c`)**
  - 以樹狀結構表示資料夾（目錄）與檔案節點。
  - 使用 `tDirectoryNode` 和 `tFileNode` 管理結構。

- **指令模組 (`function.h/c`)**
  - 實作 `ls`, `cd`, `mkdir`, `put`, `get`, `edit`, `cat`, `exit` 等指令。
  - 含有文字檔案處理、dump 存檔、密碼保護等功能。

- **主控模組 (`main.c`)**
  - 提供互動式命令列界面。
  - 啟動時可選擇載入 `.img` 或新建分區。

---

### 支援指令總覽

| 指令         | 功能                           |
|--------------|--------------------------------|
| `ls`         | 列出目前目錄內容               |
| `cd <name>`  | 切換資料夾 / `..` 返回上層     |
| `mkdir <n>`  | 建立新資料夾                   |
| `rmdir <n>`  | 刪除資料夾（含所有子項）       |
| `rm <file>`  | 刪除檔案                       |
| `put <f>`    | 上傳本機檔案至虛擬系統         |
| `get <f>`    | 將虛擬檔案存到 `dump/`         |
| `create <f>` | 新增空白檔案                   |
| `edit <f>`   | 編輯檔案內容（以 EOF 結束）    |
| `cat <f>`    | 顯示檔案內容                   |
| `status`     | 顯示磁區使用資訊               |
| `exit`       | 離開並儲存為加密 `.img` 檔案  |

---

## 額外功能（Extra Credits）

- 使用者離開系統前需輸入密碼，以保護 `.img` 檔案。
- 支援文字檔案的建立與編輯操作。
- 支援資料夾與檔案的遞迴操作與導出。

---

## 二、結構與流程圖（包含記憶體結構）

### （1）檔案系統結構圖（Directory/File 結構）

```markdown
tFileSystem
 └── root
      ├── child ── FolderA
      │            ├── child ── SubFolderA1
      │            ├── file  ── FileA1.txt
      │            └── next_sibling ── FolderB
      │                               └── file ── FileB1.txt
```

### （2）指令執行流程簡圖（範例：put）

```plaintext
使用者輸入 put hello.txt
        │
        ↓
讀取 hello.txt 檔案內容
        │
        ↓
建立 FileNode 並加至目前目錄
        │
        ↓
更新目錄結構 & 印出成功訊息
```

---
