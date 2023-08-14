#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <tuple>

template <typename... Ts> class AOS {
  using T = std::tuple<Ts...>;
  static constexpr std::size_t num_types = sizeof...(Ts);
  static constexpr size_t element_size = sizeof(T);

  static constexpr std::array<std::size_t, num_types> alignments = {
      std::alignment_of_v<Ts>...};

  template <int I> using NthType = typename std::tuple_element<I, T>::type;

  static constexpr std::array<std::size_t, num_types> sizes = {sizeof(Ts)...};
  size_t num_spots;
  T *base_array;

  template <size_t... Is>
  auto get_impl(size_t i,
                [[maybe_unused]] std::integer_sequence<size_t, Is...> int_seq) {
    return std::forward_as_tuple(std::get<Is>(base_array[i])...);
  }

public:
  size_t get_size() { return num_spots * element_size; }

  AOS(size_t n) : num_spots(n) {
    // set the total array to be 64 byte alignmed

    uintptr_t length_to_allocate = get_size();
    base_array = static_cast<T *>(std::malloc(length_to_allocate));
    std::cout << "allocated size " << length_to_allocate << "\n";
  }
  ~AOS() { free(base_array); }
  void zero() { std::memset(base_array, 0, get_size()); }

  template <size_t... Is> auto get(size_t i) {
    if constexpr (sizeof...(Is) > 0) {
      return get_impl<Is...>(i, {});
    } else {
      return get_impl(i, std::make_index_sequence<num_types>{});
    }
  }

  static void print_type_details() {
    std::cout << "num types are " << num_types << "\n";
    std::cout << "their alignments are ";
    for (const auto e : alignments) {
      std::cout << e << ", ";
    }
    std::cout << "\n";
    std::cout << "their sizes are ";
    for (const auto e : sizes) {
      std::cout << e << ", ";
    }
    std::cout << "\n";
  }

  template <size_t... Is, class F>
  void map_range(F f, size_t start = 0,
                 size_t end = std::numeric_limits<size_t>::max()) {
    if (end == std::numeric_limits<size_t>::max()) {
      end = num_spots;
    }
    for (size_t i = start; i < end; i++) {
      std::apply(f, get<Is...>(i));
    }
  }

  template <size_t... Is, class F>
  void map_range_with_index(F f, size_t start = 0,
                            size_t end = std::numeric_limits<size_t>::max()) {
    if (end == std::numeric_limits<size_t>::max()) {
      end = num_spots;
    }
    for (size_t i = start; i < end; i++) {
      std::apply(f, std::tuple_cat(std::make_tuple(i), get<Is...>(i)));
    }
  }
};
