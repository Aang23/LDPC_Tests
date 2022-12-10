#include <fstream>
#include "CDecoder_NMS_fixed_SSE.h"

int main(int /*argc*/, char *argv[])
{
    std::ifstream data_in(argv[1], std::ios::binary);
    std::ofstream data_out(argv[2], std::ios::binary);

    int8_t *input_buffer = new int8_t[8176 * 16];
    int8_t *output_buffer = new int8_t[8176 * 16];

    uint8_t *output_buffer2 = new uint8_t[8176 * 2];

    CDecoder_NMS_fixed_SSE *dec_new = new CDecoder_NMS_fixed_SSE(get_CCSDS_8176_1022_Code());
    dec_new->setFactor(8);
    dec_new->setVarRange(-127, 127);
    dec_new->setMsgRange(-127, 127);
    // dec_new->setSigmaChannel(0.37);

    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)input_buffer, 8176 * 16);

        dec_new->decode((char *)input_buffer, (char *)output_buffer, 10);

        for (int i = 0; i < 8176 * 16; i++)
            output_buffer2[i / 8] = output_buffer2[i / 8] << 1 | (output_buffer[i] > 0);

        data_out.write((char *)output_buffer2, 8176 * 2);
        data_out.flush();
    }
}