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
/* Generated on Sun Nov  7 20:44:21 EST 1999 */
//#include "stdafx.h"
#include "fftw-int.h"
#include "fftw.h"

/* Generated by: ./genfft -magic-alignment-check -magic-twiddle-load-all -magic-variables 4 -magic-loopi -hc2real 9 */

/*
 * This function contains 32 FP additions, 18 FP multiplications,
 * (or, 22 additions, 8 multiplications, 10 fused multiply/add),
 * 22 stack variables, and 18 memory accesses
 */
static const fftw_real K984807753 = FFTW_KONST(+0.984807753012208059366743024589523013670643252);
static const fftw_real K173648177 = FFTW_KONST(+0.173648177666930348851716626769314796000375677);
static const fftw_real K300767466 = FFTW_KONST(+0.300767466360870593278543795225003852144476517);
static const fftw_real K1_705737063 = FFTW_KONST(+1.705737063904886419256501927880148143872040591);
static const fftw_real K642787609 = FFTW_KONST(+0.642787609686539326322643409907263432907559884);
static const fftw_real K766044443 = FFTW_KONST(+0.766044443118978035202392650555416673935832457);
static const fftw_real K1_326827896 = FFTW_KONST(+1.326827896337876792410842639271782594433726619);
static const fftw_real K1_113340798 = FFTW_KONST(+1.113340798452838732905825904094046265936583811);
static const fftw_real K500000000 = FFTW_KONST(+0.500000000000000000000000000000000000000000000);
static const fftw_real K866025403 = FFTW_KONST(+0.866025403784438646763723170752936183471402627);
static const fftw_real K2_000000000 = FFTW_KONST(+2.000000000000000000000000000000000000000000000);
static const fftw_real K1_732050807 = FFTW_KONST(+1.732050807568877293527446341505872366942805254);

/*
 * Generator Id's : 
 * $Id: exprdag.ml,v 1.41 1999/05/26 15:44:14 fftw Exp $
 * $Id: fft.ml,v 1.43 1999/05/17 19:44:18 fftw Exp $
 * $Id: to_c.ml,v 1.25 1999/10/26 21:41:32 stevenj Exp $
 */

void fftw_hc2real_9(const fftw_real *real_input, const fftw_real *imag_input, fftw_real *output, int real_istride, int imag_istride, int ostride)
{
     fftw_real tmp3;
     fftw_real tmp26;
     fftw_real tmp12;
     fftw_real tmp20;
     fftw_real tmp19;
     fftw_real tmp8;
     fftw_real tmp17;
     fftw_real tmp27;
     fftw_real tmp22;
     fftw_real tmp28;
     fftw_real tmp32;
     fftw_real tmp33;
     ASSERT_ALIGNED_DOUBLE;
     {
	  fftw_real tmp11;
	  fftw_real tmp1;
	  fftw_real tmp2;
	  fftw_real tmp9;
	  fftw_real tmp10;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp10 = imag_input[3 * imag_istride];
	  tmp11 = K1_732050807 * tmp10;
	  tmp1 = real_input[0];
	  tmp2 = real_input[3 * real_istride];
	  tmp9 = tmp1 - tmp2;
	  tmp3 = tmp1 + (K2_000000000 * tmp2);
	  tmp26 = tmp9 + tmp11;
	  tmp12 = tmp9 - tmp11;
     }
     {
	  fftw_real tmp4;
	  fftw_real tmp7;
	  fftw_real tmp18;
	  fftw_real tmp16;
	  fftw_real tmp13;
	  fftw_real tmp21;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp4 = real_input[real_istride];
	  tmp20 = imag_input[imag_istride];
	  {
	       fftw_real tmp5;
	       fftw_real tmp6;
	       fftw_real tmp14;
	       fftw_real tmp15;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp5 = real_input[4 * real_istride];
	       tmp6 = real_input[2 * real_istride];
	       tmp7 = tmp5 + tmp6;
	       tmp18 = K866025403 * (tmp5 - tmp6);
	       tmp14 = imag_input[4 * imag_istride];
	       tmp15 = imag_input[2 * imag_istride];
	       tmp16 = K866025403 * (tmp14 + tmp15);
	       tmp19 = tmp15 - tmp14;
	  }
	  tmp8 = tmp4 + tmp7;
	  tmp13 = tmp4 - (K500000000 * tmp7);
	  tmp17 = tmp13 - tmp16;
	  tmp27 = tmp13 + tmp16;
	  tmp21 = (K500000000 * tmp19) + tmp20;
	  tmp22 = tmp18 + tmp21;
	  tmp28 = tmp21 - tmp18;
     }
     output[0] = tmp3 + (K2_000000000 * tmp8);
     tmp32 = tmp3 - tmp8;
     tmp33 = K1_732050807 * (tmp20 - tmp19);
     output[3 * ostride] = tmp32 - tmp33;
     output[6 * ostride] = tmp32 + tmp33;
     {
	  fftw_real tmp25;
	  fftw_real tmp23;
	  fftw_real tmp24;
	  fftw_real tmp31;
	  fftw_real tmp29;
	  fftw_real tmp30;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp25 = (K1_113340798 * tmp17) + (K1_326827896 * tmp22);
	  tmp23 = (K766044443 * tmp17) - (K642787609 * tmp22);
	  tmp24 = tmp12 - tmp23;
	  output[ostride] = tmp12 + (K2_000000000 * tmp23);
	  output[7 * ostride] = tmp24 + tmp25;
	  output[4 * ostride] = tmp24 - tmp25;
	  tmp31 = (K1_705737063 * tmp27) + (K300767466 * tmp28);
	  tmp29 = (K173648177 * tmp27) - (K984807753 * tmp28);
	  tmp30 = tmp26 - tmp29;
	  output[2 * ostride] = tmp26 + (K2_000000000 * tmp29);
	  output[8 * ostride] = tmp30 + tmp31;
	  output[5 * ostride] = tmp30 - tmp31;
     }
}

fftw_codelet_desc fftw_hc2real_9_desc =
{
     "fftw_hc2real_9",
     (void (*)()) fftw_hc2real_9,
     9,
     FFTW_BACKWARD,
     FFTW_HC2REAL,
     213,
     0,
     (const int *) 0,
};
