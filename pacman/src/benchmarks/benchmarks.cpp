#include "encrypt/vignere_encryptor.h"

#include <benchmark/benchmark.h>

/* TEST ENCRYPTION FUNCTION AND DECRYPTION FUNCTION ON SHORT / LONG STRINGS TO BE ABLE TO OPTIMIZE IT */

static void bench_encrypt_short(benchmark::State& state)
{
    pac::VignereEncryption enc("PACMAN");
    std::string short_string("Hello you.");
    for (auto _ : state)
    {
        enc.encrypt(short_string);
    }
}

BENCHMARK(bench_encrypt_short);

static void bench_encrypt_long(benchmark::State& state)
{
    pac::VignereEncryption enc("PACMAN");
    std::string long_string("I am the most beaufitful person in the world and your name is pacman, but I do not care about it.");
    for (auto _ : state)
    {
        enc.encrypt(long_string);
    }
}

BENCHMARK(bench_encrypt_long);

static void bench_decrypt_short(benchmark::State& state)
{
    pac::VignereEncryption enc("PACMAN");
    std::string short_string("Hello you.");
    for (auto _ : state)
    {
        enc.decrypt(short_string);
    }
}

BENCHMARK(bench_decrypt_short);

static void bench_decrypt_long(benchmark::State& state)
{
    pac::VignereEncryption enc("PACMAN");
    std::string long_string("I am the most beaufitful person in the world and your name is pacman, but I do not care about it.");
    for (auto _ : state)
    {
        enc.decrypt(long_string);
    }
}

BENCHMARK(bench_decrypt_long);
