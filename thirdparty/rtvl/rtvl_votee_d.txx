/* Copyright 2007-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_votee_d_txx
#define rtvl_votee_d_txx

#include "rtvl_votee_d.hxx"

#include "rtvl_vote.hxx"

#include <vnl/vnl_matrix_fixed.h>

template <unsigned int N>
rtvl_votee_d<N>::rtvl_votee_d(vnl_vector_fixed<double, N> const& votee_location,
  vnl_matrix_fixed<double, N, N>& votee_tensor, vnl_matrix_fixed<double, N, N> (&votee_dtensor)[N])
  : derived(votee_location, votee_tensor)
  , dtensor_(votee_dtensor)
{
}

template <unsigned int N>
void rtvl_votee_d<N>::go(rtvl_vote_internal<N>& vi, double saliency)
{
  this->derived::go(vi, saliency);

  vnl_matrix_fixed<double, N, N> dvote[N];
  rtvl_vote_component_d(vi, dvote);
  for (unsigned int k = 0; k < N; ++k)
  {
    dvote[k] *= saliency;
  }
  for (unsigned int k = 0; k < N; ++k)
  {
    this->dtensor_[k] += dvote[k];
  }
}

#define RTVL_VOTEE_D_INSTANTIATE(N) template class rtvl_votee_d<N>

#endif
