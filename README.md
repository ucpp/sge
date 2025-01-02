# SGE - Simple Game Engine  

SGE (Simple Game Engine) is a learning project aimed at building a minimalistic game engine using **DirectX 12**. This project is structured as a roadmap consisting several tasks, designed to teach and implement various modern rendering techniques and systems. 

This project is **actively under development**, and its purpose is to experiment and test various ideas in graphics engine development.  

### Important:  

- The project may contain **low-quality, messy, and suboptimal code**.  
- Errors, bugs, and missing or incorrect functionality may exist.  
- The code is intended for **personal use** and experimentation only.  

> **Please note**: This project is **not intended for production use** or for creating finished products.  

## Roadmap  
---
<details>
  <summary><strong>✔ 1. Initialize D3D12 context</strong>: Set up the DirectX 12 framework.</summary>
 
  ![First Triangle](https://github.com/ucpp/sge/blob/master/docs/first_triangle.png)

  [Last commit](https://github.com/ucpp/sge/commit/13da25b1de4a97eeb2f3149d05a9619258020e4b)
</details>
<details>
  <summary><strong>✔ 2. Process input and camera control</strong>: Implement user input and basic camera movement.</summary>
 
  ![Camera and Input](https://github.com/ucpp/sge/blob/master/docs/camera_and_input.gif)

  [Last commit](https://github.com/ucpp/sge/commit/ba4c22781d8f4f9242fe2f4917e1cb136106b02b)
</details>
<details>
  <summary><strong>✔ 3. Load simple models without animation</strong>: Import static models such as teapots, cars, and tanks.</summary>
 
  ![Models](https://github.com/ucpp/sge/blob/master/docs/load_model.gif)

  [Last commit](https://github.com/ucpp/sge/commit/97288dfe5bd48521b60c2f83586c5614c38b82e2)
</details>
<details>
  <summary><strong>✔ 4. Load mip-mapped textures</strong>: Support diffuse, normal, specular, and gloss maps (in progress).</summary>
 
  ![Textures](https://github.com/ucpp/sge/blob/master/docs/test_mips.gif)


</details>

---
    
**5. GPU compressed textures**: Use compressed texture formats for better performance.  
**6. Phong lighting**: Calculate lighting from direct and point lights in the pixel shader.  
**7. Fog effect**: Add fog either in the pixel shader or as a post-process effect.  
**8. Deferred lighting**: Implement direct, capsule, point, and spot lights with a deferred pipeline.  
**9. Cook-Torrance BRDF and Image-Based Lighting (IBL)**: Add realistic PBR lighting.  
**10. HDR pipeline**: Add auto exposure and tone mapping for high dynamic range rendering.  
**11. Bloom effect**: Implement a post-process bloom effect.  
**12. Dynamic environment reflections**: Render cube maps for dynamic reflections.  
**13. Soft shadows**: Implement soft shadows using techniques like SM+PCF, VSM, or ESM.  
**14. SSAO**: Implement screen-space ambient occlusion.  
**15. SSLR**: Add screen-space local reflections.  
**16. Load skinned mesh animations**: Import animated models and process on the GPU.  
**17. Animation blending**: Combine multiple animations (e.g., running and shooting).  
**18. MSAA**: Support multi-sample anti-aliasing.  
**19. Particle system**: Create a system that supports 10,000+ particles.  

## Project Goals  

The SGE project is designed to:  
- Learn DirectX 12 fundamentals and advanced features.  
- Implement modern rendering techniques used in game engines.  
- Build a foundation for further exploration of game engine development.  
 
## Contribution
This repository is a personal learning project, but feel free to report issues or suggest improvements!

## License
This project is licensed under the MIT License. See the LICENSE file for details.
