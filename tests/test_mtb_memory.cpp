#include "mtb_memory.hpp"


struct FooPOD {};
struct BarPOD { FooPOD Foo; };
struct BazNoPOD { BazNoPOD() { /* Default ctor */ } };

static_assert(MTB_IsPod(mtb_s08), "Expected mtb_s08 to be a POD type!");
static_assert(MTB_IsPod(mtb_s16), "Expected mtb_s16 to be a POD type!");
static_assert(MTB_IsPod(mtb_s32), "Expected mtb_s32 to be a POD type!");
static_assert(MTB_IsPod(mtb_s64), "Expected mtb_s64 to be a POD type!");
static_assert(MTB_IsPod(mtb_u08), "Expected mtb_u08 to be a POD type!");
static_assert(MTB_IsPod(mtb_u16), "Expected mtb_u16 to be a POD type!");
static_assert(MTB_IsPod(mtb_u32), "Expected mtb_u32 to be a POD type!");
static_assert(MTB_IsPod(mtb_u64), "Expected mtb_u64 to be a POD type!");

static_assert(MTB_IsPod(FooPOD), "Expected FooPOD to be a POD type!");
static_assert(MTB_IsPod(BarPOD), "Expected BarPOD to be a POD type!");
static_assert(!MTB_IsPod(BazNoPOD), "Expected BazNoPOD to be no POD type!");


TEST_CASE("Memory Construction", "[Memory]")
{
  SECTION("Plain Old Data (POD)")
  {
    static_assert(MTB_IsPod(int), "int must be POD!");
    int Pods[4]{};

    SECTION("Default construct array")
    {
      mtb_ConstructElements(mtb_LengthOf(Pods), Pods);
      REQUIRE( Pods[0] == 0 );
      REQUIRE( Pods[1] == 0 );
      REQUIRE( Pods[2] == 0 );
      REQUIRE( Pods[3] == 0 );
    }

    SECTION("Construct array with value")
    {
      mtb_ConstructElements(mtb_LengthOf(Pods), Pods, 42);
      REQUIRE( Pods[0] == 42 );
      REQUIRE( Pods[1] == 42 );
      REQUIRE( Pods[2] == 42 );
      REQUIRE( Pods[3] == 42 );

      SECTION("Destruct array")
      {
        mtb_DestructElements(mtb_LengthOf(Pods), Pods);
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
    MTB_DEFER[](){ Counters = nullptr; };

    struct foo
    {
      mtb_u08 Data;

      foo() { if(Counters) ++Counters->Ctor; }
      foo(mtb_u08 Data) : foo() { this->Data = Data; }
      ~foo() { if(Counters) ++Counters->Dtor; }
    };
    static_assert(!MTB_IsPod(foo), "foo must NOT be POD!");
    static_assert(sizeof(foo) == 1, "Size of foo must be exactly 1 byte.");
    static_assert(mtb_SafeSizeOf<foo>() == 1, "Size of foo must be exactly 1 byte.");


    foo Foos[4];
    REQUIRE( Counters->Ctor == 1 * mtb_LengthOf(Foos) );
    REQUIRE( Counters->Dtor == 0 * mtb_LengthOf(Foos) );

    mtb_SetBytes(mtb_ByteLengthOf(Foos), Foos, 0);

    mtb_ConstructElements(mtb_LengthOf(Foos), Foos);
    REQUIRE( Counters->Ctor == 2 * mtb_LengthOf(Foos) );
    REQUIRE( Counters->Dtor == 0 * mtb_LengthOf(Foos) );
    REQUIRE( Foos[0].Data == 0 );
    REQUIRE( Foos[1].Data == 0 );
    REQUIRE( Foos[2].Data == 0 );
    REQUIRE( Foos[3].Data == 0 );

    mtb_ConstructElements(mtb_LengthOf(Foos), Foos, mtb_u08(42));
    REQUIRE( Counters->Ctor == 3 * mtb_LengthOf(Foos) );
    REQUIRE( Counters->Dtor == 0 * mtb_LengthOf(Foos) );
    REQUIRE( Foos[0].Data == 42 );
    REQUIRE( Foos[1].Data == 42 );
    REQUIRE( Foos[2].Data == 42 );
    REQUIRE( Foos[3].Data == 42 );

    mtb_DestructElements(mtb_LengthOf(Foos), Foos);
    REQUIRE( Counters->Ctor == 3 * mtb_LengthOf(Foos) );
    REQUIRE( Counters->Dtor == 1 * mtb_LengthOf(Foos) );
    REQUIRE( Foos[0].Data == 42 );
    REQUIRE( Foos[1].Data == 42 );
    REQUIRE( Foos[2].Data == 42 );
    REQUIRE( Foos[3].Data == 42 );
  }
}

TEST_CASE("Memory Relocation", "[Memory]")
{
  SECTION("POD types")
  {
    int Ints[]{ 0, 1, 2, 3, 4, 5 };

    SECTION("Non overlapping")
    {
      size_t Num = 3;
      auto A = &Ints[0];
      auto B = &Ints[Num];

      mtb_MoveElements(Num, A, B);
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
      mtb_MoveElements(Num, A, B);
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
      mtb_MoveElements(Num, A, B);
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
      mtb_MoveElements(Num, A, B);
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
