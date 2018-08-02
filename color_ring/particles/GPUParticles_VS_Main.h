#if 0
//
// Generated by Microsoft (R) D3D Shader Disassembler
//
//
//   fxc /nologo GPUParticles.hlsl /Tvs_4_0 /Zi /Zpc /Qstrip_reflect
//    /Qstrip_debug /EVS_Main /FhGPUParticles_VS_Main.h
//    /VnGPUParticles_VS_Main
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// SV_VERTEXID              0   x           0   VERTID   uint   x   
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// SV_POSITION              0   xyzw        0      POS  float   xyzw
// TEXCOORD                 0   xy          1     NONE  float   xy  
// COLOR                    0   xyzw        2     NONE  float   xyzw
//
vs_4_0
dcl_globalFlags refactoringAllowed | enableRawAndStructuredBuffers
dcl_constantbuffer cb0[5], immediateIndexed
dcl_resource_structured t1, 96 
dcl_input_sgv v0.x, vertex_id
dcl_output_siv o0.xyzw, position
dcl_output o1.xy
dcl_output o2.xyzw
dcl_temps 6
dcl_indexableTemp x0[4], 4
ushr r0.x, v0.x, l(2)
ld_structured r1.xyzw, r0.x, l(80), t1.xyzw
div r2.xyzw, r1.xyzw, cb0[0].zwzw
mov x0[0].xy, r2.xyxx
mov r3.zw, r2.yyyx
add r3.xy, r2.zwzz, r2.xyxx
mov x0[1].xy, r3.xzxx
mov x0[2].xy, r3.wyww
mov x0[3].xy, r3.xyxx
and r0.yzw, v0.xxxx, l(0, 3, 1, 2)
movc r0.zw, r0.zzzw, l(0,0,0.500000,-0.500000), l(0,0,-0.500000,0.500000)
mov o1.xy, x0[r0.y + 0].xyxx
ld_structured r2.xyz, r0.x, l(8), t1.xyzx
ld_structured r3.xyzw, r0.x, l(20), t1.xyzw
mul r1.xy, r2.xyxx, r3.yyyy
mad r0.yz, r0.zzwz, r1.zzwz, r1.xxyx
mul r0.w, r3.y, r3.y
mov r2.w, r3.x
mad r0.yz, r0.wwww, r2.zzwz, r0.yyzy
ld_structured r1.xyzw, r0.x, l(36), t1.yzwx
mul r0.z, r0.z, r1.w
mul r0.y, r3.w, r0.y
ld_structured r2.xyw, r0.x, l(68), t1.yzxx
mad r0.w, r2.y, r3.y, r2.x
sincos r3.x, r4.x, r0.w
mul r0.w, r0.z, r3.x
mul r0.z, r0.z, r4.x
mad r0.z, r3.x, r0.y, r0.z
mad r0.y, r4.x, r0.y, -r0.w
ld_structured r4.xy, r0.x, l(0), t1.xyxx
ld_structured r5.xyzw, r0.x, l(52), t1.xyzw
add r0.xw, r4.xxxy, -cb0[0].xxxy
add r4.xy, r0.xwxx, r0.yzyy
mov r4.z, l(1.000000)
dp3 o0.x, r4.xyzx, cb0[1].xywx
dp3 o0.y, r4.xyzx, cb0[2].xywx
dp3 o0.w, r4.xyzx, cb0[4].xywx
mov o0.z, l(1.000000)
mov r2.xyz, r5.yzwy
mov r1.w, r5.x
add r0.xyzw, -r1.xyzw, r2.xyzw
mad o2.xyzw, r3.zzzz, r0.xyzw, r1.xyzw
ret 
// Approximately 0 instruction slots used
#endif

const BYTE GPUParticles_VS_Main[] =
{
     68,  88,  66,  67, 220,  75, 
    193, 184,  35,  82, 149, 254, 
     30, 165,  68, 194,  50,  99, 
     52, 238,   1,   0,   0,   0, 
    124,   6,   0,   0,   4,   0, 
      0,   0,  48,   0,   0,   0, 
    100,   0,   0,   0, 216,   0, 
      0,   0, 108,   6,   0,   0, 
     73,  83,  71,  78,  44,   0, 
      0,   0,   1,   0,   0,   0, 
      8,   0,   0,   0,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      6,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   1,   0,   0,  83,  86, 
     95,  86,  69,  82,  84,  69, 
     88,  73,  68,   0,  79,  83, 
     71,  78, 108,   0,   0,   0, 
      3,   0,   0,   0,   8,   0, 
      0,   0,  80,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,   0, 
      0,   0,  92,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   3,  12, 
      0,   0, 101,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,  15,   0, 
      0,   0,  83,  86,  95,  80, 
     79,  83,  73,  84,  73,  79, 
     78,   0,  84,  69,  88,  67, 
     79,  79,  82,  68,   0,  67, 
     79,  76,  79,  82,   0, 171, 
     83,  72,  69,  88, 140,   5, 
      0,   0,  64,   0,   1,   0, 
     99,   1,   0,   0, 106,  72, 
      0,   1,  89,   0,   0,   4, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
    162,   0,   0,   4,   0, 112, 
     16,   0,   1,   0,   0,   0, 
     96,   0,   0,   0,  96,   0, 
      0,   4,  18,  16,  16,   0, 
      0,   0,   0,   0,   6,   0, 
      0,   0, 103,   0,   0,   4, 
    242,  32,  16,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
    101,   0,   0,   3,  50,  32, 
     16,   0,   1,   0,   0,   0, 
    101,   0,   0,   3, 242,  32, 
     16,   0,   2,   0,   0,   0, 
    104,   0,   0,   2,   6,   0, 
      0,   0, 105,   0,   0,   4, 
      0,   0,   0,   0,   4,   0, 
      0,   0,   4,   0,   0,   0, 
     85,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,  16,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      2,   0,   0,   0, 167,   0, 
      0,   9, 242,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,  80,   0, 
      0,   0,  70, 126,  16,   0, 
      1,   0,   0,   0,  14,   0, 
      0,   8, 242,   0,  16,   0, 
      2,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
    230, 142,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     54,   0,   0,   6,  50,  48, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  70,   0, 
     16,   0,   2,   0,   0,   0, 
     54,   0,   0,   5, 194,   0, 
     16,   0,   3,   0,   0,   0, 
     86,   1,  16,   0,   2,   0, 
      0,   0,   0,   0,   0,   7, 
     50,   0,  16,   0,   3,   0, 
      0,   0, 230,  10,  16,   0, 
      2,   0,   0,   0,  70,   0, 
     16,   0,   2,   0,   0,   0, 
     54,   0,   0,   6,  50,  48, 
     32,   0,   0,   0,   0,   0, 
      1,   0,   0,   0, 134,   0, 
     16,   0,   3,   0,   0,   0, 
     54,   0,   0,   6,  50,  48, 
     32,   0,   0,   0,   0,   0, 
      2,   0,   0,   0, 118,  15, 
     16,   0,   3,   0,   0,   0, 
     54,   0,   0,   6,  50,  48, 
     32,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,  70,   0, 
     16,   0,   3,   0,   0,   0, 
      1,   0,   0,  10, 226,   0, 
     16,   0,   0,   0,   0,   0, 
      6,  16,  16,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      2,   0,   0,   0,  55,   0, 
      0,  15, 194,   0,  16,   0, 
      0,   0,   0,   0, 166,  14, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,  63,   0,   0, 
      0, 191,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0, 191, 
      0,   0,   0,  63,  54,   0, 
      0,   7,  50,  32,  16,   0, 
      1,   0,   0,   0,  70,  48, 
     32,   4,   0,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0, 167,   0,   0,   9, 
    114,   0,  16,   0,   2,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   8,   0,   0,   0, 
     70, 114,  16,   0,   1,   0, 
      0,   0, 167,   0,   0,   9, 
    242,   0,  16,   0,   3,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,  20,   0,   0,   0, 
     70, 126,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   7, 
     50,   0,  16,   0,   1,   0, 
      0,   0,  70,   0,  16,   0, 
      2,   0,   0,   0,  86,   5, 
     16,   0,   3,   0,   0,   0, 
     50,   0,   0,   9,  98,   0, 
     16,   0,   0,   0,   0,   0, 
    166,  11,  16,   0,   0,   0, 
      0,   0, 166,  11,  16,   0, 
      1,   0,   0,   0,   6,   1, 
     16,   0,   1,   0,   0,   0, 
     56,   0,   0,   7, 130,   0, 
     16,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   3,   0, 
      0,   0,  26,   0,  16,   0, 
      3,   0,   0,   0,  54,   0, 
      0,   5, 130,   0,  16,   0, 
      2,   0,   0,   0,  10,   0, 
     16,   0,   3,   0,   0,   0, 
     50,   0,   0,   9,  98,   0, 
     16,   0,   0,   0,   0,   0, 
    246,  15,  16,   0,   0,   0, 
      0,   0, 166,  11,  16,   0, 
      2,   0,   0,   0,  86,   6, 
     16,   0,   0,   0,   0,   0, 
    167,   0,   0,   9, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
     36,   0,   0,   0, 150, 115, 
     16,   0,   1,   0,   0,   0, 
     56,   0,   0,   7,  66,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  58,   0,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   7,  34,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   3,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0, 167,   0,   0,   9, 
    178,   0,  16,   0,   2,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,  68,   0,   0,   0, 
    150, 112,  16,   0,   1,   0, 
      0,   0,  50,   0,   0,   9, 
    130,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      2,   0,   0,   0,  26,   0, 
     16,   0,   3,   0,   0,   0, 
     10,   0,  16,   0,   2,   0, 
      0,   0,  77,   0,   0,   7, 
     18,   0,  16,   0,   3,   0, 
      0,   0,  18,   0,  16,   0, 
      4,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   7, 130,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      3,   0,   0,   0,  56,   0, 
      0,   7,  66,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   4,   0, 
      0,   0,  50,   0,   0,   9, 
     66,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      3,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,  10, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
    167,   0,   0,   9,  50,   0, 
     16,   0,   4,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  70, 112, 
     16,   0,   1,   0,   0,   0, 
    167,   0,   0,   9, 242,   0, 
     16,   0,   5,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
     52,   0,   0,   0,  70, 126, 
     16,   0,   1,   0,   0,   0, 
      0,   0,   0,   9, 146,   0, 
     16,   0,   0,   0,   0,   0, 
      6,   4,  16,   0,   4,   0, 
      0,   0,   6, 132,  32, 128, 
     65,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   7,  50,   0, 
     16,   0,   4,   0,   0,   0, 
    198,   0,  16,   0,   0,   0, 
      0,   0, 150,   5,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5,  66,   0,  16,   0, 
      4,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     16,   0,   0,   8,  18,  32, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   4,   0, 
      0,   0,  70, 131,  32,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  16,   0,   0,   8, 
     34,  32,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      4,   0,   0,   0,  70, 131, 
     32,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,  16,   0, 
      0,   8, 130,  32,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   4,   0,   0,   0, 
     70, 131,  32,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     54,   0,   0,   5,  66,  32, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  54,   0,   0,   5, 
    114,   0,  16,   0,   2,   0, 
      0,   0, 150,   7,  16,   0, 
      5,   0,   0,   0,  54,   0, 
      0,   5, 130,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   5,   0,   0,   0, 
      0,   0,   0,   8, 242,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16, 128,  65,   0, 
      0,   0,   1,   0,   0,   0, 
     70,  14,  16,   0,   2,   0, 
      0,   0,  50,   0,   0,   9, 
    242,  32,  16,   0,   2,   0, 
      0,   0, 166,  10,  16,   0, 
      3,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  62,   0,   0,   1, 
     83,  70,  73,  48,   8,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0
};
