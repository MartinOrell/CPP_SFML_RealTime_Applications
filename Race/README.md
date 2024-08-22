These are the commands I use for writing build files, building and executing the project.\
Using another generator should be fine too.

### Generate build files
```
cmake -S . -B build -G "MinGW Makefiles"
```
### Build project
```
cmake --build build/
```
### execute
```
build/main.exe
```