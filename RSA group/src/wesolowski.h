#ifndef WESOLOWSKI_H
#define WESOLOWSKI_H

#include <gmp.h>
#include <vector>
#include <chrono>

#include "proof.h"

class Wesolowski {
public:
Wesolowski();

void setup(int lambda, int k);
void generate(mpz_t& dest);
void generate_alpha(mpz_t alpha, int bit_size);
Proof evaluate(mpz_t l, mpz_t pi, const mpz_t x,
               long challenge);
void evaluate_batch(mpz_t* y_saves, mpz_t* proofs, mpz_t* alphas, 
                const mpz_t* x_s, long challenge, int batch_size);
bool parallel_verify(mpz_t x, long challenge, mpz_t l, mpz_t pi);
bool parallel_diff_verify(mpz_t x, long challenge, mpz_t l, mpz_t pi);
bool naive_verify(mpz_t x, long challenge, mpz_t l, mpz_t pi);
bool batch_verify(mpz_t* y_saves, mpz_t* proofs, mpz_t* alphas, 
                long challenge, int batch_size);
bool verify(const mpz_t y_saved, const mpz_t x, long challenge, const mpz_t l, const mpz_t pi);
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
void hash_prime(mpz_t l, const mpz_t input);

void unit_test();

int bit_length;
};

#endif
