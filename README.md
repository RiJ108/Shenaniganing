# Shenaniganing
Personal project exploring 2D and 3D rendering and various game mechanics.

Libraries used for rendering: 
GLFW3, 
glad, 
glm, 
freetype2

Algorithm used: 
perlin noise, 
marching cube

Crude implementation of a infinite navigation:
![Crude_infinite_nav](https://user-images.githubusercontent.com/58864879/172619610-29476ea9-52ac-4eb1-b1d3-2e9440655fdd.gif)
Need to move the mesh generation to a compute shader or at least a independent thread in the futur.

Marching cube implemantation without interpolation:
![MC_without_interpolation](https://user-images.githubusercontent.com/58864879/156930789-a6aa4cf1-76ef-4979-a76b-ef2ee70474a6.gif)

Marching cube implemantation with interpolation:
![MC_with_interpolation](https://user-images.githubusercontent.com/58864879/156930792-10aa8809-ad6c-493e-bdc2-7848d3a58db3.gif)
