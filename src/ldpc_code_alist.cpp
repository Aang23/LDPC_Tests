#include "./ldpc_code.h"
#include "alist/alist.h"
#include <algorithm>
#include <map>
#include <fstream>

namespace libldpc
{
    LDPC_Code get_ldpc_code_alist(std::string alist_path)
    {
        std::ifstream data_in(alist_path, std::ios::binary);

        auto v = tools::AList::read(data_in);

        int code_n = v.get_n_rows();
        int code_k = v.get_n_cols();
        int code_m = v.get_n_connections();

        std::vector<int> degrees;
        std::map<int, int> degrees_comps;
        std::map<int, std::vector<int>> degrees_posnodes;
        for (size_t cols = 0; cols < v.get_n_cols(); cols++)
        {
            int deg = 0;
            std::vector<int> deg_pos;
            for (size_t rows = 0; rows < v.get_n_rows(); rows++)
            {
                if (v.at(rows, cols))
                {
                    deg++;
                    deg_pos.push_back(rows);
                }
            }

            if (std::find(degrees.begin(), degrees.end(), deg) == degrees.end())
                degrees.push_back(deg);

            if (degrees_comps.count(deg) == 0)
                degrees_comps.insert({deg, 0});
            degrees_comps[deg]++;

            if (degrees_posnodes.count(deg) == 0)
                degrees_posnodes.insert({deg, {}});
            degrees_posnodes[deg].insert(degrees_posnodes[deg].end(), deg_pos.begin(), deg_pos.end());
        }
        std::sort(degrees.begin(), degrees.end(), [](const int &i1, const int &i2)
                  { return i1 > i2; });

        printf("Code N %d \n", code_n);
        printf("Code K %d \n", code_k);
        printf("Code M %d \n", code_m);

        printf("Degrees : ");
        for (int d : degrees)
            printf("%d ", d);
        printf("\n");

        std::vector<int> degrees_compus;
        printf("Degrees computations : ");
        for (int d : degrees)
        {
            printf("%d ", degrees_comps[d]);
            degrees_compus.push_back(degrees_comps[d]);
        }
        printf("\n");

        std::vector<unsigned short> final_posvarnodes;
        for (int d : degrees)
            final_posvarnodes.insert(final_posvarnodes.end(), degrees_posnodes[d].begin(), degrees_posnodes[d].end());

#if 0
    int i = 0;
    for (int f : final_posvarnodes)
    {
        if (i++ % degrees[0] == 0)
            printf("\n");
        printf("%d ", f);
    }
#endif

        /// INIT CODE
        LDPC_Code code;

        code._N = code_n; // Nombre de Variables
        code._K = code_k; // Nombre de Checks
        code._M = code_m; // Nombre de Messages

        code.DEGREES = degrees;

        code.DEGREES_COMPUTATIONS = degrees_compus;

        code.setup();

        code.PosNoeudsVariable = final_posvarnodes;

        return code;
    }
}