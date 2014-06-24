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

#include <boost/iterator/transform_iterator.hpp>

#include <gecode/int.hh>
#include <gecode/search.hh>
#include <gecode/minimodel.hh>

#include <map>
#include <string>
#include <iostream>
#include <memory>

namespace fun{

template <class Container>
struct last
{
  using value_type = typename Container::value_type;
  value_type& operator()(Container& container) const
  {
    return container.back();
  }
  const value_type& operator()(const Container& container) const
  {
    return container.back();
  }
};

}

class ScopedVariables
{
public:
  using value_type = Gecode::IntVar;
  using sequence = std::vector<value_type>;
  using iterator = 
    boost::transform_iterator<fun::last<sequence>, 
      std::vector<sequence>::iterator>;

 private:
  std::vector<sequence> variables;

 public:
  value_type& operator[](size_t i)
  {
    variables[i].back();
  }

  const value_type& operator[](size_t i) const
  {
    variables[i].back();
  }

  void hide(size_t i, const value_type& value)
  {
    variables[i].push_back(value);
  }

  void push_back(const value_type& value)
  {
    variables.push_back({value});
  }

  size_t size() const
  {
    return variables.size();
  }

  iterator begin()
  {
    return boost::make_transform_iterator<fun::last<sequence>>(variables.begin());
  }

  iterator end()
  {
    return boost::make_transform_iterator<fun::last<sequence>>(variables.end());
  }
};

class Store : public Gecode::Space
{
  ScopedVariables variables;
  Gecode::IntVarArray constraints;
public:
  Store() = default;

  template <class ConstraintExpr>
  void entail(const ConstraintExpr& expr)
  {
    Gecode::rel(*this, expr);
  }

  size_t declare(int min, int max)
  {
    variables.push_back(Gecode::IntVar(*this, min, max));
    return variables.size() - 1;
  }

  Gecode::IntVar const& operator[](size_t i) const
  {
    return variables[i];
  }

  Gecode::IntVar& operator[](size_t i)
  {
    return variables[i];
  }

  Store(bool share, Store& s) : Gecode::Space(share, s)
  {
    constraints.update(*this, share, s.constraints);
  }

  virtual Gecode::Space* copy(bool share)
  {
    return new Store(share, *this);
  }

  template <class ConstraintExpr>
  bool ask(const ConstraintExpr& expr)
  {
    std::unique_ptr<Store> s(new Store(true, *this));
    return s->ask_impl(expr);
  }

  void print() const
  {
    std::cout << constraints << std::endl;
  }

 private:
  template <class ConstraintExpr>
  bool ask_impl(const ConstraintExpr& expr)
  {
    using namespace Gecode;
    entail(expr);
    IntVarArgs gvars(variables.begin(), variables.end());
    constraints = IntVarArray(*this, gvars);
    branch(*this, constraints, INT_VAR_NONE(), INT_VAL_MIN());
    return true;
    // DFS<Store> e(this);
    // std::unique_ptr<Store> s(e.next());
    // return static_cast<bool>(s);
  }
};

#endif