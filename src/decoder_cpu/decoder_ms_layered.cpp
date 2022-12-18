#include "decoder_ms_layered.h"

namespace libldpc
{
    LDPCDecoder_MS_Layered::LDPCDecoder_MS_Layered(LDPC_Code code) : LDPCDecoderLayered(code)
    {
        p_vn_addr = new int64_t *[code.MESSAGE];
        var_nodes = new int64_t[code.NOEUD];

        for (size_t i = 0; i < code.MESSAGE; i++)
        {
            p_vn_addr[i] = &var_nodes[code.PosNoeudsVariable[i]];
        }
    }

    LDPCDecoder_MS_Layered::~LDPCDecoder_MS_Layered()
    {
        delete p_vn_addr;
        delete[] var_nodes;
    }

    void LDPCDecoder_MS_Layered::decode(
        char var_nodes[],
        char Rprime_fix[],
        int nombre_iterations)
    {
        size_t cn_deg = cn_deg_max;
        int64_t v_to_c_msgs[cn_deg_max];
        size_t cn_offset;
        int64_t sign;
        int64_t minLLR;
        int64_t abs_msg, msg;
        int64_t new_msg;

        for (size_t i = 0; i < code.NOEUD; i++)
        {
            LDPCDecoder_MS_Layered::var_nodes[i] = (int64_t)var_nodes[i];
        }

        for (size_t i = 0; i < code.MESSAGE; i++)
        {
            c_to_v_msgs[i] = 0;
        }

        while (nombre_iterations--)
        {
            /* For all CNs */
            for (size_t cn_idx = 0; cn_idx < code._K; cn_idx++)
            {
                cn_offset = cn_idx * cn_deg_max;

                /* Nasty hack for now */
                cn_deg = cn_idx == code._K - 1 ? code.DEGREES[1] : cn_deg_max;

                /* Collect all messages coming from VNs adjacent to current CN */
                for (size_t vn_idx = 0; vn_idx < cn_deg; vn_idx++)
                {
                    v_to_c_msgs[vn_idx] =
                        *p_vn_addr[cn_offset + vn_idx] -
                        c_to_v_msgs[cn_offset + vn_idx];
                }

                /* Compute new estimation directed to VNs,
                 * store it for the next iteration, and update estimation on VNs. */
                for (size_t vn_idx = 0; vn_idx < cn_deg; vn_idx++)
                {
                    sign = -1;
                    minLLR = INT64_MAX;

                    for (size_t i = 0; i < cn_deg; i++)
                    {
                        if (i == vn_idx)
                            continue;

                        msg = v_to_c_msgs[i];
                        sign *= msg >= 0 ? -1 : 1;
                        abs_msg = msg >= 0 ? msg : -msg;
                        minLLR = minLLR > abs_msg ? abs_msg : minLLR;
                    }

                    new_msg = sign * minLLR;
                    c_to_v_msgs[cn_offset + vn_idx] = new_msg;
                    *p_vn_addr[cn_offset + vn_idx] = new_msg + v_to_c_msgs[vn_idx];
                }
            }
        }

        /* Hard decision */
        for (size_t i = 0; i < code.NOEUD; i++)
        {
            Rprime_fix[i] = LDPCDecoder_MS_Layered::var_nodes[i] >= 0 ? 1 : 0;
        }
    }
}