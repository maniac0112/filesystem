#include <iostream>
#include <string> 
#include <chrono>
#include <memory>

struct FileAttribute{
    std::string _file_name; 
    std::chrono::steady_clock::time_point _creation_time; 
    std::chrono::steady_clock::time_point _last_modified_time; 
};

class FileNode{
public: 
    FileNode() = default; 
    virtual ~FileNode() = default;
    virtual void listFiles(int indent = 0) const = 0;
    virtual bool isFile() = 0; 
    virtual size_t getSize() = 0;
    void initialize(const std::string& file_name){
        _file_attribute._file_name = file_name;
        _file_attribute._creation_time = std::chrono::steady_clock::now();
        _file_attribute._last_modified_time = _file_attribute._creation_time; 
        //not doing clock::now() to avoid inconsistency b/w creation and modified
    }
protected: 
    FileAttribute _file_attribute; 
};

class File : public FileNode{ 
public: 
    File(): _size(0){
    }

    bool isFile() override{
        return true; 
    }

    size_t getSize() override{
        return _size; 
    }

    void listFiles(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "- " << _file_attribute._file_name
                    << " (" << _size << " bytes)" << "\n";
    }

    void fill(const char* buffer, size_t size){
        _file_contents = std::move(std::string(buffer, size));
        _size = size; 
    }


private: 
    std::string _file_contents; 
    size_t _size; 
};

class Directory : public FileNode{ 
public: 
    bool isFile() override{
        return false; 
    }

    size_t getSize() override{
        size_t total_size = 0; 
        for (auto &node: _files){
            total_size+= node.second->getSize();
        }
        return total_size;
    }

    void listFiles(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "+ " << _file_attribute._file_name << "/\n";
        for (auto& filenode : _files) {
            filenode.second->listFiles(indent + 2);
        }
    }

    void addFile(std::string_view file_path, const char* buffer, size_t file_size = 0){
        size_t idx = get(file_path);
        if (idx == file_path.size() || file_path[idx]=='\\'){
            //This is a new/existing directory 
            std::string directory(file_path.substr(0,idx));
            if (_files.find(directory) == _files.end()){
                //This is a new directory 
                std::unique_ptr<FileNode> new_directory = std::make_unique<Directory>();
                new_directory->initialize(directory);
                _files[directory] = std::move(new_directory);
            }

            if (idx != file_path.size()){
                //we need to recurse further
                std::string_view new_file_path = file_path.substr(idx+1);
                static_cast<Directory*>(_files[directory].get())->addFile(new_file_path, buffer, file_size);
            }

        }else{

            //This is a terminal file 
            std::unique_ptr<FileNode> new_file = std::make_unique<File>();
            new_file->initialize(std::string(file_path));
            static_cast<File*>(new_file.get())->fill(buffer, file_size);
            _files[std::string(file_path)] = std::move(new_file);
        }
    }

    void deleteFile(std::string_view file_path){
        //Could be a path to directory
        size_t idx = get(file_path);
        if (idx!= file_path.size() && file_path[idx]=='\\'){
            //This is a new/existing directory 
            std::string directory(file_path.substr(0,idx));
            if (_files.find(directory) == _files.end()){
                //invalid path
                return ;
            }

            //we need to recurse further
            std::string_view new_file_path = file_path.substr(idx+1);
            static_cast<Directory*>(_files[directory].get())->deleteFile(new_file_path);
        }else{
            _files.erase(std::string(file_path)); //This should trigger the recursive deletion
        }   
    }

private: 
    std::unordered_map<std::string, std::unique_ptr<FileNode>> _files; 
    size_t get(const std::string_view file_path){
        
        size_t pos = 0;
        size_t file_path_size = file_path.size();
        while(pos<file_path_size && file_path[pos]!='\\'){
            pos++;
        }

        return pos;
    }

};


class FileSystem{
public: 
    static std::shared_ptr<FileSystem> getInstance(){
        if (_instance == nullptr){
            _instance.reset(new FileSystem());
        }

        return _instance;
    }
    

    void addFile(const std::string_view file_path, const char* buffer, size_t file_size){
        _root->addFile(file_path,buffer, file_size);
    }

    void DeleteFile(const std::string_view file_path){
        _root->deleteFile(file_path);
    }

    void listAllFiles(){
        _root->listFiles();
    }

    size_t getTotalSize() const {
        return _root->getSize();
    }

private: 
    FileSystem(): _root(std::make_unique<Directory>()){  
        _root->initialize(".");
    }

    static std::shared_ptr<FileSystem> _instance; 
    std::unique_ptr<Directory> _root;
};

std::shared_ptr<FileSystem> FileSystem::_instance = nullptr; 


int main() {
    std::shared_ptr<FileSystem> fs = FileSystem::getInstance();

    // Create root files and dirs
    fs->addFile("file1.txt", "hello", 5);
    fs->addFile("file2.txt", "world!", 6);

    // Create nested directory structure
    fs->addFile("dir1\\file1.txt", "data123", 7);
    fs->addFile("dir1\\file2.txt", "foo", 3);
    fs->addFile("dir1\\dir2\\file3.txt", "barbaz", 6);
    fs->addFile("dir3\\file4.txt", "abcdefgh", 8);

    // Overwrite a file
    fs->addFile("dir3\\file4.txt", "overwrite", 9);

    // Add more nested structure
    fs->addFile("dir3\\sub1\\sub2\\file5.txt", "deep", 4);

    // Delete a single file
    fs->DeleteFile("file2.txt");

    // Delete a directory (recursively)
    fs->DeleteFile("dir1");

    // Print file system structure
    std::cout << "[Final FileSystem Structure]:\n";
    fs->listAllFiles();  // Add this method to FileSystem as shown below
}
