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

template <size_t var>
struct Variable{};

template <size_t value>
struct Constant{};

template <class T, class U>
struct Less{};

template <class Constraint>
struct Tell{};

template <class T>
struct Return
{
  T v;
  Return(T v) : v(v){}
  T operator()(Store)
  {
    return v;
  }
};

// template <class Prog, class Cont>    // compile-time type parameters
// struct Bind
// {
//   Bind(Prog prog, Cont cont)
//   : prog(prog), cont(cont) {}

//   auto operator()(Store& store) 
//     -> decltype(cont(prog(store))(store))
//   {
//     return cont(prog(store))(store);
//   }

//   Prog prog;
//   Cont cont;
// };

template <class Exp>
struct Compile;

// template <size_t value>
// struct Compile<Constant<value>>
// : Return<Constant<value>>
// {
//   Compile() : Return(value) {}
// };

template <size_t var>
struct Compile<Variable<var>>
{
  Gecode::IntVar operator()(Store& store)
  {
    return store[var];
  }
};

template <class L, class R>
struct Compile<Less<L, R>>
{
  Gecode::LinIntExpr operator()(Store& store)
  {
    return Compile<L>()(store) < Compile<R>()(store);
  }
};

template <class C>
struct Compile<Tell<C>>
{
  void operator()(Store& store)
  {
    store.entail(Compile<C>()(store));
  }
};

#endif