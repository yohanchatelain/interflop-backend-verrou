#pragma once

#include "vr_op.hxx"
#include "vr_roundingOp.hxx"

template <typename> class Void {};

template <template <typename O, typename R> typename RoundingMode,
          template <typename T> typename RAND = Void>
class StaticRounding {
  using AD = AddOp<double>;
  using AF = AddOp<float>;
  using SD = SubOp<double>;
  using SF = SubOp<float>;
  using MD = MulOp<double>;
  using MF = MulOp<float>;
  using DD = DivOp<double>;
  using DF = DivOp<float>;
  using CDF = CastOp<double, float>;
  using FD = MAddOp<double>;
  using FF = MAddOp<float>;

public:
  static void add_double(double a, double b, double *res, void *context) {
    // typedef typename RoundingMode<AD, RAND<AD>> Op;
    using Op = RoundingMode<AD, RAND<AD>>;
    *res = Op::apply(typename Op::PackArgs(a, b));
  }

  static void add_float(float a, float b, float *res, void *context) {
    using Op = RoundingMode<AF, RAND<AF>>;
    *res = Op::apply(typename Op::PackArgs(a, b));
  }

  static void sub_double(double a, double b, double *res, void *context) {
    using Op = RoundingMode<SD, RAND<SD>>;
    *res = Op::apply(typename Op::PackArgs(a, b));
  }

  static void sub_float(float a, float b, float *res, void *context) {
    using Op = RoundingMode<SF, RAND<SF>>;
    *res = Op::apply(typename Op::PackArgs(a, b));
  }

  static void mul_double(double a, double b, double *res, void *context) {
    using Op = RoundingMode<MD, RAND<MD>>;
    *res = Op::apply(typename Op::PackArgs(a, b));
  }

  static void mul_float(float a, float b, float *res, void *context) {
    using Op = RoundingMode<MF, RAND<MF>>;
    *res = Op::apply(typename Op::PackArgs(a, b));
  }

  static void div_double(double a, double b, double *res, void *context) {
    using Op = RoundingMode<DD, RAND<DD>>;
    *res = Op::apply(typename Op::PackArgs(a, b));
  }

  static void div_float(float a, float b, float *res, void *context) {
    using Op = RoundingMode<DF, RAND<DF>>;
    *res = Op::apply(typename Op::PackArgs(a, b));
  }

  static void cast_double_to_float(double a, float *res, void *context) {
    using Op = RoundingMode<CDF, RAND<CDF>>;
    *res = Op::apply(typename Op::PackArgs(a));
  }

  static void fma_double(double a, double b, double c, double *res,
                         void *context) {
    using Op = RoundingMode<FD, RAND<FD>>;
    *res = Op::apply(typename Op::PackArgs(a, b, c));
  }

  static void fma_float(float a, float b, float c, float *res, void *context) {
    using Op = RoundingMode<FF, RAND<FF>>;
    *res = Op::apply(typename Op::PackArgs(a, b, c));
  }

  static struct interflop_backend_interface_t get_backend(void) {
    return {
      interflop_add_float : add_float,
      interflop_sub_float : sub_float,
      interflop_mul_float : mul_float,
      interflop_div_float : div_float,
      interflop_cmp_float : NULL,
      interflop_add_double : add_double,
      interflop_sub_double : sub_double,
      interflop_mul_double : mul_double,
      interflop_div_double : div_double,
      interflop_cmp_double : NULL,
      interflop_cast_double_to_float : cast_double_to_float,
      interflop_fma_float : fma_float,
      interflop_fma_double : fma_double,
      interflop_enter_function : NULL,
      interflop_exit_function : NULL,
      interflop_user_call : INTERFLOP_VERROU_API(user_call),
      interflop_finalize : INTERFLOP_VERROU_API(finalize)
    };
  }
};

interflop_backend_interface_t dynamic_backend = {
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
  interflop_cast_double_to_float : INTERFLOP_VERROU_API(cast_double_to_float),
  interflop_fma_float : INTERFLOP_VERROU_API(fma_float),
  interflop_fma_double : INTERFLOP_VERROU_API(fma_double),
  interflop_enter_function : NULL,
  interflop_exit_function : NULL,
  interflop_user_call : INTERFLOP_VERROU_API(user_call),
  interflop_finalize : INTERFLOP_VERROU_API(finalize)
};

static struct interflop_backend_interface_t
get_static_backend(verrou_context_t *ctx) {
  switch (ctx->rounding_mode) {
  case VR_NEAREST:
    return StaticRounding<RoundingNearest>::get_backend();
  case VR_UPWARD:
    return StaticRounding<RoundingUpward>::get_backend();
  case VR_DOWNWARD:
    return StaticRounding<RoundingDownward>::get_backend();
  case VR_ZERO:
    return StaticRounding<RoundingZero>::get_backend();
  case VR_RANDOM:
    return StaticRounding<RoundingRandom, vr_rand_prng>::get_backend();
  case VR_RANDOM_DET:
    return StaticRounding<RoundingRandom, vr_rand_det>::get_backend();
  case VR_RANDOM_COMDET:
    return StaticRounding<RoundingRandom, vr_rand_comdet>::get_backend();
  case VR_AVERAGE:
    return StaticRounding<RoundingAverage, vr_rand_prng>::get_backend();
  case VR_AVERAGE_DET:
    return StaticRounding<RoundingAverage, vr_rand_det>::get_backend();
  case VR_AVERAGE_COMDET:
    return StaticRounding<RoundingAverage, vr_rand_comdet>::get_backend();
  case VR_PRANDOM:
    return StaticRounding<RoundingPRandom, vr_rand_prng>::get_backend();
  case VR_PRANDOM_DET:
    return StaticRounding<RoundingPRandom, vr_rand_det>::get_backend();
  case VR_PRANDOM_COMDET:
    return StaticRounding<RoundingPRandom, vr_rand_comdet>::get_backend();
  case VR_FARTHEST:
    return StaticRounding<RoundingFarthest>::get_backend();
  case VR_FLOAT:
    return StaticRounding<RoundingFloat>::get_backend();
  case VR_NATIVE:
    return StaticRounding<RoundingNearest>::get_backend();
  case VR_FTZ:
    interflop_panic("FTZ not implemented in backend_verrou");
  default:
    return dynamic_backend;
  }
}