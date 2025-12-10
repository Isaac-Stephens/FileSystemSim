// filesim.cpp
// Isaac Stephens
// CS3800: Operating Systems

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <ctime>

struct Node {
    std::string name;
    std::string permissions;
    std::time_t modified;
    bool isDirectory; // false = file, true = dir
    Node* parent;
    std::unordered_map<std::string, Node*> children;

    Node(const std::string& n, bool dir, Node* p)
        : name(n), permissions("755"), isDirectory(dir), parent(p) {
        modified = std::time(nullptr);
    }
};

Node* root = nullptr;
Node* cwd  = nullptr; // curent working directory

// splits a command line string into tokens using whitespace
std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string temp;
    while (ss >> temp) {
        tokens.push_back(temp);
    }
    return tokens;
}
// converts a time_t value into a readable string
std::string timeToString(std::time_t t) {
    char buf[64];
    std::tm* tm_info = std::localtime(&t);
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", tm_info);
    return std::string(buf);
}

// prints the full path of the current working directory
// works by walking UP the tree using parent pointers
void cmd_pwd() {
    Node* cur = cwd;
    std::string path = "";

    // walk backward from cwd to root
    while (cur != nullptr) {
        if (cur->parent == nullptr) {   // we reached root
            path = "/" + path;
            break;
        }
        path = cur->name + "/" + path;
        cur = cur->parent;
    }

    // remove trailing slash if needed
    if (path.size() > 1 && path.back() == '/')
        path.pop_back();

    std::cout << path << "\n";
}

// lists the names of all files and directories in the current working directory
void cmd_ls(const std::vector<std::string>& args) {
    bool longFormat = false;

    // check for "-l" flag
    if (args.size() > 1 && args[1] == "-l")
        longFormat = true;

    for (auto& pair : cwd->children) {
        Node* n = pair.second;

        if (!longFormat) {
            // normal ls prints names only
            std::cout << n->name << "\n";
        }
        else {
            // long format: type + permissions + time + name
            std::string type = n->isDirectory ? "d" : "-";
            std::string timeStr = timeToString(n->modified);

            std::cout << type << n->permissions << "  "
                      << timeStr << "  "
                      << n->name << "\n";
        }
    }
}

// creates a new directory in the current working directory
void cmd_mkdir(const std::vector<std::string>& args) {
    // ensure directory name was provided
    if (args.size() < 2) {
        std::cout << "mkdir: missing operand\n";
        return;
    }

    std::string name = args[1];

    // prevent duplicate names
    if (cwd->children.count(name)) {
        std::cout << "mkdir: directory already exists\n";
        return;
    }

    // create and insert new directory node
    Node* dir = new Node(name, true, cwd);
    cwd->children[name] = dir;
}

// creates a file if it does not exist, else, just updates timestamp
void cmd_touch(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "touch: missing file name\n";
        return;
    }

    std::string name = args[1];
    if (cwd->children.count(name)) {
        cwd->children[name]->modified = std::time(nullptr);
        return;
    }

    Node* file = new Node(name, false, cwd);
    cwd->children[name] = file;
}

// moves cwd to target
void cmd_cd(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "cd: missing operand\n";
        return;
    }

    std::string dest = args[1];

    // handle "cd .." (move up one directory)
    if (dest == "..") {
        if (cwd->parent != nullptr)
            cwd = cwd->parent;
        return;
    }

    // ensure target exists
    if (!cwd->children.count(dest)) {
        std::cout << "cd: no such file or directory\n";
        return;
    }

    Node* target = cwd->children[dest];

    // ensure target is a directory
    if (!target->isDirectory) {
        std::cout << "cd: not a directory\n";
        return;
    }

    cwd = target;
}

// removes a file from the cwd
void cmd_rm(const std::vector<std::string>& args) {
    // ensure a filename was provided
    if (args.size() < 2) {
        std::cout << "rm: missing file operand\n";
        return;
    }

    std::string name = args[1];

    // check if the file exists in the current directory
    if (!cwd->children.count(name)) {
        std::cout << "rm: file does not exist\n";
        return;
    }

    Node* target = cwd->children[name];

    // prevent deleting directories with rm
    if (target->isDirectory) {
        std::cout << "rm: cannot remove '" << name
                  << "': is a directory\n";
        return;
    }

    delete target;
    cwd->children.erase(name);
}

// removes an empty directory form cwd
void cmd_rmdir(const std::vector<std::string>& args) {
    // ensure a directory name was provided
    if (args.size() < 2) {
        std::cout << "rmdir: missing directory operand\n";
        return;
    }

    std::string name = args[1];

    // check if the directory exists in the current directory
    if (!cwd->children.count(name)) {
        std::cout << "rmdir: directory does not exist\n";
        return;
    }

    Node* target = cwd->children[name];

    // ensure the target is actually a directory
    if (!target->isDirectory) {
        std::cout << "rmdir: '" << name << "' is not a directory\n";
        return;
    }

    // ensure the directory is empty before deletion
    if (!target->children.empty()) {
        std::cout << "rmdir: directory not empty\n";
        return;
    }

    delete target;
    cwd->children.erase(name);
}


int main() {
    root = new Node("", true, nullptr);
    cwd  = root;

    std::string line;

    while (true) {
        std::cout << "$ ";
        std::getline(std::cin, line);

        auto tokens = split(line);
        if (tokens.empty()) continue;

        const std::string& cmd = tokens[0];

        if (cmd == "pwd") {
            cmd_pwd();
        }
        else if (cmd == "ls") {
            cmd_ls(tokens);
        }
        else if (cmd == "mkdir") {
            cmd_mkdir(tokens);
        }
        else if (cmd == "touch") {
            cmd_touch(tokens);
        }
        else if (cmd == "cd") {
            cmd_cd(tokens);
        }
        else if (cmd == "rm") {
            cmd_rm(tokens);
        }
        else if (cmd == "rmdir") {
            cmd_rmdir(tokens);
        }
        else if (cmd == "exit" || cmd == "quit") {
            break;
        }
        else {
            std::cout << "Unknown command\n";
        }
    }

    return 0;
}
