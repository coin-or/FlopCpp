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
     
    void forall(const MP_domain& d, const Functor& f) {
	d.Forall(&f);
    }

    void forall(const Functor& f) {
	forall(MP_domain::Empty, f);
    }

    void operator<<=(const MP_domain& s, const MP_domain& d) {
	d.Forall( s->makeInsertFunctor());
    }

    void minimize(const MP_expression &obj) {
	MP_model::default_model.minimize(obj);
    }

    void minimize_max(MP_set& d, const MP_expression &obj) {
	MP_model::default_model.minimize_max(d,obj);
    }

    void maximize(const MP_expression &obj) {
	MP_model::default_model.maximize(obj);
    }

} // End of namespace flopc
#endif
