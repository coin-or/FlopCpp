// ******************** flopc++ **********************************************
// File: flopc.cpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#include "flopc.hpp"

namespace flopc {

  MP_index& MP_index::Empty = *new MP_index();
  MP_index& MP_index::Any = *new MP_index();
  MP_index_exp MP_index_exp::Empty =  *new MP_index_exp(Constant(0.0));

  double MP_data::outOfBoundData = 0;

  const MP_domain& MP_domain::Empty =
  new MP_domain_set(&MP_set::Empty,&MP_index::Empty);

  MP_set MP_set::Empty = *new MP_set(1);

  MP_model& MP_model::default_model = *new MP_model(0);
  MP_model* MP_model::current_model = &MP_model::default_model;

}
