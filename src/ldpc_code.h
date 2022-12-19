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

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

namespace libldpc
{
    struct LDPC_Code
    {
        int _N = 0;
        int _K = 0;
        int _M = 0;

        int NOEUD = 0;
        int MESSAGE = 0;

        int NminusK = 0;

        std::vector<int> DEGREES;
        std::vector<int> DEGREES_COMPUTATIONS;
        int NB_DEGRES = 0;

        std::vector<unsigned short> PosNoeudsVariable; // Not sure if using SIMD there helps a lot at all

        void setup()
        {
            NB_DEGRES = DEGREES.size();

            NOEUD = _N;
            MESSAGE = _M;

            NminusK = (_N - _K);
        }
    };

    LDPC_Code get_CCSDS_8176_1022_Code();
    LDPC_Code get_ldpc_code_alist(std::string alist_path);
}