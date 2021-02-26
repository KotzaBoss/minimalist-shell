# Minimalist Shell

## Build

The implementation uses [CMake](https://cmake.org/) as a build tool.
To build this project simply run the following commands
```shell script
cmake -B build
make -C build
```
The resulting directory will be at the root of the project, so simply run
```
./minimalist-shell
```

## Usage

All requested features have been attempted so expect to be able to 
launch a command in the background with `&`, pipe commands with `|` 
and use user expansion `~`. Error codes for the command can be accessed by echoing `$?`.
```shell script
>>> cat doesnt_exist.txt
cat: doesnt_exist.txt: No such file or directory
>>> echo $?
1
```
