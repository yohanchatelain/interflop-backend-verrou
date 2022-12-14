
/*--------------------------------------------------------------------*/
/*--- Verrou: a FPU instrumentation tool.                          ---*/
/*--- Utilities for easier manipulation of floating-point values.  ---*/
/*---                                                vr_nextUlp.hxx ---*/
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

#pragma once
#include <cfloat>
#include <limits>
#include <stdint.h>

#include "interflop-stdlib/interflop_stdlib.h"
#include "interflop_verrou.h"

template <class REALTYPE> inline REALTYPE nextAwayFromZero(REALTYPE a) {
  interflop_panic("nextAwayFromZero called on unknown type");
};

template <> inline double nextAwayFromZero<double>(double a) {
  double res = a;
  uint64_t *resU = reinterpret_cast<uint64_t *>(&res);
  (*resU) += 1;
  return res;
};

template <> inline float nextAwayFromZero<float>(float a) {
  float res = a;
  uint32_t *resU = reinterpret_cast<uint32_t *>(&res);
  (*resU) += 1;
  return res;
};

template <class REALTYPE> inline REALTYPE nextTowardZero(REALTYPE a) {
  interflop_panic("nextTowardZero called on unknown type");
};

template <> inline double nextTowardZero<double>(double a) {
  double res = a;
  uint64_t *resU = reinterpret_cast<uint64_t *>(&res);
  (*resU) -= 1;
  return res;
};

template <> inline float nextTowardZero<float>(float a) {
  float res = a;
  uint32_t *resU = reinterpret_cast<uint32_t *>(&res);
  (*resU) -= 1;
  return res;
};

template <class REALTYPE> inline REALTYPE nextAfter(REALTYPE a) {
  return (a >= 0) ? nextAwayFromZero(a) : nextTowardZero(a);
};

template <class REALTYPE> inline REALTYPE nextPrev(REALTYPE a) {
  return (a > 0)    ? nextTowardZero(a)
         : (a != 0) ? nextAwayFromZero(a)
                    : -std::numeric_limits<REALTYPE>::denorm_min();
};
