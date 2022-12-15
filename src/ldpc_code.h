#pragma once

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

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
// #include <volk/volk_alloc.hh>
#include <vector>

struct LDPC_Code
{
    int NB_DEGRES = 0;

    int _N = 0;
    int _K = 0;
    int _M = 0;

    int NOEUD = 0;
    int MESSAGE = 0;

    int NmoinsK = 0;

    // int DEG_1 = 0;
    // int DEG_2 = 0;
    std::vector<int> DEGREES;

    // int DEG_1_COMPUTATIONS = 0;
    // int DEG_2_COMPUTATIONS = 0;
    std::vector<int> DEGREES_COMPUTATIONS;

    int NB_ITERATIONS = 0;
    int NB_BITS_VARIABLES = 0;
    int NB_BITS_MESSAGES = 0;
    int SAT_POS_VAR = 0;
    int SAT_NEG_VAR = 0;
    int SAT_POS_MSG = 0;
    int SAT_NEG_MSG = 0;

    std::vector<unsigned short> PosNoeudsVariable; // Not sure if using SIMD there helps a lot at all

    void setup()
    {
        NB_DEGRES = DEGREES.size();

        NOEUD = _N;
        MESSAGE = _M;

        NmoinsK = (_N - _K);

        SAT_POS_VAR = ((0x0001 << (NB_BITS_VARIABLES - 1)) - 1);
        SAT_NEG_VAR = (-(0x0001 << (NB_BITS_VARIABLES - 1)) + 1);
        SAT_POS_MSG = ((0x0001 << (NB_BITS_MESSAGES - 1)) - 1);
        SAT_NEG_MSG = (-(0x0001 << (NB_BITS_MESSAGES - 1)) + 1);
    }
};

LDPC_Code get_CCSDS_8176_1022_Code();
