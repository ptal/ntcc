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

struct Constraint
{
  virtual void tell(Store&) = 0;
  virtual ~Constraint(){}
};

template <class L, class R>
struct Less : Constraint
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

template <class L, class R>
std::unique_ptr<Less<L,R>> less(L&& lhs, R&& rhs)
{
  return std::unique_ptr<Less<L,R>>(new Less<L,R>(std::move(lhs), std::move(rhs)));
}

struct Program
{
  // Execute internal transition.
  virtual std::unique_ptr<Program> internal_run(Store&) = 0;
  virtual ~Program(){}
};

struct Local : Program
{
  size_t var;
  FiniteIntegerDomain domain;
  std::unique_ptr<Program> program;

  Local(size_t var, FiniteIntegerDomain&& domain, 
    std::unique_ptr<Program>&& program)
  : var(var), domain(domain), program(std::move(program))
  {}

  virtual std::unique_ptr<Program> internal_run(Store& store)
  {
    store.declare(var, domain);
    std::unique_ptr<Program> residual = program->internal_run(store);
    store.unstack(var);
    return residual;
  }

  virtual ~Local(){}
};

struct Skip : Program
{
  virtual std::unique_ptr<Program> internal_run(Store&)
  {
    return nullptr;
  }

  virtual ~Skip(){}
};

struct Tell : Program
{
  std::unique_ptr<Constraint> constraint;

  Tell(std::unique_ptr<Constraint>&& constraint)
  : constraint(std::move(constraint))
  {}

  virtual std::unique_ptr<Program> internal_run(Store& store)
  {
    assert(constraint);
    store.tell(std::move(constraint));
    return nullptr;
  }

  virtual ~Tell(){}
};

std::unique_ptr<Local> local(size_t var, FiniteIntegerDomain&& domain, 
  std::unique_ptr<Program>&& program)
{
  return std::unique_ptr<Local>(new Local(var, std::move(domain), std::move(program)));
}

std::unique_ptr<Skip> skip()
{
  return std::unique_ptr<Skip>(new Skip());
}

std::unique_ptr<Tell> tell(std::unique_ptr<Constraint>&& constraint)
{
  return std::unique_ptr<Tell>(new Tell(std::move(constraint)));
}

#endif