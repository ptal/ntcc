// Copyright 2014 IRCAM (Institut de Recherche et Coordination Acoustique/Musique)
// Copyright 2014 Pierre Talbot <ptalbot@hyc.io>

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef NTCC_RUNTIME_HPP
#define NTCC_RUNTIME_HPP

#include "store.hpp"


struct Variable
{
  size_t n;
  Variable(size_t n) : n(n) {}

  Gecode::IntVar& operator()(Store& store) const
  {
    return store.var(n);
  }
};

struct Constant
{
  int n;
  Constant(int n) : n(n) {}

  int operator()(Store&) const
  {
    return n;
  }
};

struct ConstraintBase
{
  virtual void tell(Store&) = 0;
  virtual ~ConstraintBase(){}
};

template <class L, class R>
struct Less : ConstraintBase
{
  L lhs;
  R rhs;

  Less(L&& lhs, R&& rhs)
  : lhs(lhs), rhs(rhs){}

  virtual void tell(Store& store)
  {
    Gecode::rel(store.space(), lhs(store), Gecode::IRT_LE, rhs(store));
  }

  virtual ~Less(){}
};

template <class Program>
struct Local
{
  size_t var;
  FiniteIntegerDomain domain;
  Program program;

  Local(size_t var, FiniteIntegerDomain&& domain, Program&& program)
  : var(var), domain(domain), program(program)
  {}

  void operator()(Store& store)
  {
    store.declare(var, domain);
    program(store);
    store.unstack(var);
  }
};

struct Skip
{
  void operator()(Store&){} 
};

template <class Constraint>
struct Tell
{
  Constraint constraint;

  Tell(Constraint&& constraint)
  : constraint(constraint)
  {}

  void operator()(Store& store)
  {
    store.tell(constraint);
  }
};

#endif