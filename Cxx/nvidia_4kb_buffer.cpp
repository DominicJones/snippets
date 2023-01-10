// https://godbolt.org/z/jGe3TeGnn

// Error: Formal parameter space overflowed (4097 bytes required, max 4096 bytes allowed)

class A {
    char a[4097];
};

__global__ void test(A a) {
}
