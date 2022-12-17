#include <fstream>
#include "CDecoder_NMS_fixed_SSE.h"
#include "CDecoder_NMS_fixed_AVX.h"
#include "CDecoder_OMS_fixed_SSE.h"
#include "CDecoder_OMS_fixed_AVX.h"
#include "CDecoder_MS_fixed_layered.h"

int main(int /*argc*/, char *argv[])
{
    std::ifstream data_in(argv[1], std::ios::binary);
    std::ofstream data_out(argv[2], std::ios::binary);

    int8_t *input_buffer = new int8_t[8176 * 32];
    int8_t *output_buffer = new int8_t[8176 * 32];

    uint8_t *output_buffer2 = new uint8_t[8176 * 32];

    CDecoder_NMS_fixed_SSE *dec_new = new CDecoder_NMS_fixed_SSE(get_CCSDS_8176_1022_Code());
    dec_new->setFactor(8);
    dec_new->setVarRange(-127, 127);
    dec_new->setMsgRange(-127, 127);
    // dec_new->setSigmaChannel(0.37);

    CDecoder_NMS_fixed_AVX *dec_new2 = new CDecoder_NMS_fixed_AVX(get_CCSDS_8176_1022_Code());
    dec_new2->setFactor(8);
    dec_new2->setVarRange(-127, 127);
    dec_new2->setMsgRange(-127, 127);
    // dec_new->setSigmaChannel(0.37);

    CDecoder_OMS_fixed_SSE *dec_newo = new CDecoder_OMS_fixed_SSE(get_CCSDS_8176_1022_Code());
    dec_newo->setOffset(0);
    dec_newo->setVarRange(-127, 127);
    dec_newo->setMsgRange(-127, 127);
    // dec_new->setSigmaChannel(0.37);

    CDecoder_OMS_fixed_AVX *dec_new2o = new CDecoder_OMS_fixed_AVX(get_CCSDS_8176_1022_Code());
    dec_new2o->setOffset(0);
    dec_new2o->setVarRange(-127, 127);
    dec_new2o->setMsgRange(-127, 127);
    // dec_new->setSigmaChannel(0.37);

    CDecoder_MS_fixed_layered *dec_new3 = new CDecoder_MS_fixed_layered(get_CCSDS_8176_1022_Code());
    // dec_new3->setFactor(8);
    dec_new3->setVarRange(-127, 127);
    dec_new3->setMsgRange(-127, 127);
    // dec_new->setSigmaChannel(0.37);

    auto &dec = dec_new;

    int simd_factor = dec->getSIMDSize();

    printf("SIMD Size is %d\n", simd_factor);

    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)input_buffer, 8176 * simd_factor);

        dec->decode((char *)input_buffer, (char *)output_buffer, 10);

        for (int i = 0; i < 8176 * simd_factor; i++)
            output_buffer2[i / 8] = output_buffer2[i / 8] << 1 | (output_buffer[i] > 0);

        data_out.write((char *)output_buffer2, (8176 * simd_factor) / 8);
        data_out.flush();
    }
}