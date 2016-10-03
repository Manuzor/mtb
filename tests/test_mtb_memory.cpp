#include "mtb_memory.hpp"

#include "catch.hpp"

namespace
{
  struct FooPOD {};
  struct BarPOD { FooPOD Foo; };
  struct BazNoPOD { BazNoPOD() { /* Default ctor */ } };
}

namespace mtb
{
  static_assert(IsPOD<i8>(),  "Expected i8 to be a POD type!");
  static_assert(IsPOD<i16>(), "Expected i16 to be a POD type!");
  static_assert(IsPOD<i32>(), "Expected i32 to be a POD type!");
  static_assert(IsPOD<i64>(), "Expected i64 to be a POD type!");
  static_assert(IsPOD<u8>(),  "Expected u8 to be a POD type!");
  static_assert(IsPOD<u16>(), "Expected u16 to be a POD type!");
  static_assert(IsPOD<u32>(), "Expected u32 to be a POD type!");
  static_assert(IsPOD<u64>(), "Expected u64 to be a POD type!");

  static_assert(IsPOD<FooPOD>(), "Expected FooPOD to be a POD type!");
  static_assert(IsPOD<BarPOD>(), "Expected BarPOD to be a POD type!");
  static_assert(!IsPOD<BazNoPOD>(), "Expected BazNoPOD to be no POD type!");
}

TEST_CASE("Memory Construction", "[Memory]")
{
  using namespace mtb;

  SECTION("Plain Old Data (POD)")
  {
    static_assert(IsPOD<int>(), "int must be POD!");
    int Pods[4]{};

    SECTION("Default construct array")
    {
      ConstructElements(LengthOf(Pods), Pods);
      REQUIRE( Pods[0] == 0 );
      REQUIRE( Pods[1] == 0 );
      REQUIRE( Pods[2] == 0 );
      REQUIRE( Pods[3] == 0 );
    }

    SECTION("Construct array with value")
    {
      ConstructElements(LengthOf(Pods), Pods, 42);
      REQUIRE( Pods[0] == 42 );
      REQUIRE( Pods[1] == 42 );
      REQUIRE( Pods[2] == 42 );
      REQUIRE( Pods[3] == 42 );

      SECTION("Destruct array")
      {
        DestructElements(LengthOf(Pods), Pods);
        // Should not do anything for PODs.
        REQUIRE( Pods[0] == 42 );
        REQUIRE( Pods[1] == 42 );
        REQUIRE( Pods[2] == 42 );
        REQUIRE( Pods[3] == 42 );
      }
    }
  }

  SECTION("Non POD")
  {
    struct counters
    {
      int Ctor{};
      int Dtor{};
    };

    static counters* Counters;
    counters CountersInstance;
    Counters = &CountersInstance;
    MTB_Defer[](){ Counters = nullptr; };

    struct foo
    {
      u8 Data;

      foo() { if(Counters) ++Counters->Ctor; }
      foo(u8 Data) : foo() { this->Data = Data; }
      ~foo() { if(Counters) ++Counters->Dtor; }
    };
    static_assert(!IsPOD<foo>(), "foo must NOT be POD!");
    static_assert(SizeOf<foo>() == 1, "Size of foo must be exactly 1 byte.");


    foo Foos[4];
    REQUIRE( Counters->Ctor == 1 * Convert<int>(LengthOf(Foos)) );
    REQUIRE( Counters->Dtor == 0 * Convert<int>(LengthOf(Foos)) );

    SetBytes(ByteLengthOf(Foos), Foos, 0);

    ConstructElements(LengthOf(Foos), Foos);
    REQUIRE( Counters->Ctor == 2 * Convert<int>(LengthOf(Foos)) );
    REQUIRE( Counters->Dtor == 0 * Convert<int>(LengthOf(Foos)) );
    REQUIRE( Foos[0].Data == 0 );
    REQUIRE( Foos[1].Data == 0 );
    REQUIRE( Foos[2].Data == 0 );
    REQUIRE( Foos[3].Data == 0 );

    ConstructElements(LengthOf(Foos), Foos, u8(42));
    REQUIRE( Counters->Ctor == 3 * Convert<int>(LengthOf(Foos)) );
    REQUIRE( Counters->Dtor == 0 * Convert<int>(LengthOf(Foos)) );
    REQUIRE( Foos[0].Data == 42 );
    REQUIRE( Foos[1].Data == 42 );
    REQUIRE( Foos[2].Data == 42 );
    REQUIRE( Foos[3].Data == 42 );

    DestructElements(LengthOf(Foos), Foos);
    REQUIRE( Counters->Ctor == 3 * Convert<int>(LengthOf(Foos)) );
    REQUIRE( Counters->Dtor == 1 * Convert<int>(LengthOf(Foos)) );
    REQUIRE( Foos[0].Data == 42 );
    REQUIRE( Foos[1].Data == 42 );
    REQUIRE( Foos[2].Data == 42 );
    REQUIRE( Foos[3].Data == 42 );
  }
}

TEST_CASE("Memory Relocation", "[Memory]")
{
  using namespace mtb;

  SECTION("POD types")
  {
    int Ints[]{ 0, 1, 2, 3, 4, 5 };

    SECTION("Non overlapping")
    {
      size_t Num = 3;
      auto A = &Ints[0];
      auto B = &Ints[Num];

      MoveElements(Num, A, B);
      REQUIRE( Ints[0] == 3 );
      REQUIRE( Ints[1] == 4 );
      REQUIRE( Ints[2] == 5 );
      // The other Ints are considered "destructed" now.
    }

    SECTION("Overlapping")
    {
      size_t Num = 3;
      auto A = &Ints[0];
      auto B = &Ints[2];
      MoveElements(Num, A, B);
      REQUIRE( Ints[0] == 2 );
      REQUIRE( Ints[1] == 3 );
      REQUIRE( Ints[2] == 4 );
      REQUIRE( Ints[5] == 5 );
    }
  }

  SECTION("Non POD types")
  {
    struct foo
    {
      int Id = 0;
      int MovedIn = 0;
      int Mover = 0;
      bool IsAlive = true;

      foo() = default;
      foo(int Id) : Id(Id) {}

      void operator =(foo&& ToMove)
      {
        this->MovedIn = ToMove.Id;
        ToMove.Mover = this->Id;
      }

      ~foo()
      {
        this->IsAlive = false;
      }
    };

    foo Foos[]{ {1}, {2}, {3}, {4}, {5}, {6} };

    SECTION("Non overlapping")
    {
      size_t Num = 3;
      auto A = Foos;
      auto B = &Foos[Num];
      MoveElements(Num, A, B);
      REQUIRE(  Foos[0].Id == 1 );
      REQUIRE(  Foos[0].IsAlive );
      REQUIRE(  Foos[0].MovedIn == 4 );
      REQUIRE(  Foos[0].Mover == 0 );
      REQUIRE(  Foos[1].Id == 2 );
      REQUIRE(  Foos[1].IsAlive );
      REQUIRE(  Foos[1].MovedIn == 5 );
      REQUIRE(  Foos[1].Mover == 0 );
      REQUIRE(  Foos[2].Id == 3 );
      REQUIRE(  Foos[2].IsAlive );
      REQUIRE(  Foos[2].MovedIn == 6 );
      REQUIRE(  Foos[2].Mover == 0 );
      REQUIRE(  Foos[3].Id == 4 );
      REQUIRE( !Foos[3].IsAlive );
      REQUIRE(  Foos[3].MovedIn == 0 );
      REQUIRE(  Foos[3].Mover == 1 );
      REQUIRE(  Foos[4].Id == 5 );
      REQUIRE( !Foos[4].IsAlive );
      REQUIRE(  Foos[4].MovedIn == 0 );
      REQUIRE(  Foos[4].Mover == 2 );
      REQUIRE(  Foos[5].Id == 6 );
      REQUIRE( !Foos[5].IsAlive );
      REQUIRE(  Foos[5].MovedIn == 0 );
      REQUIRE(  Foos[5].Mover == 3 );
    }

    SECTION("overlapping")
    {
      size_t Num = 3;
      auto A = &Foos[1];
      auto B = &Foos[3];
      MoveElements(Num, A, B);
      REQUIRE(  Foos[0].Id == 1 );
      REQUIRE(  Foos[0].IsAlive );
      REQUIRE(  Foos[0].MovedIn == 0 );
      REQUIRE(  Foos[0].Mover == 0 );
      REQUIRE(  Foos[1].Id == 2 );
      REQUIRE(  Foos[1].IsAlive );
      REQUIRE(  Foos[1].MovedIn == 4 );
      REQUIRE(  Foos[1].Mover == 0 );
      REQUIRE(  Foos[2].Id == 3 );
      REQUIRE(  Foos[2].IsAlive );
      REQUIRE(  Foos[2].MovedIn == 5 );
      REQUIRE(  Foos[2].Mover == 0 );
      REQUIRE(  Foos[3].Id == 4 );
      REQUIRE(  Foos[3].IsAlive );
      REQUIRE(  Foos[3].MovedIn == 6 );
      REQUIRE(  Foos[3].Mover == 2 );
      REQUIRE(  Foos[4].Id == 5 );
      REQUIRE( !Foos[4].IsAlive );
      REQUIRE(  Foos[4].MovedIn == 0 );
      REQUIRE(  Foos[4].Mover == 3 );
      REQUIRE(  Foos[5].Id == 6 );
      REQUIRE( !Foos[5].IsAlive );
      REQUIRE(  Foos[5].MovedIn == 0 );
      REQUIRE(  Foos[5].Mover == 4 );
    }
  }
}
