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

#include "decoder_nms_sse.h"
#include "tools.h"

#if LIBLDPC_ENABLE_SSE

#include "fixed_SSE_Functionstruc.h"

#define TYPE __m128i

#define VECTOR_LOAD(ptr) (_mm_load_si128(ptr))
#define VECTOR_UNCACHED_LOAD(ptr) (_mm_stream_load_si128(ptr))
#define VECTOR_STORE(ptr, v) (_mm_store_si128(ptr, v))
#define VECTOR_ADD(a, b) (_mm_adds_epi8(a, b))
#define VECTOR_SBU(a, b) (_mm_subs_epu8(a, b))
#define VECTOR_SUB(a, b) (_mm_subs_epi8(a, b))
#define VECTOR_ABS(a) (_mm_abs_epi8(a))
#define VECTOR_MAX(a, b) (_mm_max_epi8(a, b))
#define VECTOR_MIN(a, b) (_mm_min_epi8(a, b))
#define VECTOR_XOR(a, b) (_mm_xor_si128(a, b))
#define VECTOR_OR(a, b) (_mm_or_si128(a, b))
#define VECTOR_AND(a, b) (_mm_and_si128(a, b))
#define VECTOR_ANDNOT(a, b) (_mm_andnot_si128(a, b))
#define VECTOR_MIN_1(a, min1) (VECTOR_MIN(a, min1))
#define VECTOR_SIGN(a, b) (_mm_sign_epi8(a, b))
#define VECTOR_EQUAL(a, b) (_mm_cmpeq_epi8(a, b))
#define VECTOR_ZERO (_mm_setzero_si128())
#define VECTOR_SET1(a) (_mm_set1_epi8(a))
#define VECTOR_SET2(a) (_mm_set1_epi16(a))

#define VECTOR_PACK(hi, lo) (_mm_packs_epi16(lo, hi))
#define VECTOR_UNPACK_HIGH(a) (_mm_unpackhi_epi8(a, VECTOR_ZERO))
#define VECTOR_UNPACK_LOW(a) (_mm_unpacklo_epi8(a, VECTOR_ZERO))
#define VECTOR_MUL(a, b) (_mm_mullo_epi16(a, b))
#define VECTOR_DIV(a, b) (_mm_div_epi16(a, b))
#define VECTOR_DIV32(a) (_mm_srli_epi16(a, 5))

#define VECTOR_MIN_2(val, old_min1, min2) \
    (VECTOR_MIN(min2, VECTOR_MAX(val, old_min1)))

#define VECTOR_VAR_SATURATE(a) \
    (VECTOR_MAX(VECTOR_MIN(a, max_var), min_var))

#define VECTOR_SATURATE(a, max, min) \
    (VECTOR_MAX(VECTOR_MIN(a, max), min))

#define VECTOR_SUB_AND_SATURATE_VAR_8bits(a, b, min) \
    (VECTOR_MAX(VECTOR_SUB(a, b), min)) // ON DOIT CONSERVER LA SATURATION MIN A CAUSE DE -128

#define VECTOR_ADD_AND_SATURATE_VAR_8bits(a, b, min) \
    (VECTOR_MAX(VECTOR_ADD(a, b), min)) // ON DOIT CONSERVER LA SATURATION MIN A CAUSE DE -128

#define VECTOR_SUB_AND_SATURATE_VAR(a, b, max, min) \
    (VECTOR_SATURATE(VECTOR_SUB(a, b), max, min))

#define VECTOR_ADD_AND_SATURATE_VAR(a, b, max, min) \
    (VECTOR_SATURATE(VECTOR_ADD(a, b), max, min))

#define VECTOR_invSIGN2(val, sig) \
    (VECTOR_SIGN(val, sig))

namespace libldpc
{
    inline TYPE VECTOR_GET_SIGN_BIT(TYPE a, TYPE m)
    {
        TYPE b = VECTOR_AND(a, m);
        return b;
    }

    inline TYPE VECTOR_CMOV(TYPE a, TYPE b, TYPE c, TYPE d)
    {
        TYPE z = VECTOR_EQUAL(a, b);
        //    return _mm_blendv_epi8( d, c, z );
        TYPE g = VECTOR_AND(c, z);
        TYPE h = VECTOR_ANDNOT(z, d);
        return VECTOR_OR(g, h);
    }

#define PETIT 1
#define MANUAL_PREFETCH 1

    LDPCDecoder_NMS_SSE::LDPCDecoder_NMS_SSE(LDPC_Code code) : LDPCDecoderSSE(code)
    {
#if PETIT == 1
        p_vn_adr = new TYPE *[code._M];
        for (int i = 0; i < code._M; i++)
        {
            p_vn_adr[i] = &var_nodes[code.PosNoeudsVariable[i]];
        }
#endif

        tab_vContr = new TYPE[code.DEGREES[0]];
    }

    LDPCDecoder_NMS_SSE::~LDPCDecoder_NMS_SSE()
    {
#if PETIT == 1
        delete p_vn_adr;
#endif

        delete[] tab_vContr;
    }

    void LDPCDecoder_NMS_SSE::setFactor(int _factor)
    {
        factor_1 = _factor;
        factor_2 = _factor;
    }

    void LDPCDecoder_NMS_SSE::decode(char Intrinsic_fix[], char Rprime_fix[], int nombre_iterations)
    {
        if (vSAT_POS_VAR == 127)
            decode_8bits(Intrinsic_fix, Rprime_fix, nombre_iterations);
        else
            exit(0); // decode_generic(Intrinsic_fix, Rprime_fix, nombre_iterations);
    }

    void LDPCDecoder_NMS_SSE::decode_8bits(char Intrinsic_fix[], char Rprime_fix[], int nombre_iterations)
    {
        const TYPE zero = VECTOR_ZERO;
        for (int i = 0; i < code.MESSAGE; i++)
        {
            var_mesgs[i] = zero;
        }

        if (code.NOEUD % 16 == 0)
        {
            uchar_transpose_sse((TYPE *)Intrinsic_fix, (TYPE *)var_nodes, code.NOEUD);
        }
        else
        {
            char *ptrVar = (char *)var_nodes;
            for (int i = 0; i < code.NOEUD; i++)
            {
                for (int z = 0; z < 16; z++)
                {
                    ptrVar[16 * i + z] = Intrinsic_fix[z * code.NOEUD + i];
                }
            }
        }

        while (nombre_iterations--)
        {
            TYPE *p_msg1r = var_mesgs;
            TYPE *p_msg1w = var_mesgs;
#if PETIT == 1
            TYPE **p_indice_nod1 = p_vn_adr;
            TYPE **p_indice_nod2 = p_vn_adr;
#else
            const unsigned short *p_indice_nod1 = PosNoeudsVariable;
            const unsigned short *p_indice_nod2 = PosNoeudsVariable;
#endif

            const TYPE min_var = VECTOR_SET1(vSAT_NEG_VAR);
            const TYPE max_msg = VECTOR_SET1(vSAT_POS_MSG);

            for (int i = 0; i < code.DEGREES_COMPUTATIONS[0]; i++)
            {
                // TYPE tab_vContr[code.DEGREES[0]];
                TYPE sign = VECTOR_ZERO;
                TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
                TYPE min2 = min1;

                const unsigned char sign8 = 0x80;
                const unsigned char isign8 = 0xC0;
                const unsigned char isign8b = 0x40;

                const TYPE msign8 = VECTOR_SET1(sign8);
                const TYPE misign8 = VECTOR_SET1(isign8);
                const TYPE misign8b = VECTOR_SET1(isign8b);

#if PETIT == 1
#if MANUAL_PREFETCH == 1
                _mm_prefetch((const char *)(p_indice_nod1[code.DEGREES[0]]), _MM_HINT_T0);
                _mm_prefetch((const char *)(&p_msg1r[code.DEGREES[0]]), _MM_HINT_T0);
#endif
#endif

                // #pragma unroll(DEG_1)
                for (int j = 0; j < code.DEGREES[0]; j++)
                {
#if PETIT == 1
                    TYPE vNoeud = VECTOR_LOAD(*p_indice_nod1);
#else
                    TYPE vNoeud = VECTOR_LOAD(&var_nodes[(*p_indice_nod1)]);
#endif
                    TYPE vMessg = VECTOR_LOAD(p_msg1r);
                    TYPE vContr = VECTOR_SUB_AND_SATURATE_VAR_8bits(vNoeud, vMessg, min_var);
                    TYPE cSign = VECTOR_GET_SIGN_BIT(vContr, msign8);
                    sign = VECTOR_XOR(sign, cSign);
                    TYPE vAbs = VECTOR_MIN(VECTOR_ABS(vContr), max_msg);
                    tab_vContr[j] = vContr;
                    TYPE vTemp = min1;
                    min1 = VECTOR_MIN_1(vAbs, min1);
                    min2 = VECTOR_MIN_2(vAbs, vTemp, min2);
                    p_indice_nod1 += 1;
                    p_msg1r += 1;
                }

                TYPE norm_1 = VECTOR_SET2(factor_1);
                TYPE h_min1 = VECTOR_UNPACK_HIGH(min1);
                TYPE l_min1 = VECTOR_UNPACK_LOW(min1);
                TYPE h_cste_2 = VECTOR_MUL(h_min1, norm_1);
                h_cste_2 = VECTOR_DIV32(h_cste_2);
                TYPE l_cste_2 = VECTOR_MUL(l_min1, norm_1);
                l_cste_2 = VECTOR_DIV32(l_cste_2);
                TYPE cste_2 = VECTOR_PACK(h_cste_2, l_cste_2);

                TYPE norm_2 = VECTOR_SET2(factor_2);
                TYPE h_min2 = VECTOR_UNPACK_HIGH(min2);
                TYPE l_min2 = VECTOR_UNPACK_LOW(min2);
                TYPE h_cste_1 = VECTOR_MUL(h_min2, norm_2);
                h_cste_1 = VECTOR_DIV32(h_cste_1);
                TYPE l_cste_1 = VECTOR_MUL(l_min2, norm_2);
                l_cste_1 = VECTOR_DIV32(l_cste_1);
                TYPE cste_1 = VECTOR_PACK(h_cste_1, l_cste_1);

#if PETIT == 1
#if MANUAL_PREFETCH == 1
                for (int j = 0; j < code.DEGREES[0]; j++)
                {
                    _mm_prefetch((const char *)(p_indice_nod1[j]), _MM_HINT_T0);
                }
                _mm_prefetch((const char *)(p_indice_nod1[code.DEGREES[0]]), _MM_HINT_T0);
#endif
#endif

                if ((code.DEGREES[0] & 0x01) == 1)
                    sign = VECTOR_XOR(sign, misign8);
                else
                    sign = VECTOR_XOR(sign, misign8b);

                // #pragma unroll(DEG_1)
                for (int j = 0; j < code.DEGREES[0]; j++)
                {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs = VECTOR_MIN(VECTOR_ABS(vContr), max_msg);
                    TYPE vRes = VECTOR_CMOV(vAbs, min1, cste_1, cste_2);
                    TYPE vSig = VECTOR_XOR(sign, VECTOR_GET_SIGN_BIT(vContr, msign8));
                    TYPE v2St = VECTOR_invSIGN2(vRes, vSig);
                    TYPE v2Sr = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                    VECTOR_STORE(p_msg1w, v2St);
#if PETIT == 1
                    VECTOR_STORE(*p_indice_nod2, v2Sr);
#else
                    VECTOR_STORE(&var_nodes[(*p_indice_nod2)], v2Sr);
#endif
                    p_msg1w += 1;
                    p_indice_nod2 += 1;
                }
#if PETIT == 1
#if MANUAL_PREFETCH == 1
                _mm_prefetch((const char *)(*p_indice_nod2), _MM_HINT_T0);
#endif
#endif
            }

            ////////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////

            for (int degn = 1; degn < code.NB_DEGRES; degn++)
            {
                for (int i = 0; i < code.DEGREES_COMPUTATIONS[degn]; i++)
                {
                    const unsigned char sign8 = 0x80;
                    const unsigned char isign8 = 0xC0;
                    const unsigned char isign8b = 0x40;
                    const TYPE msign8 = VECTOR_SET1(sign8);
                    const TYPE misign8 = VECTOR_SET1(isign8);
                    const TYPE misign8b = VECTOR_SET1(isign8b);

                    // TYPE tab_vContr[code.DEGREES[degn]];
                    TYPE sign = VECTOR_ZERO;
                    TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
                    TYPE min2 = min1;

                    // #pragma unroll(DEG_2)
                    for (int j = 0; j < code.DEGREES[degn]; j++)
                    {
#if PETIT == 1
                        TYPE vNoeud = VECTOR_LOAD(*p_indice_nod1);
#else
                        TYPE vNoeud = VECTOR_LOAD(&var_nodes[(*p_indice_nod1)]);
#endif
                        TYPE vMessg = VECTOR_LOAD(p_msg1r);
                        TYPE vContr = VECTOR_SUB_AND_SATURATE_VAR_8bits(vNoeud, vMessg, min_var);
                        TYPE cSign = VECTOR_GET_SIGN_BIT(vContr, msign8);
                        sign = VECTOR_XOR(sign, cSign);
                        TYPE vAbs = VECTOR_MIN(VECTOR_ABS(vContr), max_msg);
                        tab_vContr[j] = vContr;
                        TYPE vTemp = min1;
                        min1 = VECTOR_MIN_1(vAbs, min1);
                        min2 = VECTOR_MIN_2(vAbs, vTemp, min2);
                        p_indice_nod1 += 1;
                        p_msg1r += 1;
                    }

#if PETIT == 1
                    for (int j = 0; j < code.DEGREES[degn]; j++)
                    {
                        _mm_prefetch((const char *)(p_indice_nod1[j]), _MM_HINT_T0);
                    }
                    _mm_prefetch((const char *)(p_indice_nod1[code.DEGREES[degn]]), _MM_HINT_T0);
#endif

                    TYPE norm_1 = VECTOR_SET2(factor_1);
                    TYPE h_min1 = VECTOR_UNPACK_HIGH(min1);
                    TYPE l_min1 = VECTOR_UNPACK_LOW(min1);
                    TYPE h_cste_2 = VECTOR_MUL(h_min1, norm_1);
                    h_cste_2 = VECTOR_DIV32(h_cste_2);
                    TYPE l_cste_2 = VECTOR_MUL(l_min1, norm_1);
                    l_cste_2 = VECTOR_DIV32(l_cste_2);
                    TYPE cste_2 = VECTOR_PACK(h_cste_2, l_cste_2);

                    TYPE norm_2 = VECTOR_SET2(factor_2);
                    TYPE h_min2 = VECTOR_UNPACK_HIGH(min2);
                    TYPE l_min2 = VECTOR_UNPACK_LOW(min2);
                    TYPE h_cste_1 = VECTOR_MUL(h_min2, norm_2);
                    h_cste_1 = VECTOR_DIV32(h_cste_1);
                    TYPE l_cste_1 = VECTOR_MUL(l_min2, norm_2);
                    l_cste_1 = VECTOR_DIV32(l_cste_1);
                    TYPE cste_1 = VECTOR_PACK(h_cste_1, l_cste_1);

                    /*#if (DEG_2 & 0x01) == 1
                                        sign = VECTOR_XOR(sign, misign8);
                    #else
                                        sign = VECTOR_XOR(sign, misign8b);
                    #endif*/

                    if ((code.DEGREES[degn] & 0x01) == 1)
                        sign = VECTOR_XOR(sign, misign8);
                    else
                        sign = VECTOR_XOR(sign, misign8b);

                    // #pragma unroll(DEG_2)
                    for (int j = 0; j < code.DEGREES[degn]; j++)
                    {
                        TYPE vContr = tab_vContr[j];
                        TYPE vAbs = VECTOR_MIN(VECTOR_ABS(vContr), max_msg);
                        TYPE vRes = VECTOR_CMOV(vAbs, min1, cste_1, cste_2);
                        TYPE vSig = VECTOR_XOR(sign, VECTOR_GET_SIGN_BIT(vContr, msign8));
                        TYPE v2St = VECTOR_invSIGN2(vRes, vSig);
                        TYPE v2Sr = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                        VECTOR_STORE(p_msg1w, v2St);
#if PETIT == 1
                        VECTOR_STORE(*p_indice_nod2, v2Sr);
#else
                        VECTOR_STORE(&var_nodes[(*p_indice_nod2)], v2Sr);
#endif
                        p_msg1w += 1;
                        p_indice_nod2 += 1;
                    }
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////////
        //
        // ON REMET EN FORME LES DONNEES DE SORTIE POUR LA SUITE DU PROCESS
        //
        if (code.NOEUD % 16 == 0)
        {
            uchar_itranspose_sse((TYPE *)var_nodes, (TYPE *)Rprime_fix, code.NOEUD);
        }
        else
        {
            char *ptr = (char *)var_nodes;
            for (int i = 0; i < code.NOEUD; i += 1)
            {
                for (int j = 0; j < 16; j += 1)
                {
                    Rprime_fix[j * code.NOEUD + i] = (ptr[16 * i + j] > 0);
                }
            }
        }
        //
        ////////////////////////////////////////////////////////////////////////////
    }
}
#endif