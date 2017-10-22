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
// COLOR                    0   xyzw        1     NONE  float       
// NORMAL                   0   xyz         2     NONE  float       
// TEXCOORD                 1   xyzw        3     NONE  float     zw
// TEXCOORD                 2   xyz         4     NONE  float       
// TEXCOORD                 3   xyz         5     NONE  float       
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// SV_TARGET                0   xyzw        0   TARGET  float   xyzw
//
ps_4_0
dcl_input_ps linear v3.zw
dcl_output o0.xyzw
div o0.xyz, v3.zzzz, v3.wwww
mov o0.w, l(1.000000)
ret 
// Approximately 0 instruction slots used
#endif

const BYTE Shadow_PS_Main[] =
{
     68,  88,  66,  67, 113, 209, 
     11, 113, 210, 213,  17,   9, 
    187,  10, 164,  74, 154, 141, 
    164, 241,   1,   0,   0,   0, 
    128,   1,   0,   0,   3,   0, 
      0,   0,  44,   0,   0,   0, 
    240,   0,   0,   0,  36,   1, 
      0,   0,  73,  83,  71,  78, 
    188,   0,   0,   0,   6,   0, 
      0,   0,   8,   0,   0,   0, 
    152,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
    164,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  15,   0,   0,   0, 
    170,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,   7,   0,   0,   0, 
    177,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   3,   0, 
      0,   0,  15,  12,   0,   0, 
    177,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   4,   0, 
      0,   0,   7,   0,   0,   0, 
    177,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   5,   0, 
      0,   0,   7,   0,   0,   0, 
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
     68,  82,  84,   0,   0,   0, 
     64,   0,   0,   0,  21,   0, 
      0,   0,  98,  16,   0,   3, 
    194,  16,  16,   0,   3,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   0,   0, 
      0,   0,  14,   0,   0,   7, 
    114,  32,  16,   0,   0,   0, 
      0,   0, 166,  26,  16,   0, 
      3,   0,   0,   0, 246,  31, 
     16,   0,   3,   0,   0,   0, 
     54,   0,   0,   5, 130,  32, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  62,   0,   0,   1
};
