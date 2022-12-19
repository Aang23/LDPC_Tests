#include <fstream>
#include <chrono>
#include "libldpc.h"

int main(int /*argc*/, char *argv[])
{
    auto code = libldpc::get_ldpc_code_alist(argv[1]);

    libldpc::LDPCDecoder *dec = libldpc::create_ldpc_decoder(code, libldpc::TYPE_NMS, libldpc::SIMD_SSE);

    int simd_factor = dec->getSIMDSize();

    int8_t *input_buffer = new int8_t[code._N * simd_factor];
    int8_t *output_buffer = new int8_t[code._N * simd_factor];

    for (int i = 0; i < code._N * simd_factor; i++)
    {
        input_buffer[i] = 0;
        output_buffer[i] = 0;
    }

    printf("-------------------------\n");

    printf("SIMD Size is %d\n", simd_factor);

    auto dec_start = std::chrono::system_clock::now();

    int total_runs = 1e4;

    double tot_size = (double)code._N * (double)total_runs * (double)simd_factor;

    printf("Total Decode Size is %d Mbit\n", int(double(tot_size) / 1e6));

    for (int i = 0; i < total_runs; i++)
    {
        dec->decode((char *)input_buffer, (char *)output_buffer, 10);
    }

    auto dec_time = (std::chrono::system_clock::now() - dec_start);
    double time_sec = dec_time.count() / 1e9;
    double speed = ((tot_size) / time_sec) / 1e6;
    printf("Total time %f, speed %f Mbps\n", time_sec, speed);
}