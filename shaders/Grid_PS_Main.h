#if 0
//
// Generated by Microsoft (R) D3D Shader Disassembler
//
//
//   fxc /nologo Grid.hlsl /Tps_4_0 /Zi /Zpc /Qstrip_reflect /Qstrip_debug
//    /EPS_Main /FhGrid_PS_Main.h /VnGrid_PS_Main
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// SV_POSITION              0   xyzw        0      POS  float       
// COLOR                    0   xy          1     NONE  float   xy  
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// SV_TARGET                0   xyzw        0   TARGET  float   xyzw
//
ps_4_0
dcl_input_ps linear v1.xy
dcl_output o0.xyzw
dcl_temps 1
ge r0.xy, v1.xyxx, l(0.980000, 0.980000, 0.000000, 0.000000)
and r0.xy, r0.xyxx, l(0x3f800000, 0x3f800000, 0, 0)
dp2 r0.x, r0.xyxx, l(1.000000, 1.000000, 0.000000, 0.000000)
min r0.x, r0.x, l(1.000000)
mad o0.xyzw, r0.xxxx, l(0.400000, 0.400000, 0.400000, 0.000000), l(0.300000, 0.300000, 0.300000, 1.000000)
ret 
// Approximately 0 instruction slots used
#endif

const BYTE Grid_PS_Main[] =
{
     68,  88,  66,  67, 170, 250, 
    122, 100, 178, 139, 225,  49, 
      9, 107, 254,  54, 182,  82, 
    148,   8,   1,   0,   0,   0, 
    184,   1,   0,   0,   3,   0, 
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
      0,   0,   3,   3,   0,   0, 
     83,  86,  95,  80,  79,  83, 
     73,  84,  73,  79,  78,   0, 
     67,  79,  76,  79,  82,   0, 
    171, 171,  79,  83,  71,  78, 
     44,   0,   0,   0,   1,   0, 
      0,   0,   8,   0,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     83,  86,  95,  84,  65,  82, 
     71,  69,  84,   0, 171, 171, 
     83,  72,  68,  82, 252,   0, 
      0,   0,  64,   0,   0,   0, 
     63,   0,   0,   0,  98,  16, 
      0,   3,  50,  16,  16,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      0,   0,   0,   0, 104,   0, 
      0,   2,   1,   0,   0,   0, 
     29,   0,   0,  10,  50,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  16,  16,   0,   1,   0, 
      0,   0,   2,  64,   0,   0, 
     72, 225, 122,  63,  72, 225, 
    122,  63,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,  10,  50,   0,  16,   0, 
      0,   0,   0,   0,  70,   0, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
    128,  63,   0,   0, 128,  63, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,  10, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  70,   0,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     51,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  50,   0, 
      0,  15, 242,  32,  16,   0, 
      0,   0,   0,   0,   6,   0, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0, 205, 204, 
    204,  62, 205, 204, 204,  62, 
    205, 204, 204,  62,   0,   0, 
      0,   0,   2,  64,   0,   0, 
    154, 153, 153,  62, 154, 153, 
    153,  62, 154, 153, 153,  62, 
      0,   0, 128,  63,  62,   0, 
      0,   1
};