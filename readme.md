# 🗂️ In-Memory File System

An object-oriented in-memory file system in C++, involving design principles and patterns like - **Composite**, **Singleton**, and **Smart Pointers (RAII)**.

---

## 📌 Features

- ✅ Create nested directories and files
- ✅ Recursive deletion of directories
- ✅ Overwrite existing files
- ✅ List the file system tree in a structured format
- ✅ Track file attributes (name, creation time, last modified time)
- ✅ Polymorphic base class for directory and file nodes
- ✅ Memory-safe using `std::unique_ptr` and `std::shared_ptr`

---

## 🧱 Design Patterns Used

| Pattern       | Usage                                                           |
|---------------|------------------------------------------------------------------|
| Composite     | `FileNode` is the base interface for both `Directory` and `File`|
| Singleton     | `FileSystem` instance is globally accessible                    |
| RAII / Smart Pointers | Uses `unique_ptr` for memory-safe tree structure         |

---

## 🗃️ File Structure

├── FileSystem.cpp   # Main implementation
└── README.md        # This file