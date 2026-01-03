#ifndef HOBBES_UTIL_INT128_HPP_INCLUDED
#define HOBBES_UTIL_INT128_HPP_INCLUDED

#if defined(_MSC_VER)
class alignas(16) int128_t {
  public:
    int64_t low, high;
    int128_t() = default;
    int128_t(int i) { low = i; high = 0;}
    bool operator==(const int128_t& rhs) const { return ( low == rhs.low) && (high == rhs.high); }
    bool operator<(const int128_t& rhs) const { return (high < rhs.high) || ((high == rhs.high) && (low < rhs.low)); }
};

#else
using int128_t = __int128;
#endif

#endif