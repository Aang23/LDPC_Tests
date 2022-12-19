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

#include "ldpc_decoder.h"

#include "decoder_sse/decoder_nms_sse.h"
#include "decoder_avx/decoder_nms_avx.h"

#include "decoder_sse/decoder_oms_sse.h"
#include "decoder_avx/decoder_oms_avx.h"

#include "decoder_cpu/decoder_ms_layered.h"

namespace libldpc
{
    LDPCDecoder::LDPCDecoder(LDPC_Code code) : code(code)
    {
    }

    LDPCDecoder::~LDPCDecoder()
    {
    }

    void LDPCDecoder::setSigmaChannel(float _SigB)
    {
        sigB = _SigB;
    }

    void LDPCDecoder::setVarRange(int min, int max)
    {
        vSAT_NEG_VAR = min;
        vSAT_POS_VAR = max;
    }

    void LDPCDecoder::setMsgRange(int min, int max)
    {
        vSAT_NEG_MSG = min;
        vSAT_POS_MSG = max;
    }

    LDPCDecoder *create_ldpc_decoder(LDPC_Code code,
                                     ldpc_decoder_type_t type,
                                     ldpc_decoder_simd_t simd,
                                     int meta)
    {
        if (type == TYPE_MS && simd == SIMD_NONE)
            return new LDPCDecoder_MS_Layered(code);
////////////////////////////////////////////////////
#if LIBLDPC_ENABLE_SSE
        else if (type == TYPE_NMS && simd == SIMD_SSE)
        {
            auto p = new libldpc::LDPCDecoder_NMS_SSE(code);
            if (meta == 0)
                p->setFactor(8);
            else
                p->setFactor(meta);
            return p;
        }
#endif
#if LIBLDPC_ENABLE_AVX
        else if (type == TYPE_NMS && simd == SIMD_AVX)
        {
            auto p = new libldpc::LDPCDecoder_NMS_AVX(code);
            if (meta == 0)
                p->setFactor(8);
            else
                p->setFactor(meta);
            return p;
        }
#endif
////////////////////////////////////////////////////
#if LIBLDPC_ENABLE_SSE
        else if (type == TYPE_OMS && simd == SIMD_SSE)
        {
            auto p = new libldpc::LDPCDecoder_OMS_SSE(code);
            if (meta == 0)
                p->setOffset(0);
            else
                p->setOffset(meta);
            return p;
        }
#endif
#if LIBLDPC_ENABLE_AVX
        else if (type == TYPE_OMS && simd == SIMD_AVX)
        {
            auto p = new libldpc::LDPCDecoder_OMS_AVX(code);
            if (meta == 0)
                p->setOffset(0);
            else
                p->setOffset(meta);
            return p;
        }
#endif
        ////////////////////////////////////////////////////
        else
            return nullptr;
    }

    void destroy_ldpc_decoder(LDPCDecoder *d)
    {
        delete d;
    }
}