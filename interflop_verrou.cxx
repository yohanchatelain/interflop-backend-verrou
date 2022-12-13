
/*--------------------------------------------------------------------*/
/*--- Verrou: a FPU instrumentation tool.                          ---*/
/*--- Interface for floating-point operations overloading.         ---*/
/*---                                         interflop_verrou.cxx ---*/
/*--------------------------------------------------------------------*/

/*
   This file is part of Verrou, a FPU instrumentation tool.

   Copyright (C) 2014-2021 EDF
     F. Févotte     <francois.fevotte@edf.fr>
     B. Lathuilière <bruno.lathuiliere@edf.fr>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307, USA.

   The GNU Lesser General Public License is contained in the file COPYING.
*/

#include <argp.h>
#include <stddef.h>

#include "interflop-stdlib/fma/interflop_fma.h"
#include "interflop-stdlib/interflop_stdlib.h"
#include "interflop_verrou.h"
#include "vr_nextUlp.hxx"
#include "vr_rand.h"

#include "vr_op.hxx"
#include "vr_roundingOp.hxx"

// * Global variables & parameters
int CHECK_C = 0;
vr_RoundingMode DEFAULTROUNDINGMODE;
vr_RoundingMode ROUNDINGMODE;
unsigned int vr_seed;

static File *stderr_stream;

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef PROFILING_EXACT
unsigned int vr_NumOp;
unsigned int vr_NumExactOp;
#endif

void verrou_init_profiling_exact(void) {
#ifdef PROFILING_EXACT
  vr_NumOp = 0;
  vr_NumExactOp = 0;
#endif
}

void verrou_get_profiling_exact(unsigned int *num, unsigned int *numExact) {
#ifdef PROFILING_EXACT
  *num = vr_NumOp;
  *numExact = vr_NumExactOp;
#endif
}

// * Operation implementation
const char *verrou_rounding_mode_name(enum vr_RoundingMode mode) {
  switch (mode) {
  case VR_NEAREST:
    return "NEAREST";
  case VR_UPWARD:
    return "UPWARD";
  case VR_DOWNWARD:
    return "DOWNWARD";
  case VR_ZERO:
    return "TOWARD_ZERO";
  case VR_RANDOM:
    return "RANDOM";
  case VR_RANDOM_DET:
    return "RANDOM_DET";
  case VR_RANDOM_COMDET:
    return "RANDOM_COMDET";
  case VR_AVERAGE:
    return "AVERAGE";
  case VR_AVERAGE_DET:
    return "AVERAGE_DET";
  case VR_AVERAGE_COMDET:
    return "AVERAGE_COMDET";
  case VR_FARTHEST:
    return "FARTHEST";
  case VR_FLOAT:
    return "FLOAT";
  case VR_NATIVE:
    return "NATIVE";
  case VR_FTZ:
    return "FTZ";
  }

  return "undefined";
}

void interflop_set_seed(u_int64_t seed, void *context) {
  verrou_context_t *ctx = (verrou_context_t *)context;
  ROUNDINGMODE = ctx->rounding_mode;
  DEFAULTROUNDINGMODE = ctx->default_rounding_mode;
  vr_seed = seed;

  if (vr_seed == (unsigned int)-1) {
    struct timeval t1;
    interflop_gettimeofday(&t1, NULL);
    vr_seed = t1.tv_usec + interflop_gettid();
  }
  verrou_set_seed(vr_seed);
}

// * C interface
void INTERFLOP_VERROU_API(configure)(verrou_conf_t conf, void *context) {
  verrou_context_t *ctx = (verrou_context_t *)context;
  ctx->default_rounding_mode = conf.default_rounding_mode;
  ctx->rounding_mode = conf.rounding_mode;
  vr_seed = conf.seed;
  interflop_set_seed(conf.seed, context);
}

const char *INTERFLOP_VERROU_API(get_backend_name)() { return "verrou"; }

const char *INTERFLOP_VERROU_API(get_backend_version)() { return "1.x-dev"; }

void verrou_begin_instr(void *context) {
  verrou_context_t *ctx = (verrou_context_t *)context;
  ctx->rounding_mode = ctx->default_rounding_mode;
}

void verrou_end_instr(void *context) {
  verrou_context_t *ctx = (verrou_context_t *)context;
  ctx->rounding_mode = VR_NEAREST;
}

void verrou_set_seed(unsigned int seed) {
  vr_seed = vr_rand_next(&vr_rand);
  vr_rand_setSeed(&vr_rand, seed);
}

void verrou_set_random_seed() { vr_rand_setSeed(&vr_rand, vr_seed); }

#define IFV_INLINE inline

IFV_INLINE void INTERFLOP_VERROU_API(add_double)(double a, double b,
                                                 double *res, void *context) {
  typedef OpWithSelectedRoundingMode<AddOp<double>> Op;
  Op::apply(Op::PackArgs(a, b), res, context);
}

IFV_INLINE void INTERFLOP_VERROU_API(add_float)(float a, float b, float *res,
                                                void *context) {
  typedef OpWithSelectedRoundingMode<AddOp<float>> Op;
  Op::apply(Op::PackArgs(a, b), res, context);
}

IFV_INLINE void INTERFLOP_VERROU_API(sub_double)(double a, double b,
                                                 double *res, void *context) {
  typedef OpWithSelectedRoundingMode<SubOp<double>> Op;
  Op::apply(Op::PackArgs(a, b), res, context);
}

IFV_INLINE void INTERFLOP_VERROU_API(sub_float)(float a, float b, float *res,
                                                void *context) {
  typedef OpWithSelectedRoundingMode<SubOp<float>> Op;
  Op::apply(Op::PackArgs(a, b), res, context);
}

IFV_INLINE void INTERFLOP_VERROU_API(mul_double)(double a, double b,
                                                 double *res, void *context) {
  typedef OpWithSelectedRoundingMode<MulOp<double>> Op;
  Op::apply(Op::PackArgs(a, b), res, context);
}

IFV_INLINE void INTERFLOP_VERROU_API(mul_float)(float a, float b, float *res,
                                                void *context) {
  typedef OpWithSelectedRoundingMode<MulOp<float>> Op;
  Op::apply(Op::PackArgs(a, b), res, context);
}

IFV_INLINE void INTERFLOP_VERROU_API(div_double)(double a, double b,
                                                 double *res, void *context) {
  typedef OpWithSelectedRoundingMode<DivOp<double>> Op;
  Op::apply(Op::PackArgs(a, b), res, context);
}

IFV_INLINE void INTERFLOP_VERROU_API(div_float)(float a, float b, float *res,
                                                void *context) {
  typedef OpWithSelectedRoundingMode<DivOp<float>> Op;
  Op::apply(Op::PackArgs(a, b), res, context);
}

IFV_INLINE void INTERFLOP_VERROU_API(cast_double_to_float)(double a, float *res,
                                                           void *context) {
  typedef OpWithSelectedRoundingMode<CastOp<double, float>> Op;
  Op::apply(Op::PackArgs(a), res, context);
}

IFV_INLINE void INTERFLOP_VERROU_API(fma_double)(double a, double b, double c,
                                                 double *res, void *context) {
  typedef OpWithSelectedRoundingMode<MAddOp<double>> Op;
  Op::apply(Op::PackArgs(a, b, c), res, context);
}

IFV_INLINE void INTERFLOP_VERROU_API(fma_float)(float a, float b, float c,
                                                float *res, void *context) {
  typedef OpWithSelectedRoundingMode<MAddOp<float>> Op;
  Op::apply(Op::PackArgs(a, b, c), res, context);
}

typedef enum { KEY_ROUNDING_MODE, KEY_SEED } key_args;

static const char key_rounding_mode_str[] = "rounding-mode";
static const char key_seed_str[] = "seed";

static struct argp_option options[] = {
    {key_rounding_mode_str, KEY_ROUNDING_MODE, "ROUNDING MODE", 0,
     "select rounding mode among {nearest, upward, downward, toward_zero, "
     "random, random_det, random_comdet, average, average_det,  "
     "average_comdet, farthest,float,native,ftz}",
     0},
    {key_seed_str, KEY_SEED, "SEED", 0, "fix the random generator seed", 0},
    {0}};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  verrou_context_t *ctx = (verrou_context_t *)state->input;
  int error = 0;
  switch (key) {
  case KEY_ROUNDING_MODE:
    if (interflop_strcasecmp("nearest", arg) == 0) {
      ctx->rounding_mode = VR_NEAREST;
    } else if (interflop_strcasecmp("upward", arg) == 0) {
      ctx->rounding_mode = VR_UPWARD;
    } else if (interflop_strcasecmp("downward", arg) == 0) {
      ctx->rounding_mode = VR_DOWNWARD;
    } else if (interflop_strcasecmp("toward_zero", arg) == 0) {
      ctx->rounding_mode = VR_ZERO;
    } else if (interflop_strcasecmp("random", arg) == 0) {
      ctx->rounding_mode = VR_RANDOM;
    } else if (interflop_strcasecmp("random_det", arg) == 0) {
      ctx->rounding_mode = VR_RANDOM_DET;
    } else if (interflop_strcasecmp("random_comdet", arg) == 0) {
      ctx->rounding_mode = VR_RANDOM_COMDET;
    } else if (interflop_strcasecmp("average", arg) == 0) {
      ctx->rounding_mode = VR_AVERAGE;
    } else if (interflop_strcasecmp("average_det", arg) == 0) {
      ctx->rounding_mode = VR_AVERAGE_DET;
    } else if (interflop_strcasecmp("average_comdet", arg) == 0) {
      ctx->rounding_mode = VR_AVERAGE_COMDET;
    } else if (interflop_strcasecmp("farthest", arg) == 0) {
      ctx->rounding_mode = VR_FARTHEST;
    } else if (interflop_strcasecmp("float", arg) == 0) {
      ctx->rounding_mode = VR_FLOAT;
    } else if (interflop_strcasecmp("native", arg) == 0) {
      ctx->rounding_mode = VR_NATIVE;
    } else if (interflop_strcasecmp("ftz", arg) == 0) {
      ctx->rounding_mode = VR_FTZ;
    } else {
      interflop_fprintf(stderr_stream,
                        "%s invalid value provided, must be one of: "
                        " nearest, upward, downward, toward_zero, random, "
                        "random_det, random_comdet,average, average_det, "
                        "average_comdet,farthest,float,native,ftz.\n",
                        key_rounding_mode_str);
      interflop_exit(42);
    }
    break;

  case KEY_SEED:
    /* seed */
    error = 0;
    char *endptr;
    ctx->seed = (unsigned long)interflop_strtol(arg, &endptr, &error);
    if (error != 0) {
      interflop_fprintf(stderr_stream,
                        "%s invalid value provided, must be an integer\n",
                        key_seed_str);
      interflop_exit(42);
    }
    interflop_set_seed(ctx->seed, ctx);
    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

#define CHECK_IMPL(name)                                                       \
  if (interflop_##name == Null) {                                              \
    interflop_panic("Interflop backend error: " #name " not implemented\n");   \
  }

void _verrou_check_stdlib(void) {
  CHECK_IMPL(exit);
  CHECK_IMPL(fprintf);
  CHECK_IMPL(gettid);
  CHECK_IMPL(gettimeofday);
  CHECK_IMPL(infHandler);
  CHECK_IMPL(malloc);
  CHECK_IMPL(nanHandler);
  CHECK_IMPL(strcasecmp);
  CHECK_IMPL(strtol);
}

void _verrou_alloc_context(void **context) {
  *context = (verrou_context_t *)interflop_malloc(sizeof(verrou_context_t));
}

void _verrou_init_context(verrou_context_t *ctx) {
  ctx->default_rounding_mode = VR_NEAREST;
  ctx->rounding_mode = VR_NEAREST; // default value
  ctx->seed = (unsigned int)-1;
}

void INTERFLOP_VERROU_API(pre_init)(File *stream, interflop_panic_t panic,
                                    void **context) {
  stderr_stream = stream;
  interflop_set_handler("panic", (void *)panic);
  _verrou_check_stdlib();
  _verrou_alloc_context(context);
  verrou_context_t *ctx = (verrou_context_t *)*context;
  _verrou_init_context(ctx);
}

static struct argp argp = {options, parse_opt, "", "", NULL, NULL, NULL};

void INTERFLOP_VERROU_API(CLI)(int argc, char **argv, void *context) {
  verrou_context_t *ctx = (verrou_context_t *)context;
  if (interflop_argp_parse != NULL) {
    interflop_argp_parse(&argp, argc, argv, 0, 0, ctx);
  } else {
    interflop_panic("Interflop backend error: argp_parse not implemented\n"
                    "Provide implementation or use interflop_configure to "
                    "configure the backend\n");
  }

  interflop_fprintf(stderr_stream, "VERROU ROUNDING MODE : %s\n",
                    verrou_rounding_mode_name(ctx->rounding_mode));
}

void INTERFLOP_VERROU_API(finalize)(void *context) {}

IFV_INLINE void vr_random_add_double(double a, double b, double *res,
                                     void *context) {
  typedef RoundingRandom<AddOp<double>> Op;
  *res = Op::apply(Op::PackArgs(a, b));
}

IFV_INLINE void vr_random_add_float(float a, float b, float *res,
                                    void *context) {
  typedef RoundingRandom<AddOp<float>> Op;
  *res = Op::apply(Op::PackArgs(a, b));
}

IFV_INLINE void vr_random_sub_double(double a, double b, double *res,
                                     void *context) {
  typedef RoundingRandom<SubOp<double>> Op;
  *res = Op::apply(Op::PackArgs(a, b));
}

IFV_INLINE void vr_random_sub_float(float a, float b, float *res,
                                    void *context) {
  typedef RoundingRandom<SubOp<float>> Op;
  *res = Op::apply(Op::PackArgs(a, b));
}

IFV_INLINE void vr_random_mul_double(double a, double b, double *res,
                                     void *context) {
  typedef RoundingRandom<MulOp<double>> Op;
  *res = Op::apply(Op::PackArgs(a, b));
}

IFV_INLINE void vr_random_mul_float(float a, float b, float *res,
                                    void *context) {
  typedef RoundingRandom<MulOp<float>> Op;
  *res = Op::apply(Op::PackArgs(a, b));
}

IFV_INLINE void vr_random_div_double(double a, double b, double *res,
                                     void *context) {
  typedef RoundingRandom<DivOp<double>> Op;
  *res = Op::apply(Op::PackArgs(a, b));
}

IFV_INLINE void vr_random_div_float(float a, float b, float *res,
                                    void *context) {
  typedef RoundingRandom<DivOp<float>> Op;
  *res = Op::apply(Op::PackArgs(a, b));
}

IFV_INLINE void vr_random_cast_double_to_float(double a, float *res,
                                               void *context) {
  typedef RoundingRandom<CastOp<double, float>> Op;
  *res = Op::apply(Op::PackArgs(a));
}

IFV_INLINE void vr_random_fma_double(double a, double b, double c, double *res,
                                     void *context) {
  typedef RoundingRandom<MAddOp<double>> Op;
  *res = Op::apply(Op::PackArgs(a, b, c));
}

IFV_INLINE void vr_random_fma_float(float a, float b, float c, float *res,
                                    void *context) {
  typedef RoundingRandom<MAddOp<float>> Op;
  *res = Op::apply(Op::PackArgs(a, b, c));
}

static struct interflop_backend_interface_t
static_backend(verrou_context_t *ctx) {
  switch (ctx->rounding_mode) {
  // case VR_NEAREST:
  //   return RoundingNearest<OP>::apply(p);
  // case VR_UPWARD:
  //   return RoundingUpward<OP>::apply(p);
  // case VR_DOWNWARD:
  //   return RoundingDownward<OP>::apply(p);
  // case VR_ZERO:
  //   return RoundingZero<OP>::apply(p);
  case VR_RANDOM:
    return {
      interflop_add_float : vr_random_add_float,
      interflop_sub_float : vr_random_sub_float,
      interflop_mul_float : vr_random_mul_float,
      interflop_div_float : vr_random_div_float,
      interflop_cmp_float : NULL,
      interflop_add_double : vr_random_add_double,
      interflop_sub_double : vr_random_sub_double,
      interflop_mul_double : vr_random_mul_double,
      interflop_div_double : vr_random_div_double,
      interflop_cmp_double : NULL,
      interflop_cast_double_to_float : vr_random_cast_double_to_float,
      interflop_fma_float : vr_random_fma_float,
      interflop_fma_double : vr_random_fma_double,
      interflop_enter_function : NULL,
      interflop_exit_function : NULL,
      interflop_user_call : NULL,
      interflop_finalize : INTERFLOP_VERROU_API(finalize)
    };
  // case VR_RANDOM_DET:
  //   return RoundingRandomDet<OP>::apply(p);
  // case VR_RANDOM_COMDET:
  //   return RoundingRandomComDet<OP>::apply(p);
  // case VR_AVERAGE:
  //   return RoundingAverage<OP>::apply(p);
  // case VR_AVERAGE_DET:
  //   return RoundingAverageDet<OP>::apply(p);
  // case VR_AVERAGE_COMDET:
  //   return RoundingAverageComDet<OP>::apply(p);
  // case VR_FARTHEST:
  //   return RoundingFarthest<OP>::apply(p);
  // case VR_FLOAT:
  //   return RoundingFloat<OP>::apply(p);
  // case VR_NATIVE:
  //   return RoundingNearest<OP>::apply(p);
  // case VR_FTZ:
  //   interflop_panic("FTZ not implemented in backend_verrou");
  default:
    return {
      interflop_add_float : INTERFLOP_VERROU_API(add_float),
      interflop_sub_float : INTERFLOP_VERROU_API(sub_float),
      interflop_mul_float : INTERFLOP_VERROU_API(mul_float),
      interflop_div_float : INTERFLOP_VERROU_API(div_float),
      interflop_cmp_float : NULL,
      interflop_add_double : INTERFLOP_VERROU_API(add_double),
      interflop_sub_double : INTERFLOP_VERROU_API(sub_double),
      interflop_mul_double : INTERFLOP_VERROU_API(mul_double),
      interflop_div_double : INTERFLOP_VERROU_API(div_double),
      interflop_cmp_double : NULL,
      interflop_cast_double_to_float :
          INTERFLOP_VERROU_API(cast_double_to_float),
      interflop_fma_float : INTERFLOP_VERROU_API(fma_float),
      interflop_fma_double : INTERFLOP_VERROU_API(fma_double),
      interflop_enter_function : NULL,
      interflop_exit_function : NULL,
      interflop_user_call : NULL,
      interflop_finalize : INTERFLOP_VERROU_API(finalize)
    };
  }
}

struct interflop_backend_interface_t INTERFLOP_VERROU_API(init)(void *context) {
  verrou_context_t *ctx = (verrou_context_t *)context;

  struct interflop_backend_interface_t interflop_verrou_backend =
      static_backend(ctx);
  // {
  //   interflop_add_float : INTERFLOP_VERROU_API(add_float),
  //   interflop_sub_float : INTERFLOP_VERROU_API(sub_float),
  //   interflop_mul_float : INTERFLOP_VERROU_API(mul_float),
  //   interflop_div_float : INTERFLOP_VERROU_API(div_float),
  //   interflop_cmp_float : NULL,
  //   interflop_add_double : INTERFLOP_VERROU_API(add_double),
  //   interflop_sub_double : INTERFLOP_VERROU_API(sub_double),
  //   interflop_mul_double : INTERFLOP_VERROU_API(mul_double),
  //   interflop_div_double : INTERFLOP_VERROU_API(div_double),
  //   interflop_cmp_double : NULL,
  //   interflop_cast_double_to_float :
  //   INTERFLOP_VERROU_API(cast_double_to_float), interflop_fma_float :
  //   INTERFLOP_VERROU_API(fma_float), interflop_fma_double :
  //   INTERFLOP_VERROU_API(fma_double), interflop_enter_function : NULL,
  //   interflop_exit_function : NULL,
  //   interflop_user_call : NULL,
  //   interflop_finalize : INTERFLOP_VERROU_API(finalize)
  // };
  interflop_set_seed(ctx->seed, ctx);
  return interflop_verrou_backend;
}

struct interflop_backend_interface_t interflop_init(void *context)
    __attribute__((weak, alias("interflop_verrou_init")));

void interflop_pre_init(File *stream, interflop_panic_t panic, void **context)
    __attribute__((weak, alias("interflop_verrou_pre_init")));

void interflop_CLI(int argc, char **argv, void *context)
    __attribute__((weak, alias("interflop_verrou_CLI")));

#if defined(__cplusplus)
}
#endif