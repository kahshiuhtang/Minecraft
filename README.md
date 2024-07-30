# Minecraft Clone

## Dependencies

1. OpenGL
2. CMake
3. g++17
4. Bullet Physics

## Install

Linux:

```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
./vcpkg install bullet3
sudo apt-get install libbullet-dev

```

## To Run

```bash
cd build
cmake --build /home/kahshiuh/Documents/projects/Minecraft/build --config Debug --target all --
./minecraft
```

## Todo:

[X] Render blocks
[X] Use mouse and keyboard to pan around and move.
[X] Generate the world automatically
[X] Create a world object that keeps track of chunks
[] Figure out how to keep track of position
[] Render a mesh instead of block by block
[] Stop phasing through blocks
