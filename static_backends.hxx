#pragma once

#include "vr_op.hxx"
#include "vr_roundingOp.hxx"

template <template <typename> typename Operation, typename Real,
          template <typename> typename RoundingMode>
void binary_op(Real a, Real b, Real *res, void *context) {
  typedef RoundingMode<Operation<Real>> Op;
  *res = Op::apply(Op::PackArgs(a, b));
}

template <>
void binary_op<AddOp, double, RoundingAverage>(double a, double b, double *res,
                                               void *context) {
  typedef RoundingAverage<AddOp<double>> Op;
  *res = Op::apply(Op::PackArgs(a, b));
}

template <template <typename> typename RoundingMode> class StaticRounding {

public:
  static void add_double(double a, double b, double *res, void *context) {
    typedef RoundingMode<AddOp<double>> Op;
    *res = Op::apply(Op::PackArgs(a, b));
  }

  static void add_float(float a, float b, float *res, void *context) {
    // typedef RoundingMode<AddOp<float>> Op;
    *res = RoundingMode<AddOp<float>>::apply(
        RoundingMode<AddOp<float>>::PackArgs(a, b));
  }

  static void sub_double(double a, double b, double *res, void *context) {
    typedef RoundingMode<SubOp<double>> Op;
    *res = Op::apply(Op::PackArgs(a, b));
  }

  static void sub_float(float a, float b, float *res, void *context) {
    typedef RoundingMode<SubOp<float>> Op;
    *res = Op::apply(Op::PackArgs(a, b));
  }

  static void mul_double(double a, double b, double *res, void *context) {
    typedef RoundingMode<MulOp<double>> Op;
    *res = Op::apply(Op::PackArgs(a, b));
  }

  static void mul_float(float a, float b, float *res, void *context) {
    typedef RoundingMode<MulOp<float>> Op;
    *res = Op::apply(Op::PackArgs(a, b));
  }

  static void div_double(double a, double b, double *res, void *context) {
    typedef RoundingMode<DivOp<double>> Op;
    *res = Op::apply(Op::PackArgs(a, b));
  }

  static void div_float(float a, float b, float *res, void *context) {
    typedef RoundingMode<DivOp<float>> Op;
    *res = Op::apply(Op::PackArgs(a, b));
  }

  static void cast_double_to_float(double a, float *res, void *context) {
    typedef RoundingMode<CastOp<double, float>> Op;
    *res = Op::apply(Op::PackArgs(a));
  }

  static void fma_double(double a, double b, double c, double *res,
                         void *context) {
    typedef RoundingMode<MAddOp<double>> Op;
    *res = Op::apply(Op::PackArgs(a, b, c));
  }

  static void fma_float(float a, float b, float c, float *res, void *context) {
    typedef RoundingMode<MAddOp<float>> Op;
    *res = Op::apply(Op::PackArgs(a, b, c));
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
      interflop_user_call : NULL,
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
  interflop_user_call : NULL,
  interflop_finalize : INTERFLOP_VERROU_API(finalize)
};

static struct interflop_backend_interface_t
get_static_backend(verrou_context_t *ctx) {
  switch (ctx->rounding_mode) {
  case VR_NEAREST:
  case VR_NATIVE:
    return StaticRounding<RoundingNearest>::get_backend();
  case VR_UPWARD:
    return StaticRounding<RoundingUpward>::get_backend();
  case VR_DOWNWARD:
    return StaticRounding<RoundingDownward>::get_backend();
  case VR_ZERO:
    return StaticRounding<RoundingZero>::get_backend();
  case VR_RANDOM:
    return StaticRounding<RoundingRandom>::get_backend();
  case VR_RANDOM_DET:
    return StaticRounding<RoundingRandomDet>::get_backend();
  case VR_RANDOM_COMDET:
    return StaticRounding<RoundingAverageComDet>::get_backend();
  case VR_AVERAGE:
    return StaticRounding<RoundingAverage>::get_backend();
  case VR_AVERAGE_DET:
    return StaticRounding<RoundingAverageDet>::get_backend();
  case VR_AVERAGE_COMDET:
    return StaticRounding<RoundingAverageComDet>::get_backend();
  case VR_FARTHEST:
    return StaticRounding<RoundingFarthest>::get_backend();
  case VR_FLOAT:
    return StaticRounding<RoundingFloat>::get_backend();
  case VR_FTZ:
    interflop_panic("FTZ not implemented in backend_verrou");
  default:
    return dynamic_backend;
  }
}