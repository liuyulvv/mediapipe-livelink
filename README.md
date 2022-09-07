[English](README_EN.md)

# mediapipe-livelink

使用mediapipe进行面部和动作捕捉，通过livelink传输数据至Unreal Engine

## TODO

- [x] 面部捕捉

    使用[3D坐标](https://google.github.io/mediapipe/solutions/face_mesh.html#face-transform-module)计算
    
    借鉴[MeFaMo](https://github.com/JimWest/MeFaMo)计算BlendShape权重

- [ ] 通过数学人工设定权重计算BlendShape并不是一个理想的方案，更好的方案是建立模型将mediapipe的输出(或其他面部识别模型的输出)回归成BlendShape权重，目前并没有找到人物图片和对应BlendShape的数据集

- [ ] 动作捕捉

## 开源许可

- [asio](THIRD_LICENSE/asio/LICENSE)

- [nlohmann](THIRD_LICENSE/nlohmann/LICENSE)

- [MeFaMo](THIRD_LICENSE/MeFaMo/LICENSE)
