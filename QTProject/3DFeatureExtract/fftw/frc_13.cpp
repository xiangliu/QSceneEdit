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
/* Generated on Sun Nov  7 20:43:56 EST 1999 */
//#include "stdafx.h"
#include "fftw-int.h"
#include "fftw.h"

/* Generated by: ./genfft -magic-alignment-check -magic-twiddle-load-all -magic-variables 4 -magic-loopi -real2hc 13 */

/*
 * This function contains 76 FP additions, 34 FP multiplications,
 * (or, 57 additions, 15 multiplications, 19 fused multiply/add),
 * 34 stack variables, and 26 memory accesses
 */
static const fftw_real K083333333 = FFTW_KONST(+0.083333333333333333333333333333333333333333333);
static const fftw_real K075902986 = FFTW_KONST(+0.075902986037193865983102897245103540356428373);
static const fftw_real K251768516 = FFTW_KONST(+0.251768516431883313623436926934233488546674281);
static const fftw_real K503537032 = FFTW_KONST(+0.503537032863766627246873853868466977093348562);
static const fftw_real K113854479 = FFTW_KONST(+0.113854479055790798974654345867655310534642560);
static const fftw_real K265966249 = FFTW_KONST(+0.265966249214837287587521063842185948798330267);
static const fftw_real K387390585 = FFTW_KONST(+0.387390585467617292130675966426762851778775217);
static const fftw_real K300462606 = FFTW_KONST(+0.300462606288665774426601772289207995520941381);
static const fftw_real K258260390 = FFTW_KONST(+0.258260390311744861420450644284508567852516811);
static const fftw_real K132983124 = FFTW_KONST(+0.132983124607418643793760531921092974399165133);
static const fftw_real K2_000000000 = FFTW_KONST(+2.000000000000000000000000000000000000000000000);
static const fftw_real K1_732050807 = FFTW_KONST(+1.732050807568877293527446341505872366942805254);
static const fftw_real K156891391 = FFTW_KONST(+0.156891391051584611046832726756003269660212636);
static const fftw_real K256247671 = FFTW_KONST(+0.256247671582936600958684654061725059144125175);
static const fftw_real K011599105 = FFTW_KONST(+0.011599105605768290721655456654083252189827041);
static const fftw_real K300238635 = FFTW_KONST(+0.300238635966332641462884626667381504676006424);
static const fftw_real K174138601 = FFTW_KONST(+0.174138601152135905005660794929264742616964676);
static const fftw_real K575140729 = FFTW_KONST(+0.575140729474003121368385547455453388461001608);
static const fftw_real K866025403 = FFTW_KONST(+0.866025403784438646763723170752936183471402627);
static const fftw_real K500000000 = FFTW_KONST(+0.500000000000000000000000000000000000000000000);

/*
 * Generator Id's : 
 * $Id: exprdag.ml,v 1.41 1999/05/26 15:44:14 fftw Exp $
 * $Id: fft.ml,v 1.43 1999/05/17 19:44:18 fftw Exp $
 * $Id: to_c.ml,v 1.25 1999/10/26 21:41:32 stevenj Exp $
 */

void fftw_real2hc_13(const fftw_real *input, fftw_real *real_output, fftw_real *imag_output, int istride, int real_ostride, int imag_ostride)
{
     fftw_real tmp65;
     fftw_real tmp11;
     fftw_real tmp28;
     fftw_real tmp37;
     fftw_real tmp51;
     fftw_real tmp62;
     fftw_real tmp22;
     fftw_real tmp58;
     fftw_real tmp59;
     fftw_real tmp66;
     fftw_real tmp56;
     fftw_real tmp63;
     fftw_real tmp35;
     fftw_real tmp38;
     ASSERT_ALIGNED_DOUBLE;
     tmp65 = input[0];
     {
	  fftw_real tmp3;
	  fftw_real tmp53;
	  fftw_real tmp21;
	  fftw_real tmp30;
	  fftw_real tmp26;
	  fftw_real tmp16;
	  fftw_real tmp29;
	  fftw_real tmp25;
	  fftw_real tmp6;
	  fftw_real tmp32;
	  fftw_real tmp9;
	  fftw_real tmp33;
	  fftw_real tmp10;
	  fftw_real tmp54;
	  fftw_real tmp1;
	  fftw_real tmp2;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp1 = input[8 * istride];
	  tmp2 = input[5 * istride];
	  tmp3 = tmp1 - tmp2;
	  tmp53 = tmp1 + tmp2;
	  {
	       fftw_real tmp17;
	       fftw_real tmp18;
	       fftw_real tmp19;
	       fftw_real tmp20;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp17 = input[12 * istride];
	       tmp18 = input[4 * istride];
	       tmp19 = input[10 * istride];
	       tmp20 = tmp18 + tmp19;
	       tmp21 = tmp17 + tmp20;
	       tmp30 = tmp17 - (K500000000 * tmp20);
	       tmp26 = tmp18 - tmp19;
	  }
	  {
	       fftw_real tmp12;
	       fftw_real tmp13;
	       fftw_real tmp14;
	       fftw_real tmp15;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp12 = input[istride];
	       tmp13 = input[3 * istride];
	       tmp14 = input[9 * istride];
	       tmp15 = tmp13 + tmp14;
	       tmp16 = tmp12 + tmp15;
	       tmp29 = tmp12 - (K500000000 * tmp15);
	       tmp25 = tmp13 - tmp14;
	  }
	  {
	       fftw_real tmp4;
	       fftw_real tmp5;
	       fftw_real tmp7;
	       fftw_real tmp8;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp4 = input[6 * istride];
	       tmp5 = input[11 * istride];
	       tmp6 = tmp4 - tmp5;
	       tmp32 = tmp4 + tmp5;
	       tmp7 = input[2 * istride];
	       tmp8 = input[7 * istride];
	       tmp9 = tmp7 - tmp8;
	       tmp33 = tmp7 + tmp8;
	  }
	  tmp10 = tmp6 + tmp9;
	  tmp54 = tmp32 + tmp33;
	  tmp11 = tmp3 - tmp10;
	  {
	       fftw_real tmp24;
	       fftw_real tmp27;
	       fftw_real tmp49;
	       fftw_real tmp50;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp24 = tmp3 + (K500000000 * tmp10);
	       tmp27 = K866025403 * (tmp25 + tmp26);
	       tmp28 = tmp24 - tmp27;
	       tmp37 = tmp27 + tmp24;
	       tmp49 = tmp9 - tmp6;
	       tmp50 = tmp25 - tmp26;
	       tmp51 = tmp49 - tmp50;
	       tmp62 = tmp50 + tmp49;
	  }
	  tmp22 = tmp16 - tmp21;
	  tmp58 = tmp16 + tmp21;
	  tmp59 = tmp53 + tmp54;
	  tmp66 = tmp58 + tmp59;
	  {
	       fftw_real tmp52;
	       fftw_real tmp55;
	       fftw_real tmp31;
	       fftw_real tmp34;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp52 = tmp29 + tmp30;
	       tmp55 = tmp53 - (K500000000 * tmp54);
	       tmp56 = tmp52 - tmp55;
	       tmp63 = tmp52 + tmp55;
	       tmp31 = tmp29 - tmp30;
	       tmp34 = K866025403 * (tmp32 - tmp33);
	       tmp35 = tmp31 + tmp34;
	       tmp38 = tmp31 - tmp34;
	  }
     }
     real_output[0] = tmp65 + tmp66;
     {
	  fftw_real tmp23;
	  fftw_real tmp45;
	  fftw_real tmp40;
	  fftw_real tmp48;
	  fftw_real tmp44;
	  fftw_real tmp46;
	  fftw_real tmp41;
	  fftw_real tmp47;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp23 = (K575140729 * tmp11) - (K174138601 * tmp22);
	  tmp45 = (K575140729 * tmp22) + (K174138601 * tmp11);
	  {
	       fftw_real tmp36;
	       fftw_real tmp39;
	       fftw_real tmp42;
	       fftw_real tmp43;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp36 = (K300238635 * tmp28) + (K011599105 * tmp35);
	       tmp39 = (K256247671 * tmp37) + (K156891391 * tmp38);
	       tmp40 = tmp36 - tmp39;
	       tmp48 = K1_732050807 * (tmp39 + tmp36);
	       tmp42 = (K300238635 * tmp35) - (K011599105 * tmp28);
	       tmp43 = (K156891391 * tmp37) - (K256247671 * tmp38);
	       tmp44 = K1_732050807 * (tmp42 - tmp43);
	       tmp46 = tmp43 + tmp42;
	  }
	  imag_output[imag_ostride] = tmp23 + (K2_000000000 * tmp40);
	  tmp41 = tmp23 - tmp40;
	  imag_output[3 * imag_ostride] = tmp41 - tmp44;
	  imag_output[4 * imag_ostride] = -(tmp41 + tmp44);
	  imag_output[5 * imag_ostride] = -(tmp45 + (K2_000000000 * tmp46));
	  tmp47 = tmp46 - tmp45;
	  imag_output[2 * imag_ostride] = tmp47 - tmp48;
	  imag_output[6 * imag_ostride] = tmp48 + tmp47;
     }
     {
	  fftw_real tmp61;
	  fftw_real tmp70;
	  fftw_real tmp74;
	  fftw_real tmp76;
	  fftw_real tmp68;
	  fftw_real tmp69;
	  fftw_real tmp57;
	  fftw_real tmp60;
	  fftw_real tmp71;
	  fftw_real tmp75;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp57 = (K132983124 * tmp51) + (K258260390 * tmp56);
	  tmp60 = K300462606 * (tmp58 - tmp59);
	  tmp61 = (K2_000000000 * tmp57) + tmp60;
	  tmp70 = tmp60 - tmp57;
	  {
	       fftw_real tmp72;
	       fftw_real tmp73;
	       fftw_real tmp64;
	       fftw_real tmp67;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp72 = (K387390585 * tmp51) - (K265966249 * tmp56);
	       tmp73 = (K113854479 * tmp62) - (K503537032 * tmp63);
	       tmp74 = tmp72 + tmp73;
	       tmp76 = tmp73 - tmp72;
	       tmp64 = (K251768516 * tmp62) + (K075902986 * tmp63);
	       tmp67 = tmp65 - (K083333333 * tmp66);
	       tmp68 = (K2_000000000 * tmp64) + tmp67;
	       tmp69 = tmp67 - tmp64;
	  }
	  real_output[real_ostride] = tmp61 + tmp68;
	  real_output[5 * real_ostride] = tmp68 - tmp61;
	  tmp71 = tmp69 - tmp70;
	  real_output[2 * real_ostride] = tmp71 - tmp74;
	  real_output[6 * real_ostride] = tmp74 + tmp71;
	  tmp75 = tmp70 + tmp69;
	  real_output[3 * real_ostride] = tmp75 - tmp76;
	  real_output[4 * real_ostride] = tmp76 + tmp75;
     }
}

fftw_codelet_desc fftw_real2hc_13_desc =
{
     "fftw_real2hc_13",
     (void (*)()) fftw_real2hc_13,
     13,
     FFTW_FORWARD,
     FFTW_REAL2HC,
     288,
     0,
     (const int *) 0,
};
