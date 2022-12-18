#pragma once

#include "decoder_layered.h"

namespace libldpc
{
    class LDPCDecoder_MS_Layered : public LDPCDecoderLayered
    {
    protected:
        int64_t **p_vn_addr; // From a CN perspective, the addresses of all neighbouring VNs
        int64_t *var_nodes;

    public:
        LDPCDecoder_MS_Layered(LDPC_Code code);
        ~LDPCDecoder_MS_Layered();
        void decode(char var_nodes[], char Rprime_fix[], int nombre_iterations);
    };
}