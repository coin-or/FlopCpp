// ******************** flopc++ **********************************************
// File: flopc.hpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#ifndef _flopc_hpp_
#define _flopc_hpp_

#include "MP_variable.hpp"
#include "MP_set.hpp"
#include "MP_index.hpp"
#include "MP_constant.hpp"
#include "MP_data.hpp"
#include "MP_constraint.hpp"
#include "MP_expression.hpp"
#include "MP_boolean.hpp"
#include "MP_model.hpp"

namespace flopc {

  // Global functions
  inline void forall(const MP_domain& d, const Functor& f) {
    d.Forall(&f);
  }

  inline void forall(const Functor& f) {
    forall(MP_domain::getEmpty(), f);
  }

  inline void operator<<=(const MP_domain& s, const MP_domain& d) {
    d.Forall( s->makeInsertFunctor());
  }

  inline void minimize(const MP_expression &obj) {
    MP_model::getDefaultModel().minimize(obj);
  }

  inline void minimize_max(MP_set& d, const MP_expression &obj) {
    MP_model::getDefaultModel().minimize_max(d,obj);
  }

  inline void maximize(const MP_expression &obj) {
    MP_model::getDefaultModel().maximize(obj);
  }

} // End of namespace flopc
#endif
