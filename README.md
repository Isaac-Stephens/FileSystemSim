# FileSystemSim
Author: Isaac Stephens

File System Simulation for CS3800 - Operating Systems

This program is a simple command-line file system simulator written in C++. It allows users to create and manage directories and files using basic Linux-style commands. All files and directories are simulated in memory. No real files are created on disk.

The system starts at the root directory `/` and supports unlimited nesting of directories and files. It includes basic navigation, file creation, deletion, and permission modification.

The simulated file system is implemented using a tree-based data structure, where each node represents either a file or a directory. This is achieved through a custom Node structure that stores the name, permissions, last modified time, type (file or directory), and a pointer to its parent. Directory nodes additionally contain an unordered_map that maps names to child nodes, allowing fast lookup of files and subdirectories by name. This design naturally models a hierarchical filesystem and allows efficient traversal for commands such as cd, pwd, and ls. The use of parent pointers enables upward navigation, while dynamic memory allocation allows the structure to grow without predefined limits on depth or size.

Supports the following commands:
```bash
pwd
ls
ls -l
cd <dir | .. | /absolute/path>
mkdir <dir>
rmdir <dir>
touch <file>
rm <file>
chmod <mode> <file|dir>
help
exit | quit
```

To run:
```bash
chmod +x run.sh
./run.sh
```
