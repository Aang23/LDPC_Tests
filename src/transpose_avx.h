/**
  Copyright (c) 2012-2015 "Bordeaux INP, Bertrand LE GAL"
  [http://legal.vvv.enseirb-matmeca.fr]

  This file is part of LDPC_C_Simulator.

  LDPC_C_Simulator is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * File:   Code_Analyzer.hpp
 * Author: cleroux
 *
 * Created on September 17, 2013, 9:49 AM
 */

#pragma once

#include "config.h"

#if LIBLDPC_ENABLE_AVX

#include <xmmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>

namespace libldpc
{
    void uchar_transpose_avx(__m256i *src, __m256i *dst, int n);
    void uchar_itranspose_avx(__m256i *src, __m256i *dst, int n);
    // void test_transpose_avx();
}
#endif
