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

#ifndef NTCC_RUNTIME_STORE_HPP
#define NTCC_RUNTIME_STORE_HPP

#include <gecode/int.hh>
#include <gecode/search.hh>
#include <gecode/minimodel.hh>

#include <map>
#include <string>
#include <iostream>

class Store : public Gecode::Space
{
  Gecode::IntVarArray constraints;
  Gecode::IntVarArgs constraints_buffer;
public:
  Store() = default;

  template <class ConstraintExpr>
  void entail(const ConstraintExpr& expr)
  {
    Gecode::rel(*this, expr);
  }

  Gecode::IntVar declare(int min, int max)
  {
    Gecode::IntVar var(*this, min, max);
    constraints_buffer << var;
    return var;
  }

  Store(bool share, Store& s) : Gecode::Space(share, s)
  {
    constraints.update(*this, share, s.constraints);
  }

  virtual Gecode::Space* copy(bool share)
  {
    return new Store(share, *this);
  }

  void prepare()
  {
    using namespace Gecode;
    constraints = IntVarArray(*this, constraints_buffer);
    branch(*this, constraints, INT_VAR_NONE(), INT_VAL_MIN());
  }

  void print() const
  {
    std::cout << constraints << std::endl;
  }
};

#endif