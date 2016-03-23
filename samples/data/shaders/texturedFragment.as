; SPIR-V
; Version: 1.0
; Generator: Khronos Glslang Reference Front End; 1
; Bound: 40
; Schema: 0
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main" %11 %20 %26
               OpExecutionMode %4 OriginUpperLeft
               OpSource GLSL 400
               OpSourceExtension "GL_ARB_separate_shader_objects"
               OpSourceExtension "GL_ARB_shading_language_420pack"
               OpName %4 "main"
               OpName %9 "color"
               OpName %11 "fColor"
               OpName %31 "diffuseTexture"
               OpName %28 "textureSampler"
               OpName %20 "fTexCoord"
               OpName %26 "fbColor"
               OpDecorate %11 Location 0
               OpDecorate %31 DescriptorSet 1
               OpDecorate %31 Binding 0
               OpDecorate %20 Location 1
               OpDecorate %28 DescriptorSet 2
               OpDecorate %28 Binding 0
               OpDecorate %26 Location 0
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeFloat 32
          %7 = OpTypeVector %6 4
          %8 = OpTypePointer Function %7
         %10 = OpTypePointer Input %7
         %11 = OpVariable %10 Input
         %13 = OpTypeImage %6 2D 0 0 0 1 Unknown
         %14 = OpTypeSampledImage %13
         %15 = OpTypePointer UniformConstant %13
         %29 = OpTypeSampler
         %30 = OpTypePointer UniformConstant %29
         %31 = OpVariable %15 UniformConstant
         %28 = OpVariable %30 UniformConstant
         %18 = OpTypeVector %6 2
         %19 = OpTypePointer Input %18
         %20 = OpVariable %19 Input
         %25 = OpTypePointer Output %7
         %26 = OpVariable %25 Output
          %4 = OpFunction %2 None %3
          %5 = OpLabel
          %9 = OpVariable %8 Function
         %12 = OpLoad %7 %11
               OpStore %9 %12
         %32 = OpLoad %13 %31
         %33 = OpLoad %30 %28
         %17 = OpSampledImage %14 %32 %33
         %21 = OpLoad %18 %20
         %22 = OpImageSampleImplicitLod %7 %17 %21
         %23 = OpLoad %7 %9
         %24 = OpFMul %7 %23 %22
               OpStore %9 %24
         %27 = OpLoad %7 %9
               OpStore %26 %27
               OpReturn
               OpFunctionEnd
