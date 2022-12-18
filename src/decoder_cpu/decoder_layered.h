#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>

#include "ldpc_decoder.h"

namespace libldpc
{
    class LDPCDecoderLayered : public LDPCDecoder
    {
    protected:
        size_t cn_deg_max;
        // char *c_to_v_msgs;
        // char *var_nodes;
        int16_t *c_to_v_msgs;

    public:
        LDPCDecoderLayered(LDPC_Code code);
        virtual ~LDPCDecoderLayered();

        virtual void decode(char var_nodes[], char Rprime_fix[], int nombre_iterations) = 0;

        int getSIMDSize() { return 1; }
    };
}
