# PSL

PSL is a little project that jit-compiles a very simple language built around floats to optimized SIMD assembly code. It is made to be a shading language, but for now it's just an experiment.
This project is very simple and is just a playground around jit code generation to learn more about assembly, jit-compilation and code optimization.

Example:
```
// This shader converts a 3D direction to 2d coordinates 0.0-1.0

f32 calcU(f32 x, f32 z) 
{ 
    return atan2(z, x) * 0.1591 + 0.5; 
} 

// Shader entry point
main myFunc(f32 Nx, f32 Ny, f32 Nz, export f32 u, export f32 v) 
{ 
    u = calcU(Nx, Nz); 
    v = asin(Ny) * 0.3183 + 0.5;
}
```
