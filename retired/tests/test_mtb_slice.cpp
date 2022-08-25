#include "../mtb.h"

TEST_CASE("Slice creation", "[slice]") {
    SECTION("Uninitialized slices ") {
        mtb::tSlice<int> foo{};
        REQUIRE(mtb::Len(foo) == 0);
        REQUIRE(foo.ptr == nullptr);
    }

    SECTION("From pointer and length") {
        int foo = 42;
        auto foo_slice = mtb::PtrSlice(&foo, 1);
        REQUIRE(mtb::Len(foo_slice) == 1);
        REQUIRE(foo_slice[0] == 42);
    }

    SECTION("From begin and end pointer") {
        REQUIRE(mtb::Len(mtb::PtrSliceBetween<int>(nullptr, nullptr)) == 0);
        REQUIRE(mtb::PtrSliceBetween<int>(nullptr, nullptr).ptr == nullptr);

        int foos[] = {0, 1, 2};

        mtb::tSlice<int> foo1 = mtb::PtrSliceBetween(&foos[0], &foos[0]);
        REQUIRE(mtb::Len(foo1) == 0);
        REQUIRE(foo1.ptr == &foos[0]);

        mtb::tSlice<int> foo2 = mtb::PtrSliceBetween(&foos[0], &foos[3]);
        REQUIRE(mtb::Len(foo2) == 3);
        REQUIRE(foo2[0] == foos[0]);
        REQUIRE(foo2[1] == foos[1]);
        REQUIRE(foo2[2] == foos[2]);
    }

    SECTION("From static array") {
        int foo[] = {0, 1, 2};
        auto bar = mtb::ArraySlice(foo);
        REQUIRE(mtb::Len(bar) == mtb::Len(foo));
        REQUIRE(bar.ptr == &foo[0]);
    }

#if 0
    SECTION("From const string") {
        const char char_array[] = { 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0' };
        const char* string = &char_array[0];

        auto foo = SliceFromString(char_array);
        REQUIRE(mtb::Len(foo) == mtb::Len(char_array) - 1);
        REQUIRE(foo.ptr == &char_array[0]);

        auto bar = SliceFromString(string);
        REQUIRE(mtb::Len(bar) == mtb::Len(char_array) - 1);
        REQUIRE(bar.ptr == string);
    }

    SECTION("From mutable string") {
        char char_array[] = { 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0' };
        char* string = &char_array[0];

        auto foo = SliceFromString(char_array);
        REQUIRE(mtb::Len(foo) == mtb::Len(char_array) - 1);
        REQUIRE(foo.ptr == &char_array[0]);

        auto bar = SliceFromString(string);
        REQUIRE(mtb::Len(bar) == mtb::Len(char_array) - 1);
        REQUIRE(bar.ptr == string);
    }
#endif

    SECTION("From two indices") {
        int data_array[]{ 0, 1, 2 };
        auto data = mtb::ArraySlice(data_array);

        auto foo = mtb::SliceBetween(data, 0, 0);
        REQUIRE( foo.len == 0 );
        REQUIRE( foo.ptr == data.ptr );

        auto bar = mtb::SliceBetween(data, 0, 1);
        REQUIRE( bar.len == 1 );
        REQUIRE( bar.ptr == data.ptr );

        auto baz = mtb::SliceBetween(bar, 0, 1);
        REQUIRE( baz.len == 1 );
        REQUIRE( baz.ptr == data.ptr );

        auto baar = mtb::SliceBetween(data, 1, 3);
        REQUIRE( baar.len == 2 );
        REQUIRE( baar.ptr == data.ptr + 1 );
    }
}

TEST_CASE("Slice implicit bool conversion", "[slice]") {
    mtb::tSlice<int> foo{};
    REQUIRE(static_cast<bool>(foo) == false);

    SECTION("Set Num") {
        foo.len = 42;
        REQUIRE(static_cast<bool>(foo) == false);
    }

    SECTION("Set ptr") {
        int dummy;
        foo.ptr = &dummy;
        REQUIRE(static_cast<bool>(foo) == false);
    }
}

TEST_CASE("Slices of void", "[slice]") {
    uint32_t dummy = 0;
    mtb::tSlice<uint32_t> foo = mtb::StructSlice(dummy);
    mtb::tSlice<void const> bar = foo;
    REQUIRE(bar.len == 4);
}

TEST_CASE("Slice casting", "[slice]") {
    struct tFoo {
        char stuff[3];
    };
    static_assert(sizeof(tFoo) == 3, "Unexpected size of tFoo");

    struct tBar {
        char stuff[2];
    };
    static_assert(sizeof(tBar) == 2, "Unexpected size of tBar");

    tFoo foo_array[4] {
        tFoo{ { 'a', 'b', 'c' } },
        tFoo{ { 'd', 'e', 'f' } },
        tFoo{ { 'g', 'h', 'i' } },
        tFoo{ { 'j', 'k', 'l' } },
    };

    tBar bar_array[6] {
        tBar{ { 'A', 'B' } },
        tBar{ { 'C', 'D' } },
        tBar{ { 'E', 'F' } },
        tBar{ { 'G', 'H' } },
        tBar{ { 'I', 'J' } },
        tBar{ { 'K', 'L' } },
    };

    SECTION("Reinterpretation to smaller type") {
        mtb::tSlice<tFoo> foo = mtb::ArraySlice(foo_array);
        mtb::tSlice<tBar> bar = mtb::SliceCast<tBar>(foo);
        REQUIRE(bar.len == 6);
        REQUIRE(bar[0].stuff[0] == 'a');
        REQUIRE(bar[0].stuff[1] == 'b');
        REQUIRE(bar[1].stuff[0] == 'c');
        REQUIRE(bar[1].stuff[1] == 'd');
        REQUIRE(bar[2].stuff[0] == 'e');
        REQUIRE(bar[2].stuff[1] == 'f');
        REQUIRE(bar[3].stuff[0] == 'g');
        REQUIRE(bar[3].stuff[1] == 'h');
        REQUIRE(bar[4].stuff[0] == 'i');
        REQUIRE(bar[4].stuff[1] == 'j');
        REQUIRE(bar[5].stuff[0] == 'k');
        REQUIRE(bar[5].stuff[1] == 'l');
    }
    SECTION("Reinterpretation to bigger type") {
        mtb::tSlice<tBar> bar = mtb::ArraySlice(bar_array);
        mtb::tSlice<tFoo> foo = mtb::SliceCast<tFoo>(bar);
        REQUIRE(foo[0].stuff[0] == 'A');
        REQUIRE(foo[0].stuff[1] == 'B');
        REQUIRE(foo[0].stuff[2] == 'C');
        REQUIRE(foo[1].stuff[0] == 'D');
        REQUIRE(foo[1].stuff[1] == 'E');
        REQUIRE(foo[1].stuff[2] == 'F');
        REQUIRE(foo[2].stuff[0] == 'G');
        REQUIRE(foo[2].stuff[1] == 'H');
        REQUIRE(foo[2].stuff[2] == 'I');
        REQUIRE(foo[3].stuff[0] == 'J');
        REQUIRE(foo[3].stuff[1] == 'K');
        REQUIRE(foo[3].stuff[2] == 'L');
    }
}
#if 0

TEST_CASE("Slice Equality", "[slice]")
{
    using namespace mtb;

    SECTION("Both Empty")
    {
        REQUIRE(tSlice<void>()  == tSlice<void>());
        REQUIRE(tSlice<i32>() == tSlice<i32>());
        REQUIRE(tSlice<u16>() == tSlice<i32>());
    }

    SECTION("Single Value")
    {
        int A = 42;
        int B = 123;
        int C = 42;
        auto foo = tSlice(1, &A);
        auto bar = tSlice(1, &B);
        auto baz = tSlice(1, &C);

        REQUIRE(foo == foo);
        REQUIRE(foo != bar);
        REQUIRE(foo == baz);
    }

    SECTION("From Same data")
    {
        int data[] = { 1, 2, 1, 2, };
        auto foo = tSlice(2, &data[0]);
        auto bar = tSlice(2, &data[1]);
        auto baz = tSlice(2, &data[2]);

        REQUIRE(foo == foo);
        REQUIRE(foo != bar);
        REQUIRE(foo == baz);
    }

    SECTION("Same length, different content but same first element")
    {
        auto foo = SliceFromString("ABC");
        auto bar = SliceFromString("ABD");

        REQUIRE(foo != bar);
    }
}

TEST_CASE("Slice Searching", "[slice]")
{
    using namespace mtb;

    int Ints[] = { 0, 1, 2, 3, 4, 5, 6 };
    auto foo = tSlice(Ints);

    SECTION("CountUntil")
    {
        REQUIRE( CountUntil(tSlice(0, (int const*)nullptr), 42) == INVALID_INDEX );

        REQUIRE( CountUntil(AsConst(foo), 0) == 0 );
        REQUIRE( CountUntil(AsConst(foo), 2) == 2 );
        REQUIRE( CountUntil(AsConst(foo), 6) == 6 );
        REQUIRE( CountUntil(AsConst(foo), 7) == INVALID_INDEX );
    }

    SECTION("CountUntil with predicate")
    {
        REQUIRE( CountUntil(tSlice<int const>((size_t)0, nullptr), 42) == INVALID_INDEX );

        REQUIRE( CountUntil(AsConst(foo), -1, [](int A, int B){ return A == 0; }) == 0 );
        REQUIRE( CountUntil(AsConst(foo), -1, [](int A, int B){ return A == 2; }) == 2 );
        REQUIRE( CountUntil(AsConst(foo), -1, [](int A, int B){ return A == 6; }) == 6 );
        REQUIRE( CountUntil(AsConst(foo), -1, [](int A, int B){ return A == 7; }) == INVALID_INDEX );
    }
}

TEST_CASE("Slice Misc", "[slice]")
{
    using namespace mtb;

    tSlice<int> foo{};
    foo.len = 42;
    REQUIRE( ByteLengthOf(foo) == 42 * sizeof(int) );

    tSlice<void> bar{};
    SetLength(bar, 1337);
    REQUIRE( ByteLengthOf(bar) == 1337 );
}
#endif
