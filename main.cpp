#include <fstream>
#include "decoder_sse/decoder_nms_sse.h"
#include "decoder_avx/decoder_nms_avx.h"
#include "decoder_sse/decoder_oms_sse.h"
#include "decoder_avx/decoder_oms_avx.h"
#include "decoder_cpu/decoder_ms_layered.h"

int main(int /*argc*/, char *argv[])
{
    std::ifstream data_in(argv[1], std::ios::binary);
    std::ofstream data_out(argv[2], std::ios::binary);

    auto code = libldpc::get_CCSDS_8176_1022_Code(); // libldpc::get_ldpc_code_alist("/home/alan/Downloads/C2_Alist.a");

    int8_t *input_buffer = new int8_t[code._N * 32];
    int8_t *output_buffer = new int8_t[code._N * 32];

    uint8_t *output_buffer2 = new uint8_t[code._N * 32];

    libldpc::LDPCDecoder *dec = libldpc::create_ldpc_decoder(code, libldpc::TYPE_NMS, libldpc::SIMD_SSE);
    dec->setNumberOfIterations(10);

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
        data_out.flush();
    }
}