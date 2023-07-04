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
        int t = std::atoi(argv[1]);
        int num_VDFs = std::atoi(argv[2]);
        //int lambda = std::atoi(argv[3]);
        //int k = std::atoi(argv[4]);
        //int w = std::atoi(argv[5]);

        //int t = 15;
        //int num_VDFs = 10;
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
        std::vector<mpz_t*> y_s(num_VDFs);
        std::vector<mpz_t*> pi_s(num_VDFs);
        std::vector<mpz_t*> alphas(num_VDFs);
        

        for (int i = 0; i < num_VDFs; i++) {
                // Allocate space for each mpz_t
                x_s[i] = (mpz_t*) malloc(sizeof(mpz_t));
                y_s[i] = (mpz_t*) malloc(sizeof(mpz_t));
                pi_s[i] = (mpz_t*) malloc(sizeof(mpz_t));
                alphas[i] = (mpz_t*) malloc(sizeof(mpz_t));

                mpz_init(*x_s[i]);
                mpz_init(*y_s[i]);
                mpz_init(*pi_s[i]);
                mpz_init(*alphas[i]);

                // Generate x and alpha
                vdf.generate(*x_s[i]);
                vdf.generate_alpha(*alphas[i], k);
        }
        
        mpz_t l, pi_agg, x_agg, y_agg;
        mpz_init(l);
        vdf.hash_prime(l, *x_s[0]);
        mpz_set_ui(x_agg, 1);
        mpz_set_ui(pi_agg, 1);
        mpz_set_ui(y_agg, 1);

        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> setup_time = finish - start;

        std::cout << "Time cost for setup: " << setup_time.count() << std::endl;

        // Batch evaulate
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < num_VDFs; ++i){
                vdf.evaluate(l, pi_s[i], x_s[i], y_s[i], pow(2,t));
        }
        finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> evulate_time = finish - start;
        std::cout << "Time cost for evulate: " << evulate_time.count() << std::endl;


        // Aggregate
        start = std::chrono::high_resolution_clock::now();
        vdf.aggregate(pi_s, x_s, y_s, pi_agg, x_agg, y_agg, alphas, num_VDFs);
        finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> aggregate_time = finish - start;
        std::cout << "Time cost for aggregate: " << aggregate_time.count() << std::endl;   

        // Verify VDFs individually.
        auto start_individual = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < num_VDFs; ++i) {
                if (!vdf.naive_verify(*x_s[i], *y_s[i], pow(2, t), l, *pi_s[i])) {
                std::cout << "Individual verification failed.\n";
                return 1;
                }
        }
        auto end_individual = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_individual = end_individual - start_individual;
        std::cout << "Time cost for individually verification: " << diff_individual.count() << std::endl;

        // Verify VDFs in a batch.
        auto start_batch = std::chrono::high_resolution_clock::now();
        if (!vdf.naive_verify(x_agg, y_agg, pow(2,t), l, pi_agg)){
                std::cout << "Batch verification failed.\n";
                return 1;
        }        
        auto end_batch = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_batch = end_batch - start_batch;
        std::cout << "Time cost for aggregation and batch verification: " << aggregate_time.count() + diff_batch.count() << std::endl;

        for (int i = 0; i < num_VDFs; i++) {
                mpz_clear(*x_s[i]);
                mpz_clear(*pi_s[i]);
                mpz_clear(*alphas[i]);
                mpz_clear(*y_s[i]);

                // Don't forget to free the allocated space to avoid memory leaks
                free(x_s[i]);
                free(pi_s[i]);
                free(alphas[i]);
                free(y_s[i]);
        }
        
        // //std::cout << "\n setup_time: " << vdf.setup_time.count() << ";" << "\n eval_time: " << vdf.eval_time.count() << ";"
        // //     << "\n proof_time: " << vdf.proof_time.count() << ";" << "\n verify_time: " << vdf.verif_time.count() << "\n";

        return 0;
}
