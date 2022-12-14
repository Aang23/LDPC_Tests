#include <fstream>
#include <chrono>
#include "libldpc.h"

int main(int /*argc*/, char *argv[])
{
    std::ifstream data_in(argv[2], std::ios::binary);
    std::ofstream data_out(argv[3], std::ios::binary);

    auto code = libldpc::get_ldpc_code_alist(argv[1]);

    int8_t *input_buffer = new int8_t[code._N * 32];
    int8_t *output_buffer = new int8_t[code._N * 32];
    uint8_t *output_buffer2 = new uint8_t[code._N * 32];

    libldpc::LDPCDecoder *dec = libldpc::create_ldpc_decoder(code, libldpc::TYPE_NMS, libldpc::SIMD_SSE);

    if (dec == nullptr)
    {
        printf("Decoder not available!\n");
        return 1;
    }

    int simd_factor = dec->getSIMDSize();

    printf("SIMD Size is %d\n", simd_factor);

    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)input_buffer, code._N * simd_factor);

        dec->decode((char *)input_buffer, (char *)output_buffer, 10);

        for (int i = 0; i < code._N * simd_factor; i++)
            output_buffer2[i / 8] = output_buffer2[i / 8] << 1 | (output_buffer[i] > 0);

        data_out.write((char *)output_buffer2, (code._N * simd_factor) / 8);
    }
}