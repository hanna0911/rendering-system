## 完成内容
- 算法：
  - [x] 光线投射Ray Casting（光源发出单光线与物体相交）：直接光Direct
  - [x] 光线追踪Ray Tracing（跟踪眼睛发出单光线递归）：直接光Direct、间接光Indirect（镜面反射、阴影）
  - [ ] 路径跟踪Path Tracing（蒙特卡洛算法实现多个随机光线递归）：（漫反射、ColorBleeding、软阴影、景深、运动模糊）
  - [ ] 光子映射Photon Mapping（焦散Caustics）
- 几何求交
- 透视相机模型
- Phong模型着色：
  - [x] 漫反射Diffuse、镜面反射Specular
  - [x] 环境光Ambient
- Mesh网格模型
- 其他：
  - 求交加速：包围盒、层次包围盒、kd-tree、octree、hash
  - 抗锯齿：超采样Super-Sampling、高斯滤波器Gaussian blur filtering
  - 景深：光圈、焦距、物距
  - 运动模糊
  - 软阴影（面光源采样）
  - 纹理贴图：UV纹理映射、凹凸贴图
  - 色散、焦散
  - 体积光

## 使用框架

- 本课程PA1所给框架
- MIT[6.837 Intro to Computer Graphics, Fall 2004](http://groups.csail.mit.edu/graphics/classes/6.837/F04/index.html)课程Assignment框架
- MIT[6.837 Computer Graphics, Fall 2012](https://ocw.mit.edu/courses/6-837-computer-graphics-fall-2012/pages/syllabus/)课程Assignment框架

```
.
├── CMakeLists.txt
├── Report.md
├── deps/vecmath/
├── include/
│   ├── camera.hpp
│   ├── group.hpp
│   ├── hit.hpp
│   ├── image.hpp
│   ├── light.hpp
│   ├── material.hpp
│   ├── mesh.hpp
│   ├── object3d.hpp
│   ├── plane.hpp
│   ├── ray.hpp
│   ├── renderer.hpp
│   ├── scene_parser.hpp
│   ├── sphere.hpp
│   ├── transform.hpp
│   ├── triangle.hpp
│   └── vec_utils.hpp
├── mesh/
├── output/
├── run_all.sh
├── src/
│   ├── image.cpp
│   ├── main.cpp
│   ├── mesh.cpp
│   ├── renderer.cpp
│   └── scene_parser.cpp
└── testcases/
```

## 光线跟踪Ray Tracing

有问题（会停住，无限递归？）