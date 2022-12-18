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
#include "ldpc_code.h"

namespace libldpc
{
    class LDPCDecoder
    {
    protected:
        float sigB;
        int nb_iters; // Use early termination criteria
        const LDPC_Code code;

        // Fixed codes
        int vSAT_NEG_MSG = -127;
        int vSAT_POS_MSG = 127;
        int vSAT_NEG_VAR = -127;
        int vSAT_POS_VAR = 127;

    public:
        LDPCDecoder(LDPC_Code code);
        virtual ~LDPCDecoder();

        virtual void setSigmaChannel(float _sigB);
        virtual void setNumberOfIterations(int _value);

        virtual void setVarRange(int min, int max);
        virtual void setMsgRange(int min, int max);

        virtual void decode(char var_nodes[], char Rprime_fix[], int nombre_iterations) = 0;

        virtual int getSIMDSize() = 0;
    };

    enum ldpc_decoder_type_t
    {
        TYPE_MS,
        TYPE_NMS,
        TYPE_OMS,
    };

    enum ldpc_decoder_simd_t
    {
        SIMD_NONE,
        SIMD_SSE,
        SIMD_AVX,
    };

    LDPCDecoder *create_ldpc_decoder(LDPC_Code code,
                                     ldpc_decoder_type_t type,
                                     ldpc_decoder_simd_t simd,
                                     int meta = 0);

    void destroy_ldpc_decoder(LDPCDecoder *d);
}