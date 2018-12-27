#include "lazy_fisher_yates.h"

using namespace lazy;
int main() {
    using It = typename std::vector<int>::iterator;
    std::vector<int> i(1000);
    std::iota(i.begin(), i.end(), 0);
    auto shuffler = make_lazy_shuffle(i.begin(), i.end());
    for(const auto v: shuffler) std::fprintf(stderr, "v: %d\n", v);
}
