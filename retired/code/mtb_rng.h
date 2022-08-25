
/*
 * Random number generation is modeled after the minimal implementation
 * of one member of the PCG family of random number
 * generators by Melissa O'Neill <oneill@pcg-random.org>
 * and adjusted to my codebase (mtb).
 * Hosted at: https://github.com/imneme/pcg-c-basic
 * See below for details.
 */

/*
 * PCG Random Number Generation for C.
 *
 * Copyright 2014 Melissa O'Neill <oneill@pcg-random.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For additional information about the PCG random number generation scheme,
 * including its license and other licensing options, visit
 *
 *     http://www.pcg-random.org
 */

/*
 * The general interface for random number generation is heavily inspired by
 * Handmade Hero by Casey Muratori.
 */

#if !defined(MTB_HEADER_mtb_rng)
#define MTB_HEADER_mtb_rng

#if defined(MTB_IMPLEMENTATION) && !defined(MTB_RNG_IMPLEMENTATION)
  #define MTB_RNG_IMPLEMENTATION
#endif

#if defined(MTB_RNG_IMPLEMENTATION) && !defined(MTB_IMPLEMENTATION)
  #define MTB_IMPLEMENTATION
#endif

#include "mtb_common.h"

struct mtb_rng
{
  mtb_u64 State;
  mtb_u64 Inc;
};

/// Create a random number generator object with the given seed.
mtb_rng
mtb_RandomSeed(mtb_u64 InitialState, mtb_u64 StreamID = 1);

///
/// Random unsigned 32-bit integer values.
///

/// Generate a uniformly distributed 32-bit random number.
mtb_u32
mtb_Random_u32(mtb_rng* RNG);

/// Generate a uniformly distributed 32-bit random number, Result, where 0 <= Result < Bound.
mtb_u32
mtb_RandomBelow_u32(mtb_rng* RNG, mtb_u32 Bound);

/// Generate a uniformly distributed 32-bit random number, Result, where LowerBound <= Result <= UpperBound.
mtb_u32
mtb_RandomBetween_u32(mtb_rng* RNG, mtb_u32 LowerBound, mtb_u32 UpperBound);

///
/// Random 32-bit floating-point values.
///

/// Generate a uniformly distributed 32-bit random floating point number, Result, where 0 <= Result <= 1.
mtb_f32
mtb_RandomUnilateral_f32(mtb_rng* RNG);

/// Generate a uniformly distributed 32-bit random floating point number, Result, where -1 <= Result <= 1.
mtb_f32
mtb_RandomBilateral_f32(mtb_rng* RNG);

/// Generate a uniformly distributed 32-bit random number, Result, where LowerBound <= Result < UpperBound.
mtb_f32
mtb_RandomBetween_f32(mtb_rng* RNG, mtb_f32 LowerBound, mtb_f32 UpperBound);

#if !defined(MTB_FLAG_ORIGINAL_BOUNDED_PCG)
  #define MTB_FLAG_ORIGINAL_BOUNDED_PCG MTB_OFF
#endif

#endif // !defined(MTB_HEADER_mtb_rng)


// ==============
// Implementation
// ==============

#if defined(MTB_RNG_IMPLEMENTATION)
#if !defined(MTB_IMPL_mtb_rng)
#define MTB_IMPL_mtb_rng

mtb_rng
mtb_RandomSeed(mtb_u64 InitialState, mtb_u64 StreamID)
{
  mtb_rng Result{};
  Result.Inc = (StreamID << 1u) | 1u;
  (void)mtb_Random_u32(&Result);
  Result.State += InitialState;
  (void)mtb_Random_u32(&Result);
  return Result;
}

mtb_u32
mtb_Random_u32(mtb_rng* RNG)
{
  mtb_u64 OldState = RNG->State;
  RNG->State = OldState * 6364136223846793005ULL + RNG->Inc;
  mtb_u32 XOrShifted = (mtb_u32)(((OldState >> 18u) ^ OldState) >> 27u);
  mtb_u32 Rot = (mtb_u32)(OldState >> 59u);
  mtb_u32 Result = (XOrShifted >> Rot) | (XOrShifted << (((mtb_u32)(-(mtb_s32)Rot)) & 31));
  return Result;
}

mtb_u32
mtb_RandomBelow_u32(mtb_rng* RNG, mtb_u32 Bound)
{
  mtb_u32 Result = 0;

  if(Bound > 0)
  {
#if MTB_FLAG(ORIGINAL_BOUNDED_PCG)
    // NOTE(Manuzor): Even though it says that the original implementation
    // should usually be fast, I'm kind of hung up on that "usually" part. I
    // think I'm willing to sacrifice a little performance for deterministic
    // program behavior in this case. By default, at least.

    // To avoid bias, we need to make the range of the RNG a multiple of
    // Bound, which we do by dropping output less than a Threshold.
    // A naive scheme to calculate the Threshold would be to do
    //
    //     mtb_u32 Threshold = 0x100000000ull % Bound;
    //
    // but 64-bit div/mod is slower than 32-bit div/mod (especially on
    // 32-bit platforms).  In essence, we do
    //
    //     mtb_u32 Threshold = (0x100000000ull-Bound) % Bound;
    //
    // because this version will calculate the same modulus, but the LHS
    // value is less than 2^32.
    mtb_u32 Threshold = (mtb_u32)(-(mtb_s32)Bound) % Bound;

    // Uniformity guarantees that this loop will terminate.  In practice, it
    // should usually terminate quickly; on average (assuming all bounds are
    // equally likely), 82.25% of the time, we can expect it to require just
    // one iteration.  In the worst case, someone passes a Bound of 2^31 + 1
    // (i.e., 2147483649), which invalidates almost 50% of the range.  In
    // practice, bounds are typically small and only a tiny amount of the range
    // is eliminated.
    while(true)
    {
      Result = mtb_Random_u32(RNG);
      if(Result >= Threshold)
        break;
    }

    Result %= Bound;
#else
    Result = mtb_Random_u32(RNG) % Bound;
#endif
  }

  return Result;
}

mtb_u32
mtb_RandomBetween_u32(mtb_rng* RNG, mtb_u32 LowerBound, mtb_u32 UpperBound)
{
  mtb_u32 Result = 0;
  if(UpperBound > LowerBound)
  {
    mtb_u32 Rand = mtb_Random_u32(RNG);
    mtb_u32 LocalBound = (UpperBound + 1) - LowerBound;
    Result = LowerBound + (Rand % LocalBound);
  }
  return Result;
}

mtb_f32
mtb_RandomUnilateral_f32(mtb_rng* RNG)
{
  mtb_f32 Divisor = 1.0f / (mtb_f32)MTB_MaxValue_u32;
  mtb_f32 Result = Divisor * (mtb_f32)mtb_Random_u32(RNG);
  return Result;
}

mtb_f32
mtb_RandomBilateral_f32(mtb_rng* RNG)
{
  mtb_f32 Result = 2.0f * mtb_RandomUnilateral_f32(RNG) - 1.0f;
  return Result;
}

mtb_f32
mtb_RandomBetween_f32(mtb_rng* RNG, mtb_f32 LowerBound, mtb_f32 UpperBound)
{
  mtb_f32 Alpha = mtb_RandomUnilateral_f32(RNG);
  mtb_f32 Result = mtb_Lerp(LowerBound, UpperBound, Alpha);
  return Result;
}

#endif // !defined(MTB_IMPL_mtb_rng)
#endif // defined(MTB_RNG_IMPLEMENTATION)
