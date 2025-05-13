#include <utility> // For std::to_underlying (C++23)
#include <stdio.h>

enum class MyEnum : int {
    A = 1,
    B = 2
};

int main() {
    MyEnum e = MyEnum::A;
#if __cplusplus >= 202302L
    auto val = std::to_underlying(e);
    printf("Value: %d (C++23 path)\n", static_cast<int>(val));
#else
    printf("Value: %d (Non-C++23 path, __cplusplus: %ldL)\n", static_cast<int>(e), __cplusplus);
#endif
    return 0;
} 