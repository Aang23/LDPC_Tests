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

    auto code = libldpc::get_CCSDS_8176_1022_Code(); // get_ldpc_code_alist("/home/alan/Downloads/C2_Alist.a");

    int8_t *input_buffer = new int8_t[code._N * 32];
    int8_t *output_buffer = new int8_t[code._N * 32];

    uint8_t *output_buffer2 = new uint8_t[code._N * 32];

    libldpc::LDPCDecoder_NMS_SSE *dec_new = new libldpc::LDPCDecoder_NMS_SSE(code);
    dec_new->setFactor(8);
    dec_new->setVarRange(-127, 127);
    dec_new->setMsgRange(-127, 127);
    // dec_new->setSigmaChannel(0.37);

    libldpc::LDPCDecoder_NMS_AVX *dec_new2 = new libldpc::LDPCDecoder_NMS_AVX(code);
    dec_new2->setFactor(8);
    dec_new2->setVarRange(-127, 127);
    dec_new2->setMsgRange(-127, 127);
    // dec_new->setSigmaChannel(0.37);

    libldpc::LDPCDecoder_OMS_SSE *dec_newo = new libldpc::LDPCDecoder_OMS_SSE(code);
    dec_newo->setOffset(0);
    dec_newo->setVarRange(-127, 127);
    dec_newo->setMsgRange(-127, 127);
    // dec_new->setSigmaChannel(0.37);

    libldpc::LDPCDecoder_OMS_AVX *dec_new2o = new libldpc::LDPCDecoder_OMS_AVX(code);
    dec_new2o->setOffset(0);
    dec_new2o->setVarRange(-127, 127);
    dec_new2o->setMsgRange(-127, 127);
    // dec_new->setSigmaChannel(0.37);

    libldpc::LDPCDecoder_MS_Layered *dec_new3 = new libldpc::LDPCDecoder_MS_Layered(code);
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
        data_in.read((char *)input_buffer, code._N * simd_factor);

        dec->decode((char *)input_buffer, (char *)output_buffer, 10);

        for (int i = 0; i < code._N * simd_factor; i++)
            output_buffer2[i / 8] = output_buffer2[i / 8] << 1 | (output_buffer[i] > 0);

        data_out.write((char *)output_buffer2, (code._N * simd_factor) / 8);
        data_out.flush();
    }
}