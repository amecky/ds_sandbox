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
dcl_constantbuffer cb0[10], immediateIndexed
dcl_resource_structured t1, 164 
dcl_input_sgv v0.x, vertex_id
dcl_output_siv o0.xyzw, position
dcl_output o1.xy
dcl_output o2.xyzw
dcl_temps 6
ushr r0.x, v0.x, l(2)
ld_structured r1.xyzw, r0.x, l(0), t1.xyzw
mov r2.x, r1.w
ld_structured r3.xyzw, r0.x, l(16), t1.zwxy
mov r2.yz, r3.zzwz
ld_structured r4.xyzw, r0.x, l(32), t1.wxyz
mad r0.yzw, r2.xxyz, r4.zzzz, r1.xxyz
mul r1.x, r4.z, r4.z
mov r3.z, r4.y
mad r0.yzw, r1.xxxx, r3.xxyz, r0.yyzw
add r1.xyz, -r0.wyzw, cb0[8].zxyz
dp3 r1.w, r1.xyzx, r1.xyzx
rsq r1.w, r1.w
mul r1.xyz, r1.wwww, r1.xyzx
mul r2.xyz, r1.xyzx, l(1.000000, 0.000000, 0.000000, 0.000000)
mad r2.xyz, r1.zxyz, l(0.000000, 0.000000, 1.000000, 0.000000), -r2.xyzx
mul r3.xyz, r1.xyzx, r2.yzxy
mad r1.xyz, r1.zxyz, r2.zxyz, -r3.xyzx
ld_structured r3.xy, r0.x, l(92), t1.xyxx
mad r1.w, r3.y, r4.z, r3.x
sincos r3.x, r5.x, r1.w
mul r3.yzw, r1.xxyz, r5.xxxx
mul r1.xyz, r1.xyzx, r3.xxxx
mad r1.xyz, r5.xxxx, r2.xyzx, -r1.xyzx
mad r2.xyz, r3.xxxx, r2.xyzx, r3.yzwy
ld_structured r3.xyzw, r0.x, l(48), t1.wxyz
mov r4.y, r3.y
mad_sat r4.xy, r3.zwzz, r4.zzzz, r4.xyxx
and r5.xy, v0.xxxx, l(1, 2, 0, 0)
movc r5.zw, r5.yyyx, l(0,0,0.500000,0.500000), l(0,0,-0.500000,-0.500000)
movc o1.xy, r5.xyxx, cb0[9].zwzz, cb0[9].xyxx
mul r4.xy, r4.xyxx, r5.zwzz
mul r2.xyz, r2.xyzx, r4.yyyy
mul r1.xyz, r1.xyzx, r4.xxxx
mul r2.xyz, r2.xyzx, l(0.500000, 0.500000, 0.500000, 0.000000)
mad r1.xyz, r1.xyzx, l(0.500000, 0.500000, 0.500000, 0.000000), -r2.xyzx
add r1.xyz, r0.yzwy, r1.xyzx
mov r1.w, l(1.000000)
dp4 o0.x, r1.xyzw, cb0[0].xyzw
dp4 o0.y, r1.xyzw, cb0[1].xyzw
dp4 o0.z, r1.xyzw, cb0[2].xyzw
dp4 o0.w, r1.xyzw, cb0[3].xyzw
ld_structured r1.xyz, r0.x, l(80), t1.xyzx
ld_structured r0.xyzw, r0.x, l(64), t1.xyzw
mov r1.w, r0.w
mov r3.yzw, r0.xxyz
add r0.xyzw, r1.wxyz, -r3.xyzw
mad o2.xyzw, r4.wwww, r0.xyzw, r3.xyzw
ret 
// Approximately 0 instruction slots used
#endif

const BYTE GPUParticles_VS_Main[] =
{
     68,  88,  66,  67, 105, 250, 
     65, 113,  44,  42,  41, 197, 
    218, 196, 172,   0,  89, 133, 
    109, 141,   1,   0,   0,   0, 
    128,   7,   0,   0,   4,   0, 
      0,   0,  48,   0,   0,   0, 
    100,   0,   0,   0, 216,   0, 
      0,   0, 112,   7,   0,   0, 
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
     83,  72,  69,  88, 144,   6, 
      0,   0,  64,   0,   1,   0, 
    164,   1,   0,   0, 106,  72, 
      0,   1,  89,   0,   0,   4, 
     70, 142,  32,   0,   0,   0, 
      0,   0,  10,   0,   0,   0, 
    162,   0,   0,   4,   0, 112, 
     16,   0,   1,   0,   0,   0, 
    164,   0,   0,   0,  96,   0, 
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
      0,   0,  85,   0,   0,   7, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,  16,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   2,   0,   0,   0, 
    167,   0,   0,   9, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  70, 126, 
     16,   0,   1,   0,   0,   0, 
     54,   0,   0,   5,  18,   0, 
     16,   0,   2,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0, 167,   0,   0,   9, 
    242,   0,  16,   0,   3,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,  16,   0,   0,   0, 
    230, 116,  16,   0,   1,   0, 
      0,   0,  54,   0,   0,   5, 
     98,   0,  16,   0,   2,   0, 
      0,   0, 166,  11,  16,   0, 
      3,   0,   0,   0, 167,   0, 
      0,   9, 242,   0,  16,   0, 
      4,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,  32,   0, 
      0,   0,  54, 121,  16,   0, 
      1,   0,   0,   0,  50,   0, 
      0,   9, 226,   0,  16,   0, 
      0,   0,   0,   0,   6,   9, 
     16,   0,   2,   0,   0,   0, 
    166,  10,  16,   0,   4,   0, 
      0,   0,   6,   9,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   7,  18,   0,  16,   0, 
      1,   0,   0,   0,  42,   0, 
     16,   0,   4,   0,   0,   0, 
     42,   0,  16,   0,   4,   0, 
      0,   0,  54,   0,   0,   5, 
     66,   0,  16,   0,   3,   0, 
      0,   0,  26,   0,  16,   0, 
      4,   0,   0,   0,  50,   0, 
      0,   9, 226,   0,  16,   0, 
      0,   0,   0,   0,   6,   0, 
     16,   0,   1,   0,   0,   0, 
      6,   9,  16,   0,   3,   0, 
      0,   0,  86,  14,  16,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   9, 114,   0,  16,   0, 
      1,   0,   0,   0, 118,  14, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,  38, 137, 
     32,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,  16,   0, 
      0,   7, 130,   0,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  68,   0,   0,   5, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      1,   0,   0,   0, 246,  15, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,  10, 
    114,   0,  16,   0,   2,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128,  63, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     50,   0,   0,  13, 114,   0, 
     16,   0,   2,   0,   0,   0, 
     38,   9,  16,   0,   1,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 128,  63, 
      0,   0,   0,   0,  70,   2, 
     16, 128,  65,   0,   0,   0, 
      2,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      3,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
    150,   4,  16,   0,   2,   0, 
      0,   0,  50,   0,   0,  10, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  38,   9,  16,   0, 
      1,   0,   0,   0,  38,   9, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16, 128,  65,   0, 
      0,   0,   3,   0,   0,   0, 
    167,   0,   0,   9,  50,   0, 
     16,   0,   3,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
     92,   0,   0,   0,  70, 112, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,   9, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     26,   0,  16,   0,   3,   0, 
      0,   0,  42,   0,  16,   0, 
      4,   0,   0,   0,  10,   0, 
     16,   0,   3,   0,   0,   0, 
     77,   0,   0,   7,  18,   0, 
     16,   0,   3,   0,   0,   0, 
     18,   0,  16,   0,   5,   0, 
      0,   0,  58,   0,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   7, 226,   0,  16,   0, 
      3,   0,   0,   0,   6,   9, 
     16,   0,   1,   0,   0,   0, 
      6,   0,  16,   0,   5,   0, 
      0,   0,  56,   0,   0,   7, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,   6,   0, 
     16,   0,   3,   0,   0,   0, 
     50,   0,   0,  10, 114,   0, 
     16,   0,   1,   0,   0,   0, 
      6,   0,  16,   0,   5,   0, 
      0,   0,  70,   2,  16,   0, 
      2,   0,   0,   0,  70,   2, 
     16, 128,  65,   0,   0,   0, 
      1,   0,   0,   0,  50,   0, 
      0,   9, 114,   0,  16,   0, 
      2,   0,   0,   0,   6,   0, 
     16,   0,   3,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0, 150,   7,  16,   0, 
      3,   0,   0,   0, 167,   0, 
      0,   9, 242,   0,  16,   0, 
      3,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,  48,   0, 
      0,   0,  54, 121,  16,   0, 
      1,   0,   0,   0,  54,   0, 
      0,   5,  34,   0,  16,   0, 
      4,   0,   0,   0,  26,   0, 
     16,   0,   3,   0,   0,   0, 
     50,  32,   0,   9,  50,   0, 
     16,   0,   4,   0,   0,   0, 
    230,  10,  16,   0,   3,   0, 
      0,   0, 166,  10,  16,   0, 
      4,   0,   0,   0,  70,   0, 
     16,   0,   4,   0,   0,   0, 
      1,   0,   0,  10,  50,   0, 
     16,   0,   5,   0,   0,   0, 
      6,  16,  16,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      1,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  55,   0, 
      0,  15, 194,   0,  16,   0, 
      5,   0,   0,   0,  86,   1, 
     16,   0,   5,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,  63,   0,   0, 
      0,  63,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0, 191, 
      0,   0,   0, 191,  55,   0, 
      0,  11,  50,  32,  16,   0, 
      1,   0,   0,   0,  70,   0, 
     16,   0,   5,   0,   0,   0, 
    230, 138,  32,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
     70, 128,  32,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
     56,   0,   0,   7,  50,   0, 
     16,   0,   4,   0,   0,   0, 
     70,   0,  16,   0,   4,   0, 
      0,   0, 230,  10,  16,   0, 
      5,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      2,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
     86,   5,  16,   0,   4,   0, 
      0,   0,  56,   0,   0,   7, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,   6,   0, 
     16,   0,   4,   0,   0,   0, 
     56,   0,   0,  10, 114,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,  63,   0,   0, 
      0,  63,   0,   0,   0,  63, 
      0,   0,   0,   0,  50,   0, 
      0,  13, 114,   0,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,  63,   0,   0,   0,  63, 
      0,   0,   0,  63,   0,   0, 
      0,   0,  70,   2,  16, 128, 
     65,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   7, 
    114,   0,  16,   0,   1,   0, 
      0,   0, 150,   7,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     54,   0,   0,   5, 130,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  17,   0,   0,   8, 
     18,  32,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  17,   0, 
      0,   8,  34,  32,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     17,   0,   0,   8,  66,  32, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  17,   0,   0,   8, 
    130,  32,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      3,   0,   0,   0, 167,   0, 
      0,   9, 114,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,  80,   0, 
      0,   0,  70, 114,  16,   0, 
      1,   0,   0,   0, 167,   0, 
      0,   9, 242,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,  64,   0, 
      0,   0,  70, 126,  16,   0, 
      1,   0,   0,   0,  54,   0, 
      0,   5, 130,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     54,   0,   0,   5, 226,   0, 
     16,   0,   3,   0,   0,   0, 
      6,   9,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   8, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  54,   9,  16,   0, 
      1,   0,   0,   0,  70,  14, 
     16, 128,  65,   0,   0,   0, 
      3,   0,   0,   0,  50,   0, 
      0,   9, 242,  32,  16,   0, 
      2,   0,   0,   0, 246,  15, 
     16,   0,   4,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      3,   0,   0,   0,  62,   0, 
      0,   1,  83,  70,  73,  48, 
      8,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0
};
