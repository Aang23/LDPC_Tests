#include "CDecoder_fixed_layered.h"

CDecoder_fixed_layered::CDecoder_fixed_layered(LDPC_Code code) : CDecoder(code)
{
    c_to_v_msgs = new int16_t[code.MESSAGE];
    cn_deg_max = code.DEGREES[0]; // TODO
}

CDecoder_fixed_layered::~CDecoder_fixed_layered()
{
    delete c_to_v_msgs;
}

void CDecoder_fixed_layered::decode(float var_nodes[], char Rprime_fix[], int nombre_iterations)
{
}