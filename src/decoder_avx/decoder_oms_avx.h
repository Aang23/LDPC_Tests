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

#include "decoder_avx.h"

#if LIBLDPC_ENABLE_AVX
namespace libldpc
{
    class LDPCDecoder_OMS_AVX : public LDPCDecoderAVX
    {
    private:
        int offset;
        int nb_exec;
        int nb_saved_iters;

        __m256i **p_vn_adr;

        __m256i *tab_vContr;

    public:
        LDPCDecoder_OMS_AVX(LDPC_Code code);
        ~LDPCDecoder_OMS_AVX();
        void setOffset(int _offset);
        void decode(char var_nodes[], char Rprime_fix[], int nombre_iterations);

        bool decode_8bits(char var_nodes[], char Rprime_fix[], int nombre_iterations);
    };
}
#endif
