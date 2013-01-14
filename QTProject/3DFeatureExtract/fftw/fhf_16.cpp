/*
 * Copyright (c) 1997-1999 Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* This file was automatically generated --- DO NOT EDIT */
/* Generated on Sun Nov  7 20:44:51 EST 1999 */
//#include "stdafx.h"
#include "fftw-int.h"
#include "fftw.h"

/* Generated by: ./genfft -magic-alignment-check -magic-twiddle-load-all -magic-variables 4 -magic-loopi -hc2hc-forward 16 */

/*
 * This function contains 298 FP additions, 130 FP multiplications,
 * (or, 244 additions, 76 multiplications, 54 fused multiply/add),
 * 51 stack variables, and 128 memory accesses
 */
static const fftw_real K277785116 = FFTW_KONST(+0.277785116509801112371415406974266437187468595);
static const fftw_real K415734806 = FFTW_KONST(+0.415734806151272618539394188808952878369280406);
static const fftw_real K490392640 = FFTW_KONST(+0.490392640201615224563091118067119518486966865);
static const fftw_real K097545161 = FFTW_KONST(+0.097545161008064133924142434238511120463845809);
static const fftw_real K1_414213562 = FFTW_KONST(+1.414213562373095048801688724209698078569671875);
static const fftw_real K2_000000000 = FFTW_KONST(+2.000000000000000000000000000000000000000000000);
static const fftw_real K707106781 = FFTW_KONST(+0.707106781186547524400844362104849039284835938);
static const fftw_real K923879532 = FFTW_KONST(+0.923879532511286756128183189396788286822416626);
static const fftw_real K382683432 = FFTW_KONST(+0.382683432365089771728459984030398866761344562);

/*
 * Generator Id's : 
 * $Id: exprdag.ml,v 1.41 1999/05/26 15:44:14 fftw Exp $
 * $Id: fft.ml,v 1.43 1999/05/17 19:44:18 fftw Exp $
 * $Id: to_c.ml,v 1.25 1999/10/26 21:41:32 stevenj Exp $
 */

void fftw_hc2hc_forward_16(fftw_real *A, const fftw_complex *W, int iostride, int m, int dist)
{
     int i;
     fftw_real *X;
     fftw_real *Y;
     X = A;
     Y = A + (16 * iostride);
     {
	  fftw_real tmp277;
	  fftw_real tmp280;
	  fftw_real tmp281;
	  fftw_real tmp309;
	  fftw_real tmp292;
	  fftw_real tmp307;
	  fftw_real tmp314;
	  fftw_real tmp322;
	  fftw_real tmp330;
	  fftw_real tmp284;
	  fftw_real tmp287;
	  fftw_real tmp288;
	  fftw_real tmp310;
	  fftw_real tmp291;
	  fftw_real tmp300;
	  fftw_real tmp315;
	  fftw_real tmp325;
	  fftw_real tmp331;
	  fftw_real tmp290;
	  fftw_real tmp289;
	  fftw_real tmp317;
	  fftw_real tmp318;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp275;
	       fftw_real tmp276;
	       fftw_real tmp278;
	       fftw_real tmp279;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp275 = X[0];
	       tmp276 = X[8 * iostride];
	       tmp277 = tmp275 + tmp276;
	       tmp278 = X[4 * iostride];
	       tmp279 = X[12 * iostride];
	       tmp280 = tmp278 + tmp279;
	       tmp281 = tmp277 + tmp280;
	       tmp309 = tmp275 - tmp276;
	       tmp292 = tmp278 - tmp279;
	  }
	  {
	       fftw_real tmp303;
	       fftw_real tmp320;
	       fftw_real tmp306;
	       fftw_real tmp321;
	       ASSERT_ALIGNED_DOUBLE;
	       {
		    fftw_real tmp301;
		    fftw_real tmp302;
		    fftw_real tmp304;
		    fftw_real tmp305;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp301 = X[iostride];
		    tmp302 = X[9 * iostride];
		    tmp303 = tmp301 - tmp302;
		    tmp320 = tmp301 + tmp302;
		    tmp304 = X[5 * iostride];
		    tmp305 = X[13 * iostride];
		    tmp306 = tmp304 - tmp305;
		    tmp321 = tmp304 + tmp305;
	       }
	       tmp307 = (K382683432 * tmp303) + (K923879532 * tmp306);
	       tmp314 = (K923879532 * tmp303) - (K382683432 * tmp306);
	       tmp322 = tmp320 - tmp321;
	       tmp330 = tmp320 + tmp321;
	  }
	  {
	       fftw_real tmp282;
	       fftw_real tmp283;
	       fftw_real tmp285;
	       fftw_real tmp286;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp282 = X[2 * iostride];
	       tmp283 = X[10 * iostride];
	       tmp284 = tmp282 + tmp283;
	       tmp290 = tmp282 - tmp283;
	       tmp285 = X[14 * iostride];
	       tmp286 = X[6 * iostride];
	       tmp287 = tmp285 + tmp286;
	       tmp289 = tmp285 - tmp286;
	  }
	  tmp288 = tmp284 + tmp287;
	  tmp310 = K707106781 * (tmp290 + tmp289);
	  tmp291 = K707106781 * (tmp289 - tmp290);
	  {
	       fftw_real tmp296;
	       fftw_real tmp323;
	       fftw_real tmp299;
	       fftw_real tmp324;
	       ASSERT_ALIGNED_DOUBLE;
	       {
		    fftw_real tmp294;
		    fftw_real tmp295;
		    fftw_real tmp297;
		    fftw_real tmp298;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp294 = X[15 * iostride];
		    tmp295 = X[7 * iostride];
		    tmp296 = tmp294 - tmp295;
		    tmp323 = tmp294 + tmp295;
		    tmp297 = X[3 * iostride];
		    tmp298 = X[11 * iostride];
		    tmp299 = tmp297 - tmp298;
		    tmp324 = tmp297 + tmp298;
	       }
	       tmp300 = (K382683432 * tmp296) - (K923879532 * tmp299);
	       tmp315 = (K923879532 * tmp296) + (K382683432 * tmp299);
	       tmp325 = tmp323 - tmp324;
	       tmp331 = tmp323 + tmp324;
	  }
	  {
	       fftw_real tmp329;
	       fftw_real tmp332;
	       fftw_real tmp327;
	       fftw_real tmp328;
	       ASSERT_ALIGNED_DOUBLE;
	       X[4 * iostride] = tmp281 - tmp288;
	       tmp329 = tmp281 + tmp288;
	       tmp332 = tmp330 + tmp331;
	       X[8 * iostride] = tmp329 - tmp332;
	       X[0] = tmp329 + tmp332;
	       Y[-4 * iostride] = tmp331 - tmp330;
	       tmp327 = tmp287 - tmp284;
	       tmp328 = K707106781 * (tmp325 - tmp322);
	       Y[-2 * iostride] = tmp327 + tmp328;
	       Y[-6 * iostride] = tmp328 - tmp327;
	  }
	  {
	       fftw_real tmp319;
	       fftw_real tmp326;
	       fftw_real tmp313;
	       fftw_real tmp316;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp319 = tmp277 - tmp280;
	       tmp326 = K707106781 * (tmp322 + tmp325);
	       X[6 * iostride] = tmp319 - tmp326;
	       X[2 * iostride] = tmp319 + tmp326;
	       tmp313 = tmp309 + tmp310;
	       tmp316 = tmp314 + tmp315;
	       X[7 * iostride] = tmp313 - tmp316;
	       X[iostride] = tmp313 + tmp316;
	  }
	  tmp317 = tmp292 + tmp291;
	  tmp318 = tmp315 - tmp314;
	  Y[-3 * iostride] = tmp317 + tmp318;
	  Y[-5 * iostride] = tmp318 - tmp317;
	  {
	       fftw_real tmp293;
	       fftw_real tmp308;
	       fftw_real tmp311;
	       fftw_real tmp312;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp293 = tmp291 - tmp292;
	       tmp308 = tmp300 - tmp307;
	       Y[-iostride] = tmp293 + tmp308;
	       Y[-7 * iostride] = tmp308 - tmp293;
	       tmp311 = tmp309 - tmp310;
	       tmp312 = tmp307 + tmp300;
	       X[5 * iostride] = tmp311 - tmp312;
	       X[3 * iostride] = tmp311 + tmp312;
	  }
     }
     X = X + dist;
     Y = Y - dist;
     for (i = 2; i < m; i = i + 2, X = X + dist, Y = Y - dist, W = W + 15) {
	  fftw_real tmp77;
	  fftw_real tmp161;
	  fftw_real tmp249;
	  fftw_real tmp262;
	  fftw_real tmp88;
	  fftw_real tmp263;
	  fftw_real tmp164;
	  fftw_real tmp246;
	  fftw_real tmp147;
	  fftw_real tmp158;
	  fftw_real tmp231;
	  fftw_real tmp198;
	  fftw_real tmp214;
	  fftw_real tmp232;
	  fftw_real tmp233;
	  fftw_real tmp234;
	  fftw_real tmp193;
	  fftw_real tmp213;
	  fftw_real tmp100;
	  fftw_real tmp222;
	  fftw_real tmp170;
	  fftw_real tmp206;
	  fftw_real tmp111;
	  fftw_real tmp223;
	  fftw_real tmp175;
	  fftw_real tmp207;
	  fftw_real tmp124;
	  fftw_real tmp135;
	  fftw_real tmp226;
	  fftw_real tmp187;
	  fftw_real tmp211;
	  fftw_real tmp227;
	  fftw_real tmp228;
	  fftw_real tmp229;
	  fftw_real tmp182;
	  fftw_real tmp210;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp71;
	       fftw_real tmp248;
	       fftw_real tmp76;
	       fftw_real tmp247;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp71 = X[0];
	       tmp248 = Y[-15 * iostride];
	       {
		    fftw_real tmp73;
		    fftw_real tmp75;
		    fftw_real tmp72;
		    fftw_real tmp74;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp73 = X[8 * iostride];
		    tmp75 = Y[-7 * iostride];
		    tmp72 = c_re(W[7]);
		    tmp74 = c_im(W[7]);
		    tmp76 = (tmp72 * tmp73) - (tmp74 * tmp75);
		    tmp247 = (tmp74 * tmp73) + (tmp72 * tmp75);
	       }
	       tmp77 = tmp71 + tmp76;
	       tmp161 = tmp71 - tmp76;
	       tmp249 = tmp247 + tmp248;
	       tmp262 = tmp248 - tmp247;
	  }
	  {
	       fftw_real tmp82;
	       fftw_real tmp162;
	       fftw_real tmp87;
	       fftw_real tmp163;
	       ASSERT_ALIGNED_DOUBLE;
	       {
		    fftw_real tmp79;
		    fftw_real tmp81;
		    fftw_real tmp78;
		    fftw_real tmp80;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp79 = X[4 * iostride];
		    tmp81 = Y[-11 * iostride];
		    tmp78 = c_re(W[3]);
		    tmp80 = c_im(W[3]);
		    tmp82 = (tmp78 * tmp79) - (tmp80 * tmp81);
		    tmp162 = (tmp80 * tmp79) + (tmp78 * tmp81);
	       }
	       {
		    fftw_real tmp84;
		    fftw_real tmp86;
		    fftw_real tmp83;
		    fftw_real tmp85;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp84 = X[12 * iostride];
		    tmp86 = Y[-3 * iostride];
		    tmp83 = c_re(W[11]);
		    tmp85 = c_im(W[11]);
		    tmp87 = (tmp83 * tmp84) - (tmp85 * tmp86);
		    tmp163 = (tmp85 * tmp84) + (tmp83 * tmp86);
	       }
	       tmp88 = tmp82 + tmp87;
	       tmp263 = tmp82 - tmp87;
	       tmp164 = tmp162 - tmp163;
	       tmp246 = tmp162 + tmp163;
	  }
	  {
	       fftw_real tmp141;
	       fftw_real tmp194;
	       fftw_real tmp157;
	       fftw_real tmp191;
	       fftw_real tmp146;
	       fftw_real tmp195;
	       fftw_real tmp152;
	       fftw_real tmp190;
	       ASSERT_ALIGNED_DOUBLE;
	       {
		    fftw_real tmp138;
		    fftw_real tmp140;
		    fftw_real tmp137;
		    fftw_real tmp139;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp138 = X[15 * iostride];
		    tmp140 = Y[0];
		    tmp137 = c_re(W[14]);
		    tmp139 = c_im(W[14]);
		    tmp141 = (tmp137 * tmp138) - (tmp139 * tmp140);
		    tmp194 = (tmp139 * tmp138) + (tmp137 * tmp140);
	       }
	       {
		    fftw_real tmp154;
		    fftw_real tmp156;
		    fftw_real tmp153;
		    fftw_real tmp155;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp154 = X[11 * iostride];
		    tmp156 = Y[-4 * iostride];
		    tmp153 = c_re(W[10]);
		    tmp155 = c_im(W[10]);
		    tmp157 = (tmp153 * tmp154) - (tmp155 * tmp156);
		    tmp191 = (tmp155 * tmp154) + (tmp153 * tmp156);
	       }
	       {
		    fftw_real tmp143;
		    fftw_real tmp145;
		    fftw_real tmp142;
		    fftw_real tmp144;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp143 = X[7 * iostride];
		    tmp145 = Y[-8 * iostride];
		    tmp142 = c_re(W[6]);
		    tmp144 = c_im(W[6]);
		    tmp146 = (tmp142 * tmp143) - (tmp144 * tmp145);
		    tmp195 = (tmp144 * tmp143) + (tmp142 * tmp145);
	       }
	       {
		    fftw_real tmp149;
		    fftw_real tmp151;
		    fftw_real tmp148;
		    fftw_real tmp150;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp149 = X[3 * iostride];
		    tmp151 = Y[-12 * iostride];
		    tmp148 = c_re(W[2]);
		    tmp150 = c_im(W[2]);
		    tmp152 = (tmp148 * tmp149) - (tmp150 * tmp151);
		    tmp190 = (tmp150 * tmp149) + (tmp148 * tmp151);
	       }
	       {
		    fftw_real tmp196;
		    fftw_real tmp197;
		    fftw_real tmp189;
		    fftw_real tmp192;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp147 = tmp141 + tmp146;
		    tmp158 = tmp152 + tmp157;
		    tmp231 = tmp147 - tmp158;
		    tmp196 = tmp194 - tmp195;
		    tmp197 = tmp152 - tmp157;
		    tmp198 = tmp196 + tmp197;
		    tmp214 = tmp196 - tmp197;
		    tmp232 = tmp194 + tmp195;
		    tmp233 = tmp190 + tmp191;
		    tmp234 = tmp232 - tmp233;
		    tmp189 = tmp141 - tmp146;
		    tmp192 = tmp190 - tmp191;
		    tmp193 = tmp189 - tmp192;
		    tmp213 = tmp189 + tmp192;
	       }
	  }
	  {
	       fftw_real tmp94;
	       fftw_real tmp166;
	       fftw_real tmp99;
	       fftw_real tmp167;
	       fftw_real tmp168;
	       fftw_real tmp169;
	       ASSERT_ALIGNED_DOUBLE;
	       {
		    fftw_real tmp91;
		    fftw_real tmp93;
		    fftw_real tmp90;
		    fftw_real tmp92;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp91 = X[2 * iostride];
		    tmp93 = Y[-13 * iostride];
		    tmp90 = c_re(W[1]);
		    tmp92 = c_im(W[1]);
		    tmp94 = (tmp90 * tmp91) - (tmp92 * tmp93);
		    tmp166 = (tmp92 * tmp91) + (tmp90 * tmp93);
	       }
	       {
		    fftw_real tmp96;
		    fftw_real tmp98;
		    fftw_real tmp95;
		    fftw_real tmp97;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp96 = X[10 * iostride];
		    tmp98 = Y[-5 * iostride];
		    tmp95 = c_re(W[9]);
		    tmp97 = c_im(W[9]);
		    tmp99 = (tmp95 * tmp96) - (tmp97 * tmp98);
		    tmp167 = (tmp97 * tmp96) + (tmp95 * tmp98);
	       }
	       tmp100 = tmp94 + tmp99;
	       tmp222 = tmp166 + tmp167;
	       tmp168 = tmp166 - tmp167;
	       tmp169 = tmp94 - tmp99;
	       tmp170 = tmp168 - tmp169;
	       tmp206 = tmp169 + tmp168;
	  }
	  {
	       fftw_real tmp105;
	       fftw_real tmp172;
	       fftw_real tmp110;
	       fftw_real tmp173;
	       fftw_real tmp171;
	       fftw_real tmp174;
	       ASSERT_ALIGNED_DOUBLE;
	       {
		    fftw_real tmp102;
		    fftw_real tmp104;
		    fftw_real tmp101;
		    fftw_real tmp103;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp102 = X[14 * iostride];
		    tmp104 = Y[-iostride];
		    tmp101 = c_re(W[13]);
		    tmp103 = c_im(W[13]);
		    tmp105 = (tmp101 * tmp102) - (tmp103 * tmp104);
		    tmp172 = (tmp103 * tmp102) + (tmp101 * tmp104);
	       }
	       {
		    fftw_real tmp107;
		    fftw_real tmp109;
		    fftw_real tmp106;
		    fftw_real tmp108;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp107 = X[6 * iostride];
		    tmp109 = Y[-9 * iostride];
		    tmp106 = c_re(W[5]);
		    tmp108 = c_im(W[5]);
		    tmp110 = (tmp106 * tmp107) - (tmp108 * tmp109);
		    tmp173 = (tmp108 * tmp107) + (tmp106 * tmp109);
	       }
	       tmp111 = tmp105 + tmp110;
	       tmp223 = tmp172 + tmp173;
	       tmp171 = tmp105 - tmp110;
	       tmp174 = tmp172 - tmp173;
	       tmp175 = tmp171 + tmp174;
	       tmp207 = tmp171 - tmp174;
	  }
	  {
	       fftw_real tmp118;
	       fftw_real tmp178;
	       fftw_real tmp134;
	       fftw_real tmp185;
	       fftw_real tmp123;
	       fftw_real tmp179;
	       fftw_real tmp129;
	       fftw_real tmp184;
	       ASSERT_ALIGNED_DOUBLE;
	       {
		    fftw_real tmp115;
		    fftw_real tmp117;
		    fftw_real tmp114;
		    fftw_real tmp116;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp115 = X[iostride];
		    tmp117 = Y[-14 * iostride];
		    tmp114 = c_re(W[0]);
		    tmp116 = c_im(W[0]);
		    tmp118 = (tmp114 * tmp115) - (tmp116 * tmp117);
		    tmp178 = (tmp116 * tmp115) + (tmp114 * tmp117);
	       }
	       {
		    fftw_real tmp131;
		    fftw_real tmp133;
		    fftw_real tmp130;
		    fftw_real tmp132;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp131 = X[13 * iostride];
		    tmp133 = Y[-2 * iostride];
		    tmp130 = c_re(W[12]);
		    tmp132 = c_im(W[12]);
		    tmp134 = (tmp130 * tmp131) - (tmp132 * tmp133);
		    tmp185 = (tmp132 * tmp131) + (tmp130 * tmp133);
	       }
	       {
		    fftw_real tmp120;
		    fftw_real tmp122;
		    fftw_real tmp119;
		    fftw_real tmp121;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp120 = X[9 * iostride];
		    tmp122 = Y[-6 * iostride];
		    tmp119 = c_re(W[8]);
		    tmp121 = c_im(W[8]);
		    tmp123 = (tmp119 * tmp120) - (tmp121 * tmp122);
		    tmp179 = (tmp121 * tmp120) + (tmp119 * tmp122);
	       }
	       {
		    fftw_real tmp126;
		    fftw_real tmp128;
		    fftw_real tmp125;
		    fftw_real tmp127;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp126 = X[5 * iostride];
		    tmp128 = Y[-10 * iostride];
		    tmp125 = c_re(W[4]);
		    tmp127 = c_im(W[4]);
		    tmp129 = (tmp125 * tmp126) - (tmp127 * tmp128);
		    tmp184 = (tmp127 * tmp126) + (tmp125 * tmp128);
	       }
	       {
		    fftw_real tmp183;
		    fftw_real tmp186;
		    fftw_real tmp180;
		    fftw_real tmp181;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp124 = tmp118 + tmp123;
		    tmp135 = tmp129 + tmp134;
		    tmp226 = tmp124 - tmp135;
		    tmp183 = tmp118 - tmp123;
		    tmp186 = tmp184 - tmp185;
		    tmp187 = tmp183 - tmp186;
		    tmp211 = tmp183 + tmp186;
		    tmp227 = tmp178 + tmp179;
		    tmp228 = tmp184 + tmp185;
		    tmp229 = tmp227 - tmp228;
		    tmp180 = tmp178 - tmp179;
		    tmp181 = tmp129 - tmp134;
		    tmp182 = tmp180 + tmp181;
		    tmp210 = tmp180 - tmp181;
	       }
	  }
	  {
	       fftw_real tmp177;
	       fftw_real tmp201;
	       fftw_real tmp271;
	       fftw_real tmp273;
	       fftw_real tmp200;
	       fftw_real tmp274;
	       fftw_real tmp204;
	       fftw_real tmp272;
	       ASSERT_ALIGNED_DOUBLE;
	       {
		    fftw_real tmp165;
		    fftw_real tmp176;
		    fftw_real tmp269;
		    fftw_real tmp270;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp165 = tmp161 - tmp164;
		    tmp176 = K707106781 * (tmp170 - tmp175);
		    tmp177 = tmp165 + tmp176;
		    tmp201 = tmp165 - tmp176;
		    tmp269 = K707106781 * (tmp207 - tmp206);
		    tmp270 = tmp263 + tmp262;
		    tmp271 = tmp269 + tmp270;
		    tmp273 = tmp270 - tmp269;
	       }
	       {
		    fftw_real tmp188;
		    fftw_real tmp199;
		    fftw_real tmp202;
		    fftw_real tmp203;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp188 = (K923879532 * tmp182) + (K382683432 * tmp187);
		    tmp199 = (K382683432 * tmp193) - (K923879532 * tmp198);
		    tmp200 = tmp188 + tmp199;
		    tmp274 = tmp199 - tmp188;
		    tmp202 = (K382683432 * tmp182) - (K923879532 * tmp187);
		    tmp203 = (K382683432 * tmp198) + (K923879532 * tmp193);
		    tmp204 = tmp202 - tmp203;
		    tmp272 = tmp202 + tmp203;
	       }
	       Y[-11 * iostride] = tmp177 - tmp200;
	       X[3 * iostride] = tmp177 + tmp200;
	       Y[-15 * iostride] = tmp201 - tmp204;
	       X[7 * iostride] = tmp201 + tmp204;
	       X[11 * iostride] = -(tmp271 - tmp272);
	       Y[-3 * iostride] = tmp272 + tmp271;
	       X[15 * iostride] = -(tmp273 - tmp274);
	       Y[-7 * iostride] = tmp274 + tmp273;
	  }
	  {
	       fftw_real tmp209;
	       fftw_real tmp217;
	       fftw_real tmp265;
	       fftw_real tmp267;
	       fftw_real tmp216;
	       fftw_real tmp268;
	       fftw_real tmp220;
	       fftw_real tmp266;
	       ASSERT_ALIGNED_DOUBLE;
	       {
		    fftw_real tmp205;
		    fftw_real tmp208;
		    fftw_real tmp261;
		    fftw_real tmp264;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp205 = tmp161 + tmp164;
		    tmp208 = K707106781 * (tmp206 + tmp207);
		    tmp209 = tmp205 + tmp208;
		    tmp217 = tmp205 - tmp208;
		    tmp261 = K707106781 * (tmp170 + tmp175);
		    tmp264 = tmp262 - tmp263;
		    tmp265 = tmp261 + tmp264;
		    tmp267 = tmp264 - tmp261;
	       }
	       {
		    fftw_real tmp212;
		    fftw_real tmp215;
		    fftw_real tmp218;
		    fftw_real tmp219;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp212 = (K382683432 * tmp210) + (K923879532 * tmp211);
		    tmp215 = (K923879532 * tmp213) - (K382683432 * tmp214);
		    tmp216 = tmp212 + tmp215;
		    tmp268 = tmp215 - tmp212;
		    tmp218 = (K923879532 * tmp210) - (K382683432 * tmp211);
		    tmp219 = (K923879532 * tmp214) + (K382683432 * tmp213);
		    tmp220 = tmp218 - tmp219;
		    tmp266 = tmp218 + tmp219;
	       }
	       Y[-9 * iostride] = tmp209 - tmp216;
	       X[iostride] = tmp209 + tmp216;
	       Y[-13 * iostride] = tmp217 - tmp220;
	       X[5 * iostride] = tmp217 + tmp220;
	       X[9 * iostride] = -(tmp265 - tmp266);
	       Y[-iostride] = tmp266 + tmp265;
	       X[13 * iostride] = -(tmp267 - tmp268);
	       Y[-5 * iostride] = tmp268 + tmp267;
	  }
	  {
	       fftw_real tmp225;
	       fftw_real tmp237;
	       fftw_real tmp257;
	       fftw_real tmp259;
	       fftw_real tmp236;
	       fftw_real tmp260;
	       fftw_real tmp240;
	       fftw_real tmp258;
	       ASSERT_ALIGNED_DOUBLE;
	       {
		    fftw_real tmp221;
		    fftw_real tmp224;
		    fftw_real tmp255;
		    fftw_real tmp256;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp221 = tmp77 - tmp88;
		    tmp224 = tmp222 - tmp223;
		    tmp225 = tmp221 + tmp224;
		    tmp237 = tmp221 - tmp224;
		    tmp255 = tmp111 - tmp100;
		    tmp256 = tmp249 - tmp246;
		    tmp257 = tmp255 + tmp256;
		    tmp259 = tmp256 - tmp255;
	       }
	       {
		    fftw_real tmp230;
		    fftw_real tmp235;
		    fftw_real tmp238;
		    fftw_real tmp239;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp230 = tmp226 + tmp229;
		    tmp235 = tmp231 - tmp234;
		    tmp236 = K707106781 * (tmp230 + tmp235);
		    tmp260 = K707106781 * (tmp235 - tmp230);
		    tmp238 = tmp229 - tmp226;
		    tmp239 = tmp231 + tmp234;
		    tmp240 = K707106781 * (tmp238 - tmp239);
		    tmp258 = K707106781 * (tmp238 + tmp239);
	       }
	       Y[-10 * iostride] = tmp225 - tmp236;
	       X[2 * iostride] = tmp225 + tmp236;
	       Y[-14 * iostride] = tmp237 - tmp240;
	       X[6 * iostride] = tmp237 + tmp240;
	       X[10 * iostride] = -(tmp257 - tmp258);
	       Y[-2 * iostride] = tmp258 + tmp257;
	       X[14 * iostride] = -(tmp259 - tmp260);
	       Y[-6 * iostride] = tmp260 + tmp259;
	  }
	  {
	       fftw_real tmp113;
	       fftw_real tmp241;
	       fftw_real tmp251;
	       fftw_real tmp253;
	       fftw_real tmp160;
	       fftw_real tmp254;
	       fftw_real tmp244;
	       fftw_real tmp252;
	       ASSERT_ALIGNED_DOUBLE;
	       {
		    fftw_real tmp89;
		    fftw_real tmp112;
		    fftw_real tmp245;
		    fftw_real tmp250;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp89 = tmp77 + tmp88;
		    tmp112 = tmp100 + tmp111;
		    tmp113 = tmp89 + tmp112;
		    tmp241 = tmp89 - tmp112;
		    tmp245 = tmp222 + tmp223;
		    tmp250 = tmp246 + tmp249;
		    tmp251 = tmp245 + tmp250;
		    tmp253 = tmp250 - tmp245;
	       }
	       {
		    fftw_real tmp136;
		    fftw_real tmp159;
		    fftw_real tmp242;
		    fftw_real tmp243;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp136 = tmp124 + tmp135;
		    tmp159 = tmp147 + tmp158;
		    tmp160 = tmp136 + tmp159;
		    tmp254 = tmp159 - tmp136;
		    tmp242 = tmp227 + tmp228;
		    tmp243 = tmp232 + tmp233;
		    tmp244 = tmp242 - tmp243;
		    tmp252 = tmp242 + tmp243;
	       }
	       Y[-8 * iostride] = tmp113 - tmp160;
	       X[0] = tmp113 + tmp160;
	       Y[-12 * iostride] = tmp241 - tmp244;
	       X[4 * iostride] = tmp241 + tmp244;
	       X[8 * iostride] = -(tmp251 - tmp252);
	       Y[0] = tmp252 + tmp251;
	       X[12 * iostride] = -(tmp253 - tmp254);
	       Y[-4 * iostride] = tmp254 + tmp253;
	  }
     }
     if (i == m) {
	  fftw_real tmp5;
	  fftw_real tmp41;
	  fftw_real tmp61;
	  fftw_real tmp67;
	  fftw_real tmp30;
	  fftw_real tmp49;
	  fftw_real tmp34;
	  fftw_real tmp50;
	  fftw_real tmp12;
	  fftw_real tmp66;
	  fftw_real tmp44;
	  fftw_real tmp58;
	  fftw_real tmp19;
	  fftw_real tmp46;
	  fftw_real tmp23;
	  fftw_real tmp47;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp1;
	       fftw_real tmp60;
	       fftw_real tmp4;
	       fftw_real tmp59;
	       fftw_real tmp2;
	       fftw_real tmp3;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp1 = X[0];
	       tmp60 = X[8 * iostride];
	       tmp2 = X[4 * iostride];
	       tmp3 = X[12 * iostride];
	       tmp4 = K707106781 * (tmp2 - tmp3);
	       tmp59 = K707106781 * (tmp2 + tmp3);
	       tmp5 = tmp1 + tmp4;
	       tmp41 = tmp1 - tmp4;
	       tmp61 = tmp59 + tmp60;
	       tmp67 = tmp60 - tmp59;
	  }
	  {
	       fftw_real tmp29;
	       fftw_real tmp33;
	       fftw_real tmp27;
	       fftw_real tmp31;
	       ASSERT_ALIGNED_DOUBLE;
	       {
		    fftw_real tmp28;
		    fftw_real tmp32;
		    fftw_real tmp25;
		    fftw_real tmp26;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp28 = X[15 * iostride];
		    tmp29 = K2_000000000 * tmp28;
		    tmp32 = X[7 * iostride];
		    tmp33 = K2_000000000 * tmp32;
		    tmp25 = X[3 * iostride];
		    tmp26 = X[11 * iostride];
		    tmp27 = K1_414213562 * (tmp25 - tmp26);
		    tmp31 = K1_414213562 * (tmp25 + tmp26);
	       }
	       tmp30 = tmp27 - tmp29;
	       tmp49 = tmp27 + tmp29;
	       tmp34 = tmp31 + tmp33;
	       tmp50 = tmp33 - tmp31;
	  }
	  {
	       fftw_real tmp8;
	       fftw_real tmp42;
	       fftw_real tmp11;
	       fftw_real tmp43;
	       ASSERT_ALIGNED_DOUBLE;
	       {
		    fftw_real tmp6;
		    fftw_real tmp7;
		    fftw_real tmp9;
		    fftw_real tmp10;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp6 = X[2 * iostride];
		    tmp7 = X[10 * iostride];
		    tmp8 = (K923879532 * tmp6) - (K382683432 * tmp7);
		    tmp42 = (K382683432 * tmp6) + (K923879532 * tmp7);
		    tmp9 = X[6 * iostride];
		    tmp10 = X[14 * iostride];
		    tmp11 = (K382683432 * tmp9) - (K923879532 * tmp10);
		    tmp43 = (K923879532 * tmp9) + (K382683432 * tmp10);
	       }
	       tmp12 = tmp8 + tmp11;
	       tmp66 = tmp11 - tmp8;
	       tmp44 = tmp42 - tmp43;
	       tmp58 = tmp42 + tmp43;
	  }
	  {
	       fftw_real tmp15;
	       fftw_real tmp22;
	       fftw_real tmp18;
	       fftw_real tmp20;
	       ASSERT_ALIGNED_DOUBLE;
	       {
		    fftw_real tmp14;
		    fftw_real tmp21;
		    fftw_real tmp16;
		    fftw_real tmp17;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp14 = X[iostride];
		    tmp15 = K2_000000000 * tmp14;
		    tmp21 = X[9 * iostride];
		    tmp22 = K2_000000000 * tmp21;
		    tmp16 = X[5 * iostride];
		    tmp17 = X[13 * iostride];
		    tmp18 = K1_414213562 * (tmp16 - tmp17);
		    tmp20 = K1_414213562 * (tmp16 + tmp17);
	       }
	       tmp19 = tmp15 + tmp18;
	       tmp46 = tmp15 - tmp18;
	       tmp23 = tmp20 + tmp22;
	       tmp47 = tmp22 - tmp20;
	  }
	  {
	       fftw_real tmp13;
	       fftw_real tmp62;
	       fftw_real tmp36;
	       fftw_real tmp57;
	       fftw_real tmp24;
	       fftw_real tmp35;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp13 = tmp5 - tmp12;
	       tmp62 = tmp58 + tmp61;
	       tmp24 = (K097545161 * tmp19) + (K490392640 * tmp23);
	       tmp35 = (K097545161 * tmp30) - (K490392640 * tmp34);
	       tmp36 = tmp24 + tmp35;
	       tmp57 = tmp35 - tmp24;
	       X[4 * iostride] = tmp13 - tmp36;
	       X[3 * iostride] = tmp13 + tmp36;
	       Y[0] = tmp57 - tmp62;
	       Y[-7 * iostride] = tmp57 + tmp62;
	  }
	  {
	       fftw_real tmp37;
	       fftw_real tmp64;
	       fftw_real tmp40;
	       fftw_real tmp63;
	       fftw_real tmp38;
	       fftw_real tmp39;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp37 = tmp5 + tmp12;
	       tmp64 = tmp61 - tmp58;
	       tmp38 = (K490392640 * tmp19) - (K097545161 * tmp23);
	       tmp39 = (K490392640 * tmp30) + (K097545161 * tmp34);
	       tmp40 = tmp38 + tmp39;
	       tmp63 = tmp39 - tmp38;
	       X[7 * iostride] = tmp37 - tmp40;
	       X[0] = tmp37 + tmp40;
	       Y[-4 * iostride] = tmp63 - tmp64;
	       Y[-3 * iostride] = tmp63 + tmp64;
	  }
	  {
	       fftw_real tmp45;
	       fftw_real tmp68;
	       fftw_real tmp52;
	       fftw_real tmp65;
	       fftw_real tmp48;
	       fftw_real tmp51;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp45 = tmp41 + tmp44;
	       tmp68 = tmp66 - tmp67;
	       tmp48 = (K415734806 * tmp46) + (K277785116 * tmp47);
	       tmp51 = (K415734806 * tmp49) + (K277785116 * tmp50);
	       tmp52 = tmp48 - tmp51;
	       tmp65 = tmp48 + tmp51;
	       X[6 * iostride] = tmp45 - tmp52;
	       X[iostride] = tmp45 + tmp52;
	       Y[-5 * iostride] = -(tmp65 + tmp68);
	       Y[-2 * iostride] = tmp68 - tmp65;
	  }
	  {
	       fftw_real tmp53;
	       fftw_real tmp70;
	       fftw_real tmp56;
	       fftw_real tmp69;
	       fftw_real tmp54;
	       fftw_real tmp55;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp53 = tmp41 - tmp44;
	       tmp70 = tmp66 + tmp67;
	       tmp54 = (K415734806 * tmp50) - (K277785116 * tmp49);
	       tmp55 = (K415734806 * tmp47) - (K277785116 * tmp46);
	       tmp56 = tmp54 - tmp55;
	       tmp69 = tmp55 + tmp54;
	       X[5 * iostride] = tmp53 - tmp56;
	       X[2 * iostride] = tmp53 + tmp56;
	       Y[-6 * iostride] = tmp69 - tmp70;
	       Y[-iostride] = tmp69 + tmp70;
	  }
     }
}

static const int twiddle_order[] =
{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
fftw_codelet_desc fftw_hc2hc_forward_16_desc =
{
     "fftw_hc2hc_forward_16",
     (void (*)()) fftw_hc2hc_forward_16,
     16,
     FFTW_FORWARD,
     FFTW_HC2HC,
     355,
     15,
     twiddle_order,
};
