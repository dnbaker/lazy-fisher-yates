#ifndef LAZY_FISHER_H__
#define LAZY_FISHER_H__
#include "fastrange/fastrange.h"
#include "aesctr/aesctr.h"
#include <random>
#include <cstdint>
#include <iterator>

#ifndef DEFAULT_PRNG
#define DEFAULT_PRNG ::aes::AesCtr<uint64_t>
#endif


namespace lazy {
using u32 = std::uint32_t;
using u64 = std::uint64_t;
namespace range {
static inline u32 fastrange(u32 x, u32 y) {return ::fastrange32(x, y);}
static inline u64 fastrange(u64 x, u64 y) {return ::fastrange64(x, y);}

struct RangeSelect { // In case I want to add shared functionality later.
};

struct ModSelect: RangeSelect {
    template<typename T, typename IntType>
    IntType operator()(T &gen, IntType lim) const {
        return gen() % lim;
    }
};
template<size_t N>
struct CModSelect: RangeSelect {
    template<typename T, typename IntType>
    IntType operator()(T &gen, IntType i=0) const { // Interface compatibility.
        return gen() % N;
    }
};
struct FRSelect: RangeSelect {
    /* FastRange: uses Daniel Lemire's fastrange to avoid modulus operations while still getting
     * a random index within a range.
     * It might introduce some slight bias, but in practical cases, it seems to be decently effective
     * while also being > 10x as fast.
    */
    template<typename T, typename IntType>
    IntType operator()(T &gen, IntType lim) const {
        return fastrange(gen(), lim);
    }
};
} // namespace range

template<typename Iterator, typename SelectType, typename Generator=DEFAULT_PRNG>
class LazyFisherYates {
// Currently requires a random access iterator
    Iterator start_;
    Iterator end_;
    Generator gen_;
    const SelectType select_;
public:
    LazyFisherYates(Iterator start, Iterator end, Generator &&gen=Generator()):
        start_(start), end_(end), gen_(std::move(gen)), select_() {}
    class iterator {
        LazyFisherYates &ref_;
        Iterator it_;
    public:
        iterator(LazyFisherYates &ref, Iterator pos): ref_(ref), it_(pos) {
            if(it_ == ref_.start_) {
                auto it2 = it_; std::advance(it2, ref_.select_(ref_.gen_, std::distance(it_, ref_.end_)));
                using std::swap;
                swap(*it2, *it_);
            }
        }
        bool operator==(const iterator &o) {return o.it_ == it_;}
        bool operator!=(const iterator &o) {return o.it_ != it_;}
        iterator &operator++() {
            const auto dist = std::distance(++it_, ref_.end_);
            if(dist > 1) {
                assert(std::is_unsigned<decltype(dist)>::value || dist >= 0);
                using std::swap;
                auto it2 = it_;
                std::advance(it2, ref_.select_(ref_.gen_, dist));
                swap(*it2, *it_);
            }
            return *this;
        }
        decltype(auto) operator*(){return *it_;}
        decltype(auto) operator->(){return &*it_;}
    };
    // TODO: const iterator
    iterator begin() {
        return iterator(*this, start_);
    }
    iterator end() {
        return iterator(*this, end_);
    }
    auto &operator[](size_t index) {
        auto it = begin();
        while(it != end() && index--) ++it;
        return *it;
        // No bounds checking here.
    }
};

template<typename It, typename Generator=DEFAULT_PRNG>
using lazy_mod_shuffler = LazyFisherYates<It, range::ModSelect, Generator>;
template<typename It, typename Generator=DEFAULT_PRNG>
using lazy_fr_shuffler = LazyFisherYates<It, range::FRSelect, Generator>;

template<typename It, typename Generator=DEFAULT_PRNG, typename SelectType=range::ModSelect>
auto make_lazy_shuffle(It start, It end, Generator &&gen=Generator()) {
    return LazyFisherYates<It, SelectType, Generator>(start, end, std::move(gen));
}


} // namespace lazy


#endif /* #ifndef LAZY_FISHER_H__ */
