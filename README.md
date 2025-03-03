# SGE - Simple Game Engine  

![Preview](https://github.com/ucpp/sge/blob/master/docs/shadows.png)

SGE (Simple Game Engine) is a learning project aimed at building a minimalistic game engine using **DirectX 12**. This project is structured as a roadmap consisting several tasks, designed to teach and implement various modern rendering techniques and systems. 

This project is **actively under development**, and its purpose is to experiment and test various ideas in graphics engine development.  

### Important:  

- The project may contain **low-quality, messy, and suboptimal code**.  
- Errors, bugs, and missing or incorrect functionality may exist.  
- The code is intended for **personal use** and experimentation only.  

> **Please note**: This project is **not intended for production use** or for creating finished products.  

## Roadmap  
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
  <summary><strong>✔ 4. Load mip-mapped textures</strong>: Support diffuse, normal, specular, and gloss maps.</summary>
 
  ![Textures](https://github.com/ucpp/sge/blob/master/docs/test_mips.gif)

</details>
<details>
  <summary><strong>✔ 5. GPU compressed textures</strong>: Use compressed texture formats for better performance.</summary>
 
  ![Textures](https://github.com/ucpp/sge/blob/master/docs/gpu_compressed_textures.png)

  [Last commit](https://github.com/ucpp/sge/commit/7f94cee0e085e4e762407c829d87ead4bfd991c2)

</details>

<details>
  <summary><strong>✔ 6. Phong lighting</strong>: Calculate lighting from direct and point lights in the pixel shader.</summary>

  ![Phong lighting](https://github.com/ucpp/sge/blob/master/docs/phong.gif)

  [Last commit](https://github.com/ucpp/sge/commit/fff8aa416b0fb9948be084802f2dd173c7df1e0b)
 
</details>
<details>
  <summary><strong>✔ 7. MSAA</strong>: Support multi-sample anti-aliasing.</summary>

  ![MSAA](https://github.com/ucpp/sge/blob/master/docs/msaa.png)

  [Last commit](https://github.com/ucpp/sge/commit/f62c52b29d1cff6fb0d903e2ca63036b8a65547d)

</details>
<details>
  <summary><strong>✔ 8. Fog effect</strong>: Add fog either in the pixel shader or as a post-process effect.</summary>
 
  ![Fog](https://github.com/ucpp/sge/blob/master/docs/fog.gif)

  [Last commit](https://github.com/ucpp/sge/commit/e9dd4b4625f0a0aff2d495b8370214d21d88713a)

</details>
<details>
  <summary><strong>✔ 9. Deferred lighting</strong>: Implement direct, capsule, point, and spot lights with a deferred pipeline.</summary>

  ![Deferred](https://github.com/ucpp/sge/blob/master/docs/deferred.gif)

</details>
<details>
  <summary><strong>✔ 10. Cook-Torrance BRDF and Image-Based Lighting (IBL)</strong>: Add realistic PBR lighting.</summary>

  ![BRDF](https://github.com/ucpp/sge/blob/master/docs/point_lights.png)

</details>
<details>
  <summary><strong>✔ 11. HDR pipeline</strong>: Add auto exposure and tone mapping for high dynamic range rendering.</summary>

  ![BRDF](https://github.com/ucpp/sge/blob/master/docs/brdf.png)

</details>
<details>
  <summary><strong>✔ 12. Bloom effect</strong>: Implement a post-process bloom effect.</summary>

  ![Bloom](https://github.com/ucpp/sge/blob/master/docs/bloom.png)

</details>
 <details>
  <summary><strong>✔ 13. SSAO</strong>: Implement screen-space ambient occlusion.</summary>

  ![SSAO](https://github.com/ucpp/sge/blob/master/docs/ssao.png)

</details>
 <details>
  <summary><strong>✔ 14. Soft shadows</strong>: Implement soft shadows using techniques like SM+PCF, VSM, or ESM.</summary>

  ![Shadows](https://github.com/ucpp/sge/blob/master/docs/shadows.png)
  
</details>
 <details>
  <summary><strong>✔ 15. Load skinned mesh animations</strong>: Import animated models and process on the GPU.</summary>

  ![Animation](https://github.com/ucpp/sge/blob/master/docs/anim.gif)
  
</details>
 <details>
  <summary><strong>✔ 16. Animation blending</strong>: Combine multiple animations (e.g., running and shooting).</summary>

  ![Blending](https://github.com/ucpp/sge/blob/master/docs/blend_anim.gif)
  
</details>
</details>
 <details>
  <summary><strong>☐ 17. Dynamic environment reflections</strong>: Render cube maps for dynamic reflections (in progress).</summary>
   
</details>
</details>
 <details>
  <summary><strong>☐ 18. SSLR</strong>: Add screen-space local reflections (in progress).</summary>
   
</details> 
  
--- 
  
**19. Particle system**: Create a system that supports 10,000+ particles.  
**20. Transparency**: Implement transparency rendering with proper sorting and blending techniques.  
**21. TAA/FXAA**: Add temporal anti-aliasing (TAA) and fast approximate anti-aliasing (FXAA) as post-process options.

## Project Goals  

The SGE project is designed to:  
- Learn DirectX 12 fundamentals and advanced features.  
- Implement modern rendering techniques used in game engines.  
- Build a foundation for further exploration of game engine development.  

##  Controls
### Camera Controls
- **Movement**:
  - `W`: Move forward.
  - `S`: Move backward.
  - `A`: Move left.
  - `D`: Move right.
- **Zoom**:
  - Use the mouse wheel to adjust the field of view (FOV).
- **Rotation**:
  - Hold the right mouse button and move the mouse to rotate the camera.
### Editor Controls
- **Toggle Editor**:
  - `Q`: Toggle the editor UI on/off.
---
  
## Contribution
This repository is a personal learning project, but feel free to report issues or suggest improvements!

## License
This project is licensed under the MIT License. See the LICENSE file for details.

## Credits  
- Editor icons in this project were taken from Unreal Engine Editor.
- Backpack model by Berk Gedik, from [Sketchfab](https://sketchfab.com/3d-models/survival-guitar-backpack-low-poly-799f8c4511f84fab8c3f12887f7e6b36).
