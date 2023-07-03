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
        int lambda = std::atoi(argv[3]);
        int k = std::atoi(argv[4]);
        int w = std::atoi(argv[5]);

        //t = 6;
        lambda = 1024;
        k = 128;
        w = 0;

        srand(time(NULL));

        Wesolowski vdf = Wesolowski();

        // Running the Setup phase of algorithm
        auto start = std::chrono::high_resolution_clock::now();
        vdf.setup(lambda, k);

        // Drawing a random input from the RSA group for each VDF
        std::vector<mpz_t> x_s(num_VDFs);
        std::vector<mpz_t> pi_s(num_VDFs);
        std::vector<mpz_t> alphas(num_VDFs);

        for (int i = 0; i < num_VDFs; i++) {
                mpz_init(x_s[i]);
                vdf.generate(x_s[i]);

                mpz_init(pi_s[i]);

                mpz_init(alphas[i]);
                vdf.generate_alpha(alphas[i], k);
        }

        auto finish = std::chrono::high_resolution_clock::now();
        auto setup_time = finish - start;

        mpz_t l;
        mpz_init(l);
        vdf.hash_prime(l, x_s[0]);

        vdf.batch_evaluate(l, pi_s, x_s, pow(2, t), alphas);
        
        // Drawing a random input from the RSA group
        mpz_t x;
        mpz_init(x);
        vdf.generate(x);

        // Generate num_VDFs random xs
        std::vector<mpz_t> xs(num_VDFs);
        for (int i = 0; i < num_VDFs; i++) {
                mpz_init(xs[i]);
                vdf.generate(xs[i]);
        }

        // Generate num_VDFs random alphas
        std::vector<mpz_t> alphas(num_VDFs);
        for (int i = 0; i < num_VDFs; i++) {
                mpz_init(alphas[i]);
                mpz_urandomb(alphas[i], rng, k); // Assuming rng is a random number generator and k is the number of bits in alpha
        }      


        //std::cout << "Random X: " << x;
        //Here is the evaluation part. We combine the evaluation and proof however there are two chrono for the separate phases.
        mpz_t l, pi;
        mpz_init(l);
        mpz_init(pi);
        vdf.evaluate(l, pi, x, pow(2, t));

        // Here we run the naive and optimized verifications
        bool result_verif;
        if(w==0)
        {
                result_verif = vdf.naive_verify(x, pow(2, t), l, pi);
        }
        else if(w == -1)
        {
                result_verif = vdf.parallel_verify(x, pow(2, t), l, pi);
        }
        else if(w == -2)
        {
                result_verif = vdf.parallel_diff_verify(x, pow(2, t), l, pi);
        }
        else
        {
                result_verif = vdf.optimized_verify(x, pow(2, t), l, pi, w);
        }
        std::ofstream file;

        //file.open("result/" + std::to_string(t) + "_" +
        //          std::to_string(lambda) + "_" + std::to_string(k)+ "_" + std::to_string(w) + ".csv",
        //          std::ofstream::out | std::ofstream::app);

        std::cout << "\n setup_time: " << vdf.setup_time.count() << ";" << "\n eval_time: " << vdf.eval_time.count() << ";"
             << "\n proof_time: " << vdf.proof_time.count() << ";" << "\n verify_time: " << vdf.verif_time.count() << "\n";
        //std::cout << vdf.verif_time.count() << std::endl;

        //file.close();
        return 0;
}
