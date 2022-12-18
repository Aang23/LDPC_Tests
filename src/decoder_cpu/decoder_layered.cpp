#include "decoder_layered.h"

namespace libldpc
{
    LDPCDecoderLayered::LDPCDecoderLayered(LDPC_Code code) : LDPCDecoder(code)
    {
        c_to_v_msgs = new int16_t[code.MESSAGE];
        cn_deg_max = code.DEGREES[0]; // TODO
    }

    LDPCDecoderLayered::~LDPCDecoderLayered()
    {
        delete c_to_v_msgs;
    }
}