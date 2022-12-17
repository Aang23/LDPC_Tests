#ifndef CDECODER_MS_FIXED_LAYERED_H
#define CDECODER_MS_FIXED_LAYERED_H

#include "CDecoder_fixed_layered.h"

class CDecoder_MS_fixed_layered : public CDecoder_fixed_layered
{
protected:
    int64_t **p_vn_addr; // From a CN perspective, the addresses of all neighbouring VNs
    int64_t *var_nodes;

public:
    CDecoder_MS_fixed_layered(LDPC_Code code);
    ~CDecoder_MS_fixed_layered();
    void decode(char var_nodes[], char Rprime_fix[], int nombre_iterations);
};

#endif