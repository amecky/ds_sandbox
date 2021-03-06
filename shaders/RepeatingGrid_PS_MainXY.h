#if 0
//
// Generated by Microsoft (R) D3D Shader Disassembler
//
//
//   fxc /nologo RepeatingGrid.hlsl /Tps_4_0 /Zi /Zpc /Qstrip_reflect
//    /Qstrip_debug /EPS_MainXY /FhRepeatingGrid_PS_MainXY.h
//    /VnRepeatingGrid_PS_MainXY
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// SV_POSITION              0   xyzw        0      POS  float       
// NORMAL                   0   xyz         1     NONE  float   xy  
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// SV_TARGET                0   xyzw        0   TARGET  float   xyzw
//
ps_4_0
dcl_constantbuffer cb0[1], immediateIndexed
dcl_input_ps linear v1.xy
dcl_output o0.xyzw
dcl_temps 2
add r0.xy, v1.xyxx, l(-0.500000, -0.500000, 0.000000, 0.000000)
frc r0.xy, r0.xyxx
add r0.xy, r0.xyxx, l(-0.500000, -0.500000, 0.000000, 0.000000)
deriv_rtx r0.zw, v1.xxxy
deriv_rty r1.xy, v1.xyxx
add r0.zw, |r0.zzzw|, |r1.xxxy|
div r0.xy, |r0.xyxx|, r0.zwzz
min r0.x, r0.y, r0.x
min r0.x, r0.x, l(1.000000)
mad r0.x, -r0.x, cb0[0].y, l(1.000000)
mul o0.xyz, r0.xxxx, cb0[0].xxxx
mov o0.w, l(1.000000)
ret 
// Approximately 0 instruction slots used
#endif

const BYTE RepeatingGrid_PS_MainXY[] =
{
     68,  88,  66,  67, 253,   3, 
    101, 100, 185,  73,  95,  18, 
    176,   8,  35, 176, 201, 237, 
    165, 225,   1,   0,   0,   0, 
     96,   2,   0,   0,   3,   0, 
      0,   0,  44,   0,   0,   0, 
    128,   0,   0,   0, 180,   0, 
      0,   0,  73,  83,  71,  78, 
     76,   0,   0,   0,   2,   0, 
      0,   0,   8,   0,   0,   0, 
     56,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     68,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,   7,   3,   0,   0, 
     83,  86,  95,  80,  79,  83, 
     73,  84,  73,  79,  78,   0, 
     78,  79,  82,  77,  65,  76, 
      0, 171,  79,  83,  71,  78, 
     44,   0,   0,   0,   1,   0, 
      0,   0,   8,   0,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     83,  86,  95,  84,  65,  82, 
     71,  69,  84,   0, 171, 171, 
     83,  72,  68,  82, 164,   1, 
      0,   0,  64,   0,   0,   0, 
    105,   0,   0,   0,  89,   0, 
      0,   4,  70, 142,  32,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  98,  16,   0,   3, 
     50,  16,  16,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   0,   0, 
      0,   0, 104,   0,   0,   2, 
      2,   0,   0,   0,   0,   0, 
      0,  10,  50,   0,  16,   0, 
      0,   0,   0,   0,  70,  16, 
     16,   0,   1,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0, 191,   0,   0,   0, 191, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  26,   0,   0,   5, 
     50,   0,  16,   0,   0,   0, 
      0,   0,  70,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
      0,  10,  50,   0,  16,   0, 
      0,   0,   0,   0,  70,   0, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0, 191,   0,   0,   0, 191, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  11,   0,   0,   5, 
    194,   0,  16,   0,   0,   0, 
      0,   0,   6,  20,  16,   0, 
      1,   0,   0,   0,  12,   0, 
      0,   5,  50,   0,  16,   0, 
      1,   0,   0,   0,  70,  16, 
     16,   0,   1,   0,   0,   0, 
      0,   0,   0,   9, 194,   0, 
     16,   0,   0,   0,   0,   0, 
    166,  14,  16, 128, 129,   0, 
      0,   0,   0,   0,   0,   0, 
      6,   4,  16, 128, 129,   0, 
      0,   0,   1,   0,   0,   0, 
     14,   0,   0,   8,  50,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   0,  16, 128, 129,   0, 
      0,   0,   0,   0,   0,   0, 
    230,  10,  16,   0,   0,   0, 
      0,   0,  51,   0,   0,   7, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     51,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  50,   0, 
      0,  11,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,  26, 128, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     56,   0,   0,   8, 114,  32, 
     16,   0,   0,   0,   0,   0, 
      6,   0,  16,   0,   0,   0, 
      0,   0,   6, 128,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
    130,  32,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  62,   0, 
      0,   1
};
