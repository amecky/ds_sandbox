#if 0
//
// Generated by Microsoft (R) D3D Shader Disassembler
//
//
//   fxc /nologo Shadow.hlsl /Tps_4_0 /Zi /Zpc /Qstrip_reflect /Qstrip_debug
//    /EPS_Main /FhShadow_PS_Main.h /VnShadow_PS_Main
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// SV_POSITION              0   xyzw        0      POS  float       
// COLOR                    0   xyzw        1     NONE  float   xyzw
// NORMAL                   0   xyz         2     NONE  float   xyz 
// TEXCOORD                 1   xyzw        3     NONE  float   xy w
// TEXCOORD                 2   xyz         4     NONE  float   xyz 
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// SV_TARGET                0   xyzw        0   TARGET  float   xyzw
//
ps_4_0
dcl_constantbuffer cb0[2], immediateIndexed
dcl_sampler s0, mode_default
dcl_resource_texture2d (float,float,float,float) t0
dcl_input_ps linear v1.xyzw
dcl_input_ps linear v2.xyz
dcl_input_ps linear v3.xyw
dcl_input_ps linear v4.xyz
dcl_output o0.xyzw
dcl_temps 4
div r0.xyzw, v3.xyxy, v3.wwww
add r1.xyzw, r0.zwzw, l(-0.000977, -0.000977, 0.000000, -0.000977)
sample r2.xyzw, r1.xyxx, t0.xyzw, s0
sample r1.xyzw, r1.zwzz, t0.xyzw, s0
add r1.x, r1.x, r2.x
add r2.xyzw, r0.zwzw, l(0.000977, -0.000977, -0.000977, 0.000000)
sample r3.xyzw, r2.xyxx, t0.xyzw, s0
sample r2.xyzw, r2.zwzz, t0.xyzw, s0
add r1.x, r1.x, r3.x
add r1.x, r2.x, r1.x
sample r2.xyzw, r0.zwzz, t0.xyzw, s0
add r1.x, r1.x, r2.x
add r2.xyzw, r0.zwzw, l(0.000977, 0.000000, -0.000977, 0.000977)
add r0.xyzw, r0.xyzw, l(0.000000, 0.000977, 0.000977, 0.000977)
sample r3.xyzw, r2.xyxx, t0.xyzw, s0
sample r2.xyzw, r2.zwzz, t0.xyzw, s0
add r1.x, r1.x, r3.x
add r1.x, r2.x, r1.x
sample r2.xyzw, r0.xyxx, t0.xyzw, s0
sample r0.xyzw, r0.zwzz, t0.xyzw, s0
add r0.y, r1.x, r2.x
add r0.x, r0.x, r0.y
mul r0.x, r0.x, l(0.111111)
dp3_sat r0.y, v2.xyzx, v4.xyzx
mul r1.xyzw, r0.yyyy, cb0[1].xyzw
lt r0.y, l(0.000000), r0.y
mad_sat r1.xyzw, r1.xyzw, r0.xxxx, cb0[0].xyzw
movc r0.xyzw, r0.yyyy, r1.xyzw, cb0[0].xyzw
mul o0.xyzw, r0.xyzw, v1.xyzw
ret 
// Approximately 0 instruction slots used
#endif

const BYTE Shadow_PS_Main[] =
{
     68,  88,  66,  67,  25, 150, 
    177, 195,  92,  75,  48, 246, 
    112, 185, 189, 111,  24, 207, 
    156, 249,   1,   0,   0,   0, 
     80,   5,   0,   0,   3,   0, 
      0,   0,  44,   0,   0,   0, 
    216,   0,   0,   0,  12,   1, 
      0,   0,  73,  83,  71,  78, 
    164,   0,   0,   0,   5,   0, 
      0,   0,   8,   0,   0,   0, 
    128,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
    140,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  15,  15,   0,   0, 
    146,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,   7,   7,   0,   0, 
    153,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   3,   0, 
      0,   0,  15,  11,   0,   0, 
    153,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   4,   0, 
      0,   0,   7,   7,   0,   0, 
     83,  86,  95,  80,  79,  83, 
     73,  84,  73,  79,  78,   0, 
     67,  79,  76,  79,  82,   0, 
     78,  79,  82,  77,  65,  76, 
      0,  84,  69,  88,  67,  79, 
     79,  82,  68,   0, 171, 171, 
     79,  83,  71,  78,  44,   0, 
      0,   0,   1,   0,   0,   0, 
      8,   0,   0,   0,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  83,  86, 
     95,  84,  65,  82,  71,  69, 
     84,   0, 171, 171,  83,  72, 
     68,  82,  60,   4,   0,   0, 
     64,   0,   0,   0,  15,   1, 
      0,   0,  89,   0,   0,   4, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
     90,   0,   0,   3,   0,  96, 
     16,   0,   0,   0,   0,   0, 
     88,  24,   0,   4,   0, 112, 
     16,   0,   0,   0,   0,   0, 
     85,  85,   0,   0,  98,  16, 
      0,   3, 242,  16,  16,   0, 
      1,   0,   0,   0,  98,  16, 
      0,   3, 114,  16,  16,   0, 
      2,   0,   0,   0,  98,  16, 
      0,   3, 178,  16,  16,   0, 
      3,   0,   0,   0,  98,  16, 
      0,   3, 114,  16,  16,   0, 
      4,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      0,   0,   0,   0, 104,   0, 
      0,   2,   4,   0,   0,   0, 
     14,   0,   0,   7, 242,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  20,  16,   0,   3,   0, 
      0,   0, 246,  31,  16,   0, 
      3,   0,   0,   0,   0,   0, 
      0,  10, 242,   0,  16,   0, 
      1,   0,   0,   0, 230,  14, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
    128, 186,   0,   0, 128, 186, 
      0,   0,   0,   0,   0,   0, 
    128, 186,  69,   0,   0,   9, 
    242,   0,  16,   0,   2,   0, 
      0,   0,  70,   0,  16,   0, 
      1,   0,   0,   0,  70, 126, 
     16,   0,   0,   0,   0,   0, 
      0,  96,  16,   0,   0,   0, 
      0,   0,  69,   0,   0,   9, 
    242,   0,  16,   0,   1,   0, 
      0,   0, 230,  10,  16,   0, 
      1,   0,   0,   0,  70, 126, 
     16,   0,   0,   0,   0,   0, 
      0,  96,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   7, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   2,   0,   0,   0, 
      0,   0,   0,  10, 242,   0, 
     16,   0,   2,   0,   0,   0, 
    230,  14,  16,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0, 128,  58,   0,   0, 
    128, 186,   0,   0, 128, 186, 
      0,   0,   0,   0,  69,   0, 
      0,   9, 242,   0,  16,   0, 
      3,   0,   0,   0,  70,   0, 
     16,   0,   2,   0,   0,   0, 
     70, 126,  16,   0,   0,   0, 
      0,   0,   0,  96,  16,   0, 
      0,   0,   0,   0,  69,   0, 
      0,   9, 242,   0,  16,   0, 
      2,   0,   0,   0, 230,  10, 
     16,   0,   2,   0,   0,   0, 
     70, 126,  16,   0,   0,   0, 
      0,   0,   0,  96,  16,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   7,  18,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   3,   0, 
      0,   0,   0,   0,   0,   7, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      2,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
     69,   0,   0,   9, 242,   0, 
     16,   0,   2,   0,   0,   0, 
    230,  10,  16,   0,   0,   0, 
      0,   0,  70, 126,  16,   0, 
      0,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
      0,   0,   0,   7,  18,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      2,   0,   0,   0,   0,   0, 
      0,  10, 242,   0,  16,   0, 
      2,   0,   0,   0, 230,  14, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
    128,  58,   0,   0,   0,   0, 
      0,   0, 128, 186,   0,   0, 
    128,  58,   0,   0,   0,  10, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 128,  58,   0,   0, 
    128,  58,   0,   0, 128,  58, 
     69,   0,   0,   9, 242,   0, 
     16,   0,   3,   0,   0,   0, 
     70,   0,  16,   0,   2,   0, 
      0,   0,  70, 126,  16,   0, 
      0,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
     69,   0,   0,   9, 242,   0, 
     16,   0,   2,   0,   0,   0, 
    230,  10,  16,   0,   2,   0, 
      0,   0,  70, 126,  16,   0, 
      0,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
      0,   0,   0,   7,  18,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   7,  18,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   2,   0,   0,   0, 
     10,   0,  16,   0,   1,   0, 
      0,   0,  69,   0,   0,   9, 
    242,   0,  16,   0,   2,   0, 
      0,   0,  70,   0,  16,   0, 
      0,   0,   0,   0,  70, 126, 
     16,   0,   0,   0,   0,   0, 
      0,  96,  16,   0,   0,   0, 
      0,   0,  69,   0,   0,   9, 
    242,   0,  16,   0,   0,   0, 
      0,   0, 230,  10,  16,   0, 
      0,   0,   0,   0,  70, 126, 
     16,   0,   0,   0,   0,   0, 
      0,  96,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   7, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   2,   0,   0,   0, 
      0,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,  57, 142, 
    227,  61,  16,  32,   0,   7, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  70,  18,  16,   0, 
      2,   0,   0,   0,  70,  18, 
     16,   0,   4,   0,   0,   0, 
     56,   0,   0,   8, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     86,   5,  16,   0,   0,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  49,   0,   0,   7, 
     34,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     50,  32,   0,  10, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,   6,   0,  16,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  55,   0, 
      0,  10, 242,   0,  16,   0, 
      0,   0,   0,   0,  86,   5, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  56,   0,   0,   7, 
    242,  32,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  70,  30, 
     16,   0,   1,   0,   0,   0, 
     62,   0,   0,   1
};
