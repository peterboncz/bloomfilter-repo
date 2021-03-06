// Generating random data

#pragma once

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <functional>
#include <random>
#include <stdexcept>
#include <vector>

#include <dtl/dtl.hpp>
#include <dtl/mem.hpp>

template<typename int_type>
std::vector<int_type> GenerateRandom(std::size_t count);


template<>
std::vector<uint64_t> GenerateRandom<uint64_t>(::std::size_t count) {
  std::vector<uint64_t> result(count);
  std::random_device random;
  // To generate random keys to lookup, this uses ::std::random_device which is slower but
  // stronger than some other pseudo-random alternatives. The reason is that some of these
  // alternatives (like libstdc++'s ::std::default_random, which is a linear congruential
  // generator) behave non-randomly under some hash families like Dietzfelbinger's
  // multiply-shift.
  auto genrand = [&random]() {
    return random() + (static_cast<::std::uint64_t>(random()) << 32);
  };
  std::generate(result.begin(), result.end(), std::ref(genrand));
  return result;
}


template<>
std::vector<uint32_t> GenerateRandom<uint32_t>(std::size_t count) {
  std::vector<uint32_t> result(count);
  std::random_device random;
  // To generate random keys to lookup, this uses ::std::random_device which is slower but
  // stronger than some other pseudo-random alternatives. The reason is that some of these
  // alternatives (like libstdc++'s ::std::default_random, which is a linear congruential
  // generator) behave non-randomly under some hash families like Dietzfelbinger's
  // multiply-shift.
  std::generate(result.begin(), result.end(), std::ref(random));
  return result;
}


std::vector<uint32_t> GenerateRandom_u32(std::size_t count, std::bitset<1ull<<32>* bs) {
  std::vector<uint32_t> result(count);
  std::random_device random;
  // To generate random keys to lookup, this uses ::std::random_device which is slower but
  // stronger than some other pseudo-random alternatives. The reason is that some of these
  // alternatives (like libstdc++'s ::std::default_random, which is a linear congruential
  // generator) behave non-randomly under some hash families like Dietzfelbinger's
  // multiply-shift.
  auto genrand = [&]() {
    uint32_t r = random();
    while ((*bs)[r]) {
      (*bs)[r] = true;
      r = random();
    }
    return r;
  };
  std::generate(result.begin(), result.end(), std::ref(genrand));
  return result;
}


// Using two pointer ranges for sequences x and y, create a vector clone of x but for
// y_probability y's mixed in.
template <typename T>
std::vector<T, dtl::mem::numa_allocator<T>> MixIn(const T* x_begin, const T* x_end, const T* y_begin, const T* y_end,
    double y_probability) {
  const size_t x_size = x_end - x_begin, y_size = y_end - y_begin;
  if (y_size > (1ull << 32)) throw std::length_error("y is too long");
  std::vector<T, dtl::mem::numa_allocator<T>> result(x_begin, x_end);
  std::random_device random;
  auto genrand = [&random, y_size]() {
    return (static_cast<size_t>(random()) * y_size) >> 32;
  };
  for (size_t i = 0; i < y_probability * x_size; ++i) {
    result[i] = *(y_begin + genrand());
  }
  std::shuffle(result.begin(), result.end(), random);
  return result;
}
