#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <gmp.h>


#include "wesolowski.h"

// argv arguments :
//    t : log2 of difficulty (must be an integer)
//    lambda : length of modulus
//    k : bit-level security of hashing function
//    Size of Lenstra window w
int main(int argc, char *argv[]) {

        // Argument parsing
        //int t = std::atoi(argv[1]);
        //int num_VDFs = std::atoi(argv[2]);
        //int lambda = std::atoi(argv[3]);
        //int k = std::atoi(argv[4]);
        //int w = std::atoi(argv[5]);

        int t = 10;
        int num_VDFs = 3;
        int lambda = 1024;
        int k = 128;
        //w = 0;

        srand(time(NULL));

        Wesolowski vdf = Wesolowski();

        // Running the Setup phase of algorithm
        auto start = std::chrono::high_resolution_clock::now();
        vdf.setup(lambda, k);

        // Initialize vector of pointers to mpz_t
        std::vector<mpz_t*> x_s(num_VDFs);
        std::vector<mpz_t*> pi_s(num_VDFs);
        std::vector<mpz_t*> alphas(num_VDFs);

        for (int i = 0; i < num_VDFs; i++) {
                // Allocate space for each mpz_t
                x_s[i] = (mpz_t*) malloc(sizeof(mpz_t));
                pi_s[i] = (mpz_t*) malloc(sizeof(mpz_t));
                alphas[i] = (mpz_t*) malloc(sizeof(mpz_t));

                mpz_init(*x_s[i]);
                mpz_init(*pi_s[i]);
                mpz_init(*alphas[i]);

                // Generate x and alpha
                vdf.generate(*x_s[i]);
                vdf.generate_alpha(*alphas[i], k);
        }

        auto finish = std::chrono::high_resolution_clock::now();
        auto setup_time = finish - start;

        mpz_t l, pi_agg, x_agg;
        mpz_init(l);
        vdf.hash_prime(l, *x_s[0]);
        std::cout << "l: " << l << "\n one of x: " << &x_s[3] ;

        start = std::chrono::high_resolution_clock::now();
        vdf.batch_evaluate(l, pi_s, x_s, pow(2, t), pi_agg, x_agg, alphas, num_VDFs);
        finish = std::chrono::high_resolution_clock::now();
        auto batch_evulate_time = finish - start;

        // Verify VDFs individually.
        // auto start_individual = std::chrono::high_resolution_clock::now();
        // for (int i = 0; i < num_VDFs; ++i) {
        //         if (!vdf.naive_verify(*x_s[i], pow(2, t), l, *pi_s[i])) {
        //         std::cout << "Individual verification failed.\n";
        //         return 1;
        //         }
        // }
        // auto end_individual = std::chrono::high_resolution_clock::now();
        // std::chrono::duration<double> diff_individual = end_individual - start_individual;

        // for (int i = 0; i < num_VDFs; i++) {
        //         mpz_clear(*x_s[i]);
        //         mpz_clear(*pi_s[i]);
        //         mpz_clear(*alphas[i]);

        //         // Don't forget to free the allocated space to avoid memory leaks
        //         free(x_s[i]);
        //         free(pi_s[i]);
        //         free(alphas[i]);
        // }


        // // Verify VDFs in a batch.
        // auto start_batch = std::chrono::high_resolution_clock::now();
        // if (!vdf.naive_verify(x_agg, pow(2,t), l, pi_agg)){
        //         std::cout << "Batch verification failed.\n";
        //         return 1;
        // }        
        // auto end_batch = std::chrono::high_resolution_clock::now();
        // std::chrono::duration<double> diff_batch = end_batch - start_batch;

        // //std::cout << "\n setup_time: " << vdf.setup_time.count() << ";" << "\n eval_time: " << vdf.eval_time.count() << ";"
        // //     << "\n proof_time: " << vdf.proof_time.count() << ";" << "\n verify_time: " << vdf.verif_time.count() << "\n";

        return 0;
}
