# Command-line Text Viewer (CTV)

A simple interactive text-based file browser and text viewer for Linux, written in C++.

![image](https://github.com/user-attachments/assets/a150d6e6-fd66-4bd4-8234-01ecbfea1dff)

## Overview

CTV is a command-line version of my Qt-based text viewer found here https://github.com/q5sys/qtv . It provides an interactive TUI (Text User Interface) that allows you to:

- Browse directories
- List files with filtering for text and source code files
- View the contents of text files

## Features

- Split-screen interface with file browser on the left and file viewer on the right
- Directory navigation using arrow keys
- File listing with size information
- Text file viewing with line numbers
- File filtering (supports .txt, .sh, .md, .pro, .cpp, .h, .conf files)
- Keyboard shortcuts for navigation

## Requirements

- C++17 compatible compiler (GCC 8+ or Clang 5+)
- CMake 3.10 or higher
- ncurses library
- Builds on Linux and FreeBSD

## Building

```bash
# Install ncurses development package (if not already installed)
sudo apt-get install libncurses5-dev  # For Debian/Ubuntu
# or
sudo yum install ncurses-devel        # For CentOS/RHEL/Fedora

# Create a build directory
mkdir build
cd build

# Configure and build
cmake ..
make

# Optionally install
sudo make install
```

## Usage

Simply run the application:

```bash
ctv
```

### Keyboard Controls

- **Tab**: Switch focus between file browser (left) and file viewer (right) panels
- **Up/Down Arrow Keys**: 
  - When file browser has focus: Navigate through files and directories
  - When file viewer has focus: Scroll text up and down
- **Enter**: Open selected directory or view selected file
- **Page Up/Page Down**: Scroll through file contents
- **q**: Quit the application

## License

MIT

See the LICENSE file for details.
