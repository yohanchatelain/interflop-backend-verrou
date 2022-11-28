
/*--------------------------------------------------------------------*/
/*--- Verrou: a FPU instrumentation tool.                          ---*/
/*--- Interface for floatin-point operations overloading.          ---*/
/*---                                           interflop_verrou.h ---*/
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

#ifndef __INTERFLOP_VERROU_H
#define __INTERFLOP_VERROU_H

#ifdef __cplusplus
extern "C" {
#endif
#define INTERFLOP_VERROU_API(FCT) interflop_verrou_##FCT

#include "common/interflop.h"
#include "interflop-stdlib/interflop_stdlib.h"

enum vr_RoundingMode {
  VR_NEAREST,
  VR_UPWARD,
  VR_DOWNWARD,
  VR_ZERO,
  VR_RANDOM, // Must be immediately after standard rounding modes
  VR_RANDOM_DET,
  VR_RANDOM_COMDET,
  VR_AVERAGE,
  VR_AVERAGE_DET,
  VR_AVERAGE_COMDET,
  VR_FARTHEST,
  VR_FLOAT,
  VR_NATIVE,
  VR_FTZ
};

typedef struct {
  enum vr_RoundingMode default_rounding_mode;
  enum vr_RoundingMode rounding_mode;
  unsigned int seed;
} verrou_context_t;

typedef verrou_context_t verrou_conf_t;

void INTERFLOP_VERROU_API(configure)(verrou_conf_t conf, void *context);
void INTERFLOP_VERROU_API(finalize)(void *context);

const char *INTERFLOP_VERROU_API(get_backend_name)(void);
const char *INTERFLOP_VERROU_API(get_backend_version)(void);

const char *verrou_rounding_mode_name(enum vr_RoundingMode mode);

void verrou_begin_instr(void *context);
void verrou_end_instr(void *context);

void verrou_set_seed(unsigned int seed);
void verrou_set_random_seed(void);

void verrou_init_profiling_exact(void);
void verrou_get_profiling_exact(unsigned int *num, unsigned int *numExact);

void INTERFLOP_VERROU_API(pre_init)(File *stream, interflop_panic_t panic,
                                    void **context);
struct interflop_backend_interface_t INTERFLOP_VERROU_API(init)(void *context);

void INTERFLOP_VERROU_API(add_double)(double a, double b, double *res,
                                      void *context);
void INTERFLOP_VERROU_API(add_float)(float a, float b, float *res,
                                     void *context);
void INTERFLOP_VERROU_API(sub_double)(double a, double b, double *res,
                                      void *context);
void INTERFLOP_VERROU_API(sub_float)(float a, float b, float *res,
                                     void *context);
void INTERFLOP_VERROU_API(mul_double)(double a, double b, double *res,
                                      void *context);
void INTERFLOP_VERROU_API(mul_float)(float a, float b, float *res,
                                     void *context);
void INTERFLOP_VERROU_API(div_double)(double a, double b, double *res,
                                      void *context);
void INTERFLOP_VERROU_API(div_float)(float a, float b, float *res,
                                     void *context);

void INTERFLOP_VERROU_API(cast_double_to_float)(double a, float *b,
                                                void *context);

void INTERFLOP_VERROU_API(madd_double)(double a, double b, double c,
                                       double *res, void *context);
void INTERFLOP_VERROU_API(madd_float)(float a, float b, float c, float *res,
                                      void *context);

void INTERFLOP_VERROU_API(finalize)(void *context);

#ifdef __cplusplus
}
#endif

#endif /* ndef __INTERFLOP_VERROU_H */
