/*===========================================================================*\
 *                                                                           *
  *      Euler-angle representation and optimization of 3D cross-fields       *
   *    Copyright (C) 2014 by Mesh Generation Group, RWTH Aachen University    *
    *                       www.meshing.rwth-aachen.de                          *
     *             author: David Bommes (bommes@cs.rwth-aachen.de)               *
      *                                                                           *
       *       code based on “Boundary aligned smooth 3D cross-frame field” by     *
        *                    J. Huang, Y. Tong, H. Wei and H. Bao                   *
         *            and optimized for performance with Maplesoft Maple 18          *
          *                                                                           *
          \*===========================================================================*/

/*
written by:
 -- David Bommes (bommes@cs.rwth-aachen.de)
 -- Klaus Leppkes (klaus.leppkes@rwth-aachen.de)
 -- Johannes Lotz (johannes.lotz@rwth-aachen.de)
 -- Jacques du Toit (jacques@nag.co.uk)

This code can be used for benchmarking purposes for computing
adjoints. Whenever using or publishing this code, please contact the
authors first.
*/

#pragma once
#include <cmath>

template<class T> void f_harmonic(const T *x, T& _f);
