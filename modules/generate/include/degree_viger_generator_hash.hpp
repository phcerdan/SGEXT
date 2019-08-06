/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef DEGREE_VIGER_GENERATOR_HASH_HPP
#define DEGREE_VIGER_GENERATOR_HASH_HPP

#include "rng.hpp"
#include <assert.h>
#include <cstddef> // For NULL

namespace SG {

// Fast replace
inline int *fast_rpl(int *m, const int a, const int b) {
    while (*m != a)
        m++;
    *m = b;
    return m;
}
// Fast search
inline int *fast_search(int *m, const int size, const int a) {
    int *p = m + size;
    while (m != p--)
        if (*p == a)
            return p;
    return NULL;
}

namespace generator {

inline unsigned long int rand_int31() {
    // 0x7FFFFFFFUL is the 32 bits (4 bytes) max number: 1111111111111111...
    return (unsigned long int)(RNG::rand01() * 0x7FFFFFFFUL);
};
inline int my_random() { return rand_int31(); }

static constexpr int HASH_NONE = -1;
static constexpr int HASH_MIN_SIZE = 100;
inline constexpr bool IS_HASH(const int x) { return (x > HASH_MIN_SIZE); };
inline constexpr int HASH_EXPAND(int x) {
    x += x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
};

inline constexpr int HASH_KEY(const int x, const int size) {
    return (x * 2198737) & ((size)-1);
}

inline constexpr int HASH_REKEY(const int k, const int s) {
    assert(k >= 0);
    if (k == 0)
        return s - 1;
    else
        return k - 1;
};
inline constexpr int HASH_SIZE(const int x) {
    if (IS_HASH(x))
        return HASH_EXPAND(x);
    else
        return x;
};

inline int HASH_PAIR_KEY(const int x, const int y, const int size) {
    return HASH_KEY(x * 1434879443 + y, size);
}

//_________________________________________________________________________
// Hash-only functions : table must NOT be Raw.
// the argument 'size' is the total size of the hash table
//_________________________________________________________________________

// copy hash table into raw vector
inline void H_copy(int *mem, int *h, int size) {
    for (int i = HASH_EXPAND(size); i--; h++)
        if (*h != HASH_NONE)
            *(mem++) = *h;
}

// Look for the place to add an element. Return NULL if element is already here.
inline int *H_add(int *h, const int size, int a) {
    int k = HASH_KEY(a, size);
    if (h[k] == HASH_NONE)
        return h + k;
    while (h[k] != a) {
        k = HASH_REKEY(k, size);
        if (h[k] == HASH_NONE)
            return h + k;
    }
    return NULL;
}

// would element be well placed in newk ?
inline bool
H_better(const int a, const int size, const int currentk, const int newk) {
    int k = HASH_KEY(a, size);
    if (newk < currentk)
        return (k < currentk && k >= newk);
    else
        return (k < currentk || k >= newk);
}

// removes h[k]
inline void H_rm(int *h, const int size, int k) {
    int lasthole = k;
    do {
        k = HASH_REKEY(k, size);
        int next = h[k];
        if (next == HASH_NONE)
            break;
        if (H_better(next, size, k, lasthole)) {
            h[lasthole] = next;
            lasthole = k;
        }
    } while (true);
    h[lasthole] = HASH_NONE;
}

// put a
inline int *H_put(int *h, const int size, const int a) {
    assert(H_add(h, size, a) != NULL);
    int k = HASH_KEY(a, size);
    while (h[k] != HASH_NONE) {
        k = HASH_REKEY(k, size);
    }
    h[k] = a;
    assert(H_add(h, size, a) == NULL);
    return h + k;
}

// find A
inline int H_find(int *h, int size, const int a) {
    assert(H_add(h, size, a) == NULL);
    int k = HASH_KEY(a, size);
    while (h[k] != a) {
        k = HASH_REKEY(k, size);
    }
    return k;
}

// Look for the place to add an element. Return NULL if element is already here.
inline bool H_pair_insert(int *h, const int size, int a, int b) {
    int k = HASH_PAIR_KEY(a, b, size);
    if (h[2 * k] == HASH_NONE) {
        h[2 * k] = a;
        h[2 * k + 1] = b;
        return true;
    }
    while (h[2 * k] != a || h[2 * k + 1] != b) {
        k = HASH_REKEY(k, size);
        if (h[2 * k] == HASH_NONE) {
            h[2 * k] = a;
            h[2 * k + 1] = b;
            return true;
        }
    }
    return false;
}

//_________________________________________________________________________
// Generic functions : table can be either Hash or Raw.
// the argument 'size' is the number of elements
//_________________________________________________________________________

// Look for an element
inline bool H_is(int *mem, const int size, const int elem) {
    if (IS_HASH(size))
        return (H_add(mem, HASH_EXPAND(size), elem) == NULL);
    else
        return fast_search(mem, size, elem) != NULL;
}

// pick random location (containing an element)
inline int *H_random(int *mem, int size) {
    if (!IS_HASH(size))
        return mem + (my_random() % size);
    size = HASH_EXPAND(size);
    int *yo;
    do {
        yo = mem + HASH_KEY(my_random(), size);
    } while (*yo == HASH_NONE);
    return yo;
}

// replace *k by b
inline int *H_rpl(int *mem, int size, int *k, const int b) {
    assert(!H_is(mem, size, b));
    if (!IS_HASH(size)) {
        *k = b;
        return k;
    } else {
        size = HASH_EXPAND(size);
        assert(mem + int(k - mem) == k);
        H_rm(mem, size, int(k - mem));
        return H_put(mem, size, b);
    }
}

// replace a by b
inline int *H_rpl(int *mem, int size, const int a, const int b) {
    assert(H_is(mem, size, a));
    assert(!H_is(mem, size, b));
    if (!IS_HASH(size))
        return fast_rpl(mem, a, b);
    else {
        size = HASH_EXPAND(size);
        H_rm(mem, size, H_find(mem, size, a));
        return H_put(mem, size, b);
    }
}

} // namespace generator
} // namespace SG
#endif
