[简体中文](README.md)

# mediapipe-livelink

Face and motion capture using mediapipe, send data to Unreal Engine with livelink

## TODO

- [x] Face Capture

    Calculation using [3D](https://google.github.io/mediapipe/solutions/face_mesh.html#face-transform-module) coordinates.
    
    Learn from [MeFaMo](https://github.com/JimWest/MeFaMo) to calculate the BlendShape weight.

- [ ] It is not an ideal solution to calculate the BlendShape by mathematically setting the weight manually. A better solution is to build a model to return the output of the mediapipe (or the output of other facial recognition models) to the BlendShape weight. At present, there is no character picture and corresponding BlendShape data set.

- [ ] Motion capture

## LICENSE

- [asio](THIRD_LICENSE/asio/LICENSE)

- [nlohmann](THIRD_LICENSE/nlohmann/LICENSE)

- [MeFaMo](THIRD_LICENSE/MeFaMo/LICENSE)
