# Cache Simulator
This sub project finished the lab2 task.

## Build
Make sure `CMake` is at a latest version.
```shell
$ cmake -version
cmake version 3.6.0

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```

Current source level is on `C++14`. A most recent C++ compiler is needed to compile. My current compiler is:
```shell
$ clang++ --version
Apple LLVM version 7.3.0 (clang-703.0.31)
Target: x86_64-apple-darwin15.6.0
Thread model: posix
InstalledDir: /Library/Developer/CommandLineTools/usr/bin
```

Let's assume we only want to build this "lab2" project. A quick example of building such CMake project is like this:

![](https://www.dropbox.com/s/upvlqz9mk7byp1i/Screenshot%202016-11-25%2016.50.25.png?raw=1)

The commands used above is:
```shell
mkdir build
cd build
cmake ..
```

Now we can make the project:
![](https://www.dropbox.com/s/w29uku0yob77udc/Screenshot%202016-11-25%2016.55.00.png?raw=1)

Use this command to execute the simulator:
```shell
./lab2/cache_sim cacheconfig.txt trace.txt
```
The output is at `trace.txt.out`
