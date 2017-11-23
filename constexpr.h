#pragma once

constexpr long constfactorial(long n) {
    long f = 1;
    if ( n > 1 ) {
        f = n * constfactorial(n-1);
    }
    return f;
}

