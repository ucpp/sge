# SGE - Simple Game Engine  

SGE (Simple Game Engine) is a learning project aimed at building a minimalistic game engine using **DirectX 12**. This project is structured as a roadmap consisting several tasks, designed to teach and implement various modern rendering techniques and systems.  

## Roadmap  
---
<details>
  <summary><strong>✔ Initialize D3D12 context</strong>: Set up the DirectX 12 framework.</summary>
 
  ![First Triangle](https://github.com/ucpp/sge/blob/master/docs/first_triangle.png)

  [Last commit](https://github.com/ucpp/sge/commit/13da25b1de4a97eeb2f3149d05a9619258020e4b)
</details>
<details>
  <summary><strong>✔ Process input and camera control</strong>: Implement user input and basic camera movement.</summary>
 
  ![First Triangle](https://github.com/ucpp/sge/blob/master/docs/camera_and_input.gif)

  [Last commit](https://github.com/ucpp/sge/commit/ba4c22781d8f4f9242fe2f4917e1cb136106b02b)
</details>

---

- [ ] **Load simple models without animation**: Import static models such as teapots, cars, and tanks.  
- [ ] **Load mip-mapped textures**: Support diffuse, normal, specular, and gloss maps.  
- [ ] **Phong lighting**: Calculate lighting from direct and point lights in the pixel shader.  
- [ ] **Soft shadows**: Implement soft shadows using techniques like SM+PCF, VSM, or ESM.  
- [ ] **Dynamic environment reflections**: Render cube maps for dynamic reflections.  
- [ ] **Fog effect**: Add fog either in the pixel shader or as a post-process effect.  
- [ ] **Bloom effect**: Implement a post-process bloom effect.  
- [ ] **Load skinned mesh animations**: Import animated models and process on the GPU.  
- [ ] **Animation blending**: Combine multiple animations (e.g., running and shooting).  
- [ ] **GPU compressed textures**: Use compressed texture formats for better performance.  
- [ ] **Deferred lighting**: Implement direct, capsule, point, and spot lights with a deferred pipeline.  
- [ ] **Cook-Torrance BRDF and Image-Based Lighting (IBL)**: Add realistic PBR lighting.  
- [ ] **HDR pipeline**: Add auto exposure and tone mapping for high dynamic range rendering.  
- [ ] **MSAA**: Support multi-sample anti-aliasing.  
- [ ] **SSAO**: Implement screen-space ambient occlusion.  
- [ ] **SSLR**: Add screen-space local reflections.
- [ ] **Particle system**: Create a system that supports 10,000+ particles.  

## Project Goals  

The SGE project is designed to:  
- Learn DirectX 12 fundamentals and advanced features.  
- Implement modern rendering techniques used in game engines.  
- Build a foundation for further exploration of game engine development.  
 
## Contribution
This repository is a personal learning project, but feel free to report issues or suggest improvements!

## License
This project is licensed under the MIT License. See the LICENSE file for details.
