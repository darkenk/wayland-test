# Wayland-test
App for testing wayland communication

## Dependencies
* cmake 3.0.2
```
sudo add-apt-repository ppa:george-edison55/cmake-3.x
sudo apt-get update
sudo apt-get install cmake cmake-extra-modules
```
* wayland 1.7.0 and mesa 10.6
Follow the instructions from http://wayland.freedesktop.org/building.html

## Building
```
export PKG_CONFIG_PATH=$WLD/lib/pkgconfig/:$WLD/share/pkgconfig/
mkdir -p build && cd build
cmake ../ -DCMAKE_PREFIX_PATH=${WLD}
make
```
WLD is the path to wayland install directory.

## Running
```
export LD_LIBRARY_PATH=${WLD}/lib
```
