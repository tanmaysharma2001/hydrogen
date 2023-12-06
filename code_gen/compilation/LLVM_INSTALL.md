# LLVM Installation Instructions

Voc compiler uses llvm to generate native assembly code from AST, so in order to build the compiler, you are required to have LLVM installed.

This project is developed using LLVM 4.0.1, and this is !!STRONGLY advisable to build LLVM from source.

Below you will find instructions on how to do it.

## Prerequisites

To install LLVM from source you should have the following:

1. CMake version 3.4.3+
2. C++11 Compatible compiler
3. At least 4 GB of RAM
4. 8GB of free space on your disk

## Installation Process

**NOTE**: If you already have another version of LLVM installed on your computer, it may conflict with out-of-source installation and cause build to fail and other problems. You can use your system's LLVM distribution and headers, but notice that it is not guaranteed to work.

1. Download LLVM 4.0.1 Source Code: <http://releases.llvm.org/4.0.1/llvm-4.0.1.src.tar.xz>
2. Unpack the archive to desired place
3. Enter llvm source folder:

   ```cd llvm-4.0.1.src```
   
4. Create directory for all build files and enter it:

   ```mkdir build && cd build```

5. Run CMake to create build files:

   ```cmake ..```

6. Run make to build the project, this will take some time:

   ```make```

   **NOTE**: you can specify number of threads, on which to run build by appending -jX option, where X is number of threads. However it will increase RAM requirements significantly. (6 GB is not enough for 2 threads)you
7. After build is done, you can install libraries to the system, so they can be automatically accessible by build tools such as CMake
   ```sudo make install```

After everything is done, you can check if your installation was succesful by running llvm-config tool.
