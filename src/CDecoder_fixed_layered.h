#ifndef __CDecoder_layered_
#define __CDecoder_layered_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>

#include "CDecoder_fixed.h"

class CDecoder_fixed_layered : public CDecoder_fixed
{
protected:
    size_t cn_deg_max;
    // char *c_to_v_msgs;
    // char *var_nodes;
    int16_t *c_to_v_msgs;

public:
    CDecoder_fixed_layered(LDPC_Code code);
    virtual ~CDecoder_fixed_layered();
    virtual void decode(char var_nodes[], char Rprime_fix[], int nombre_iterations) = 0;
    virtual void decode(float var_nodes[], char Rprime_fix[], int nombre_iterations);
};

#endif
