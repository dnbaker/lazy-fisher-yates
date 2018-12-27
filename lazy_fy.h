#ifndef LAZY_FISHER_H__
#define LAZY_FISHER_H__
#include "fastrange/fastrange.h"
#include <random>




namespace lazy {
using u32 = std::uint32_t;
using u64 = std::uint64_t;
namespace range {
static inline u32 fastrange(u32 x, u32 y) {return ::fastrange32(x, y);}
static inline u64 fastrange(u64 x, u64 y) {return ::fastrange64(x, y);}

class RangeSelect { // In case I want to add shared functionality later.
};

class ModSelect: RangeSelect {
    template<typename T, typename IntType>
    IntType operator()(T &gen, IntType lim) {
        return gen() % lim;
    }
};
template<size_t N>
class CModSelect: RangeSelect {
    template<typename T, typename IntType>
    IntType operator()(T &gen, IntType i=0) { // Interface compatibility.
        return gen() % N;
    }
};
class FRSelect: RangeSelect {
    /* FastRange: uses Daniel Lemire's fastrange to avoid modulus operations while still getting
     * a random index within a range.
     * It might introduce some slight bias, but in practical cases, it seems to be decently effective
     * while also being > 10x as fast.
    */
    template<typename T, typename IntType>
    IntType operator()(T &gen, IntType lim) {
        return fastrange(gen(), lim);
    }
};
} // namespace range

template<typename Iterator, typename SelectType, typename Generator=std::mt19937_64>
class LazyFisherYates {
// Currently requires a random access iterator
    Iterator  start_;
    Iterator end_;
    const SelectType select_;
public:
    LazyFisherYates(Iterator start, Iterator end, Generator &&gen):
        start_(start), end_(end), gen_(std::move(gen)), select_() {}
    class iterator {
        LazyFisherYates &ref_;
        Iterator it_;
    public:
        iterator(LazyFisherYates &ref, Iterator pos): ref_(ref), it_(pos) {}
        bool operator==(const iterator &o) {return o.it_ == it_;}
        bool operator!=(const iterator &o) {return o.it_ != it_;}
        iterator &operator++() {
            const auto dist = std::distance(++it_, ref_.end_);
            if(dist > 1) {
                assert(std::is_unsigned<decltype(dist)>::value || dist >= 0);
                using std::swap;
                auto it2 = it_ + select_(gen_, dist);
                swap(*it2, *it_);
            }
            return *this;
        }
    };
    // TODO: const iterator
    iterator begin() {
        return iterator(*this, start_);
    }
    iterator end() {
        return iterator(*this, end_);
    }
};

template<typename It, typename Generator=std::mt19937_64>
using lazy_mod_shuffler = LazyFisherYates<It, range::ModSelect, Generator>
template<typename It, typename Generator=std::mt19937_64>
using lazy_fr_shuffler = LazyFisherYates<It, range::FRSelect, Generator>


} // namespace lazy


#endif /* #ifndef LAZY_FISHER_H__ */
