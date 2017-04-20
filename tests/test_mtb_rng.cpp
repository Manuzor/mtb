#include "mtb_rng.h"

TEST_CASE("Random integer generation", "[RNG]")
{
  SECTION( "Random" )
  {
    mtb_rng A = mtb_RandomSeed(0);
    mtb_rng B = mtb_RandomSeed(0);
    for(int Iteration = 0; Iteration < 255; ++Iteration)
    {
      CAPTURE( Iteration );
      REQUIRE( mtb_Random_u32(&A) == mtb_Random_u32(&B) );
    }
  }

  SECTION( "Random below" )
  {
    mtb_rng RNG = mtb_RandomSeed(0);
    for (mtb_u32 OuterIteration = 0; OuterIteration < 128; ++OuterIteration)
    {
      CAPTURE( OuterIteration );
      mtb_u32 Bound = OuterIteration * 79;
      CAPTURE( Bound );
      for(mtb_u32 InnerIteration = 0; InnerIteration < 128; ++InnerIteration)
      {
        CAPTURE( InnerIteration );
        mtb_u32 Rand = mtb_RandomBelow_u32(&RNG, Bound);
        if(Bound == 0)
        {
          REQUIRE( Rand == 0 );
        }
        else
        {
          REQUIRE( Rand >= 0 );
          REQUIRE( Rand < Bound );
        }
      }
    }
  }

  SECTION( "Random between" )
  {
    mtb_rng RNG = mtb_RandomSeed(0);
    for (mtb_u32 OuterIteration = 0; OuterIteration < 128; ++OuterIteration)
    {
      CAPTURE( OuterIteration );
      mtb_u32 LowerBound = OuterIteration * 47;
      mtb_u32 UpperBound = OuterIteration * 79;
      CAPTURE( UpperBound );
      for(mtb_u32 InnerIteration = 0; InnerIteration < 128; ++InnerIteration)
      {
        CAPTURE( InnerIteration );
        mtb_u32 Rand = mtb_RandomBetween_u32(&RNG, LowerBound, UpperBound);
        if(UpperBound == 0)
        {
          REQUIRE( Rand == 0 );
        }
        else
        {
          REQUIRE( Rand >= LowerBound );
          REQUIRE( Rand <= UpperBound );
        }
      }
    }
  }
}

TEST_CASE("Random floating-point number generation", "[RNG]")
{
  mtb_rng RNG = mtb_RandomSeed(0);

  SECTION( "Unilateral" )
  {
    for (int Iteration = 0; Iteration < 255; ++Iteration)
    {
      REQUIRE( mtb_RandomUnilateral_f32(&RNG) <= 1.0f );
      REQUIRE( mtb_RandomUnilateral_f32(&RNG) >= 0.0f );
    }
  }

  SECTION( "Bilateral" )
  {
    for (int Iteration = 0; Iteration < 255; ++Iteration)
    {
      REQUIRE( mtb_RandomBilateral_f32(&RNG) <= 1.0f );
      REQUIRE( mtb_RandomBilateral_f32(&RNG) >= -1.0f );
    }
  }

  SECTION( "Find unilateral 1.0f" )
  {
    RNG = mtb_RandomSeed(0);
    mtb_f32 Rand;

    for (mtb_u64 Iteration = 0; Iteration < 1'800'000; ++Iteration)
    {
      Rand = mtb_RandomUnilateral_f32(&RNG);
      if(Rand == 1.0f)
        break;
    }

    REQUIRE( Rand == 1.0f );
  }
}
