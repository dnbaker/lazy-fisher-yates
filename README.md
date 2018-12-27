###
A header-only lazy Fisher-Yates implementation, only performing permutations as needed.


The easiest thing is to the `make_lazy_shuffle(beg, end)` convenience function.
These all modify the range in-place.

For more advanced uses, please see template arguments on `make_lazy_shuffle`, use `lazy_mod_shuffler`, `lazy_fr_shuffler`, or view the LazyFisherYates class directly.
