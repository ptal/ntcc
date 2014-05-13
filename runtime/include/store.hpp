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
#include <stack>

namespace fun{

template <class Container>
struct last
{
  using value_type = typename Container::value_type;
  value_type& operator()(Container& container) const
  {
    return container.top();
  }
  const value_type& operator()(const Container& container) const
  {
    return container.top();
  }
};

}

struct FiniteIntegerDomain
{
  int min;
  int max;

  FiniteIntegerDomain(int min, int max)
  : min(min), max(max)
  {}
};

class SymbolTable
{
public:
  using value_type = FiniteIntegerDomain;
  using scoping_stack = std::stack<value_type>;
  using iterator = 
    boost::transform_iterator<fun::last<scoping_stack>, 
      std::vector<scoping_stack>::iterator>;

 private:
  std::vector<scoping_stack> variables;

 public:
  value_type& operator[](size_t i)
  {
    assert(variables.size() > i);
    assert(variables[i].size() > 0);
    variables[i].top();
  }

  const value_type& operator[](size_t i) const
  {
    assert(variables.size() > i);
    assert(variables[i].size() > 0);
    variables[i].top();
  }

  void declare(size_t i, const value_type& value)
  {
    // Extend the symbol table if it is a new variable.
    if(variables.size() <= i)
      variables.resize(i+1);
    variables[i].push(value);
  }

  void unstack(size_t i)
  {
    assert(variables.size() > i);
    assert(variables[i].size() > 0);
    variables[i].pop();
  }

  size_t size() const
  {
    return variables.size();
  }

  iterator begin()
  {
    return boost::make_transform_iterator<fun::last<scoping_stack>>(variables.begin());
  }

  iterator end()
  {
    return boost::make_transform_iterator<fun::last<scoping_stack>>(variables.end());
  }
};

class StoreSpace : public Gecode::Space
{
  Gecode::IntVarArray constraints;

public:
  template <class Iterator>
  StoreSpace(Iterator b, Iterator e)
  : constraints(b, e)
  {}

  StoreSpace(bool share, StoreSpace& s) : Gecode::Space(share, s)
  {
    constraints.update(*this, share, s.constraints);
  }

  virtual Gecode::Space* copy(bool share)
  {
    return new StoreSpace(share, *this);
  }

  Gecode::IntVar& var(size_t i)
  {
    return constraints[i];
  }

  void print() const
  {
    std::cout << constraints << std::endl;
  }
};

class Constraint;

class Store
{
  SymbolTable variables;
  std::unique_ptr<StoreSpace> transient_store;
  std::vector<std::unique_ptr<Constraint>> constraints;

public:
  Store() = default;

  void tell(std::unique_ptr<Constraint>&& constraint)
  {
    constraints.push_back(std::move(constraint));
  }

  StoreSpace& space()
  {
    assert(transient_store);
    return *transient_store;
  }

  void declare(size_t var, const FiniteIntegerDomain& domain)
  {
    std::cout << "Declare variable " << var << "[" 
      << domain.min << ".." << domain.max << "]" << std::endl;
    variables.declare(var, domain);
  }

  void unstack(size_t var)
  {
    std::cout << "Destroy variable " << var << std::endl;
    variables.unstack(var);
  }

  Gecode::IntVar& var(size_t i)
  {
    assert(transient_store);
    return transient_store->var(i);
  }

 //  template <class Constraint>
 //  bool ask(const Constraint& expr)
 //  {
 //    Store s(true, *this);
 //    return s.ask_impl(expr);
 //  }

 // private:
 //  template <class ConstraintExpr>
 //  bool ask_impl(const ConstraintExpr& expr)
 //  {
 //    using namespace Gecode;
 //    entail(expr);
 //    IntVarArgs gvars(variables.begin(), variables.end());
 //    constraints = IntVarArray(*this, gvars);
 //    branch(*this, constraints, INT_VAR_SIZE_MAX(), INT_VAL_SPLIT_MIN());
 //    DFS<Store> e(this);
 //    std::unique_ptr<Store> s(e.next());
 //    return static_cast<bool>(s);
 //  }
};

#endif