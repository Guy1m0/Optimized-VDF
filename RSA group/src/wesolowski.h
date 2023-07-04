#ifndef WESOLOWSKI_H
#define WESOLOWSKI_H

#include <gmp.h>
#include <vector>
#include <chrono>

#include "proof.h"

class Wesolowski {
public:
Wesolowski();
void hash_prime(mpz_t l, const mpz_t input);
void setup(int lambda, int k);
void generate(mpz_t& dest);
void generate_alpha(mpz_t alpha, int bit_size);
void evaluate(mpz_t l, mpz_t* pi, const mpz_t* x, mpz_t* y,
               long challenge);
void aggregate(std::vector<mpz_t*>& pi_s, const std::vector<mpz_t*>& x_s, 
                std::vector<mpz_t*>& y_s, mpz_t pi_agg, mpz_t x_agg, 
                mpz_t y_agg, std::vector<mpz_t*>& alphas, int batch_size);
bool parallel_verify(mpz_t x, long challenge, mpz_t l, mpz_t pi);
bool parallel_diff_verify(mpz_t x, long challenge, mpz_t l, mpz_t pi);
bool naive_verify(mpz_t x, mpz_t y, long challenge, mpz_t l, mpz_t pi);
bool optimized_verify(mpz_t x, long challenge, mpz_t l, mpz_t pi, int w);
std::chrono::duration<double> setup_time;
std::chrono::duration<double> eval_time;
std::chrono::duration<double> proof_time;
std::chrono::duration<double> verif_time;
std::chrono::duration<double> verif_time_opti;

private:
mpz_t y_saved;
mpz_t N;
mpz_t p;
mpz_t q;
int k;
int lambda;
mpz_t challenge;

gmp_randstate_t rstate;

void HashG(mpz_t& dest, mpz_t hashed);


void unit_test();

int bit_length;
};

#endif
