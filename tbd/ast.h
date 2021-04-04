#define a(x) b(x)
#define b(x) #x
static_assert(__cplusplus > 201103L, a(__cplusplus));

// Copyright (c) 2018, Benjamin Shropshire,
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef TBD_AST_H_
#define TBD_AST_H_

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "absl/base/attributes.h"
#include "absl/memory/memory.h"
#include "absl/strings/str_cat.h"
#include "tbd/dimensions.h"

namespace tbd {
// Generaric location type.
struct Loc {
  template <class L>
  Loc(L l)
      : filename(*l.begin.filename),
        line_begin(l.begin.line),
        line_end(l.end.line),
        col_begin(l.begin.column),
        col_end(l.end.column) {}
  Loc() = default;

  friend ::std::ostream& operator<<(::std::ostream& os, const Loc& l) {
    if (l.line_begin == l.line_end) {
      return os << l.filename << ":" << l.line_begin << ":[" << l.col_begin
                << "," << l.col_end << "]";
    } else {
      return os << l.filename << ":" << l.line_begin << ":" << l.col_begin
                << " to " << l.line_end << ":" << l.col_end;
    }
  }

  std::string line() const { return absl::StrCat(filename, ":", line_begin); }

  std::string filename = "??";
  int line_begin = 0, line_end = 0;
  int col_begin = 0, col_end = 0;
};

bool operator==(const Loc&, const Loc&);
bool operator<(const Loc&, const Loc&);

Loc Join(std::vector<Loc> loc);

template <class... T>
Loc Join(const Loc& l, const T&... v) {
  return Join({l, Loc{v}...});
}

class VisitNodes;
class LiteralValue;

// An AST node with a location that can be logged
class NodeI {
 public:
  explicit NodeI(Loc loc) : loc_(std::move(loc)) {}
  virtual ~NodeI() = default;

  const Loc& location() const { return loc_; }
  void set_location(const Loc& l) { loc_ = l; }
  const std::string& source_file() const { return loc_.filename; }
  int source_line() const { return loc_.line_begin; }

  ABSL_MUST_USE_RESULT bool VisitNode(VisitNodes* v) const { return Visit(v); }

 private:
  ABSL_MUST_USE_RESULT virtual bool Visit(VisitNodes*) const = 0;

  Loc loc_;
};

struct StableNodeCompare {
  bool operator()(NodeI const* l, NodeI const* r) const;
};

class ExpressionNode : public NodeI {
 public:
  using NodeI::NodeI;
};

class BinaryExpression : public ExpressionNode {
 public:
  BinaryExpression(std::unique_ptr<ExpressionNode> l,
                   std::unique_ptr<ExpressionNode> r);

  ExpressionNode* left() const { return l_.get(); }
  ExpressionNode* right() const { return r_.get(); }

 private:
  std::unique_ptr<ExpressionNode> l_, r_;
};

class UnitExp final : public NodeI {
  ABSL_MUST_USE_RESULT bool Visit(VisitNodes*) const override;

 public:
  using NodeI::NodeI;

  struct UnitT;
  void Mul(std::unique_ptr<UnitT> o);
  void Div(std::unique_ptr<UnitT> o);

  struct UnitT {
    std::string id;
    int exp;
    Loc loc;
  };

  const std::vector<UnitT>& bits() const { return bits_; }

 private:
  std::vector<UnitT> bits_;
};

class UnitDef final : public NodeI {
  ABSL_MUST_USE_RESULT bool Visit(VisitNodes*) const override;

 public:
  using NodeI::NodeI;
  UnitDef(Loc loc, std::unique_ptr<std::string>, std::unique_ptr<LiteralValue>,
          std::unique_ptr<UnitExp>);

  const std::string& name() const { return name_; }
  const float value() const { return value_; }
  const UnitExp& unit() const { return *unit_; }

 private:
  std::string name_;
  double value_;
  std::unique_ptr<UnitExp> unit_;
};

class Equality final : public ExpressionNode {
  ABSL_MUST_USE_RESULT bool Visit(VisitNodes*) const override;

 public:
  Equality(Loc loc, std::unique_ptr<ExpressionNode> l,
           std::unique_ptr<ExpressionNode> r);

  ExpressionNode* left() const { return l_.get(); }
  ExpressionNode* right() const { return r_.get(); }

 private:
  std::unique_ptr<ExpressionNode> l_, r_;
};

class LiteralValue final : public ExpressionNode {
  ABSL_MUST_USE_RESULT bool Visit(VisitNodes*) const override;

 public:
  LiteralValue(Loc loc, int i) : ExpressionNode(loc), value_(i) {}
  LiteralValue(Loc loc, double f) : ExpressionNode(loc), value_(f) {}

  double value() const { return value_; }

 private:
  double value_;
};

class NamedValue final : public ExpressionNode {
  ABSL_MUST_USE_RESULT bool Visit(VisitNodes*) const override;

 public:
  NamedValue(Loc loc, std::unique_ptr<std::string> s);

  std::string name() const { return name_; }

  NamedValue(Loc loc, std::string s);  // for testing

 private:
  std::string name_;
};

class PowerExp final : public ExpressionNode {
  ABSL_MUST_USE_RESULT bool Visit(VisitNodes*) const override;

 public:
  PowerExp(Loc loc, std::unique_ptr<ExpressionNode> b, int e);

  ExpressionNode* base() const { return b_.get(); }
  int exp() const { return e_; }

 private:
  std::unique_ptr<ExpressionNode> b_;
  int e_;
};

class ProductExp final : public BinaryExpression {
  ABSL_MUST_USE_RESULT bool Visit(VisitNodes*) const override;

 public:
  using BinaryExpression::BinaryExpression;
};

class QuotientExp final : public BinaryExpression {
  ABSL_MUST_USE_RESULT bool Visit(VisitNodes*) const override;

 public:
  using BinaryExpression::BinaryExpression;
};

class SumExp final : public BinaryExpression {
  ABSL_MUST_USE_RESULT bool Visit(VisitNodes*) const override;

 public:
  using BinaryExpression::BinaryExpression;
};

class DifExp final : public BinaryExpression {
  ABSL_MUST_USE_RESULT bool Visit(VisitNodes*) const override;

 public:
  using BinaryExpression::BinaryExpression;
};

class NegativeExp final : public ExpressionNode {
  ABSL_MUST_USE_RESULT bool Visit(VisitNodes*) const override;

 public:
  NegativeExp(Loc loc, std::unique_ptr<ExpressionNode> e);
  ExpressionNode* value() const { return e_.get(); }

 private:
  std::unique_ptr<ExpressionNode> e_;
};

class Define final : public ExpressionNode {
  ABSL_MUST_USE_RESULT bool Visit(VisitNodes*) const override;

 public:
  Define(Loc loc, std::unique_ptr<std::string> name,
         std::unique_ptr<LiteralValue> val, std::unique_ptr<UnitExp> unit);

  const std::string& name() const { return name_; }
  double value() const { return val_->value(); }
  const UnitExp& unit() const { return *unit_; }

  Define(Loc loc, std::string s, double v);  // for testing

 private:
  std::string name_;
  std::unique_ptr<LiteralValue> val_;
  std::unique_ptr<UnitExp> unit_;
};

class Specification final : public NodeI {
  ABSL_MUST_USE_RESULT bool Visit(VisitNodes*) const override;

 public:
  Specification(Loc loc, std::unique_ptr<std::string> name,
                std::unique_ptr<UnitExp> unit);

  const std::string& name() const { return name_; }
  const UnitExp& unit() const { return *unit_; }

 private:
  std::string name_;
  std::unique_ptr<UnitExp> unit_;
};

class Document final : public NodeI {
  ABSL_MUST_USE_RESULT bool Visit(VisitNodes*) const override;

 public:
  Document() : NodeI(Loc{}) {}

  void AddEquality(std::unique_ptr<Equality> e);
  void AddDefinition(std::unique_ptr<Define> d);
  void AddSpecification(std::unique_ptr<Specification> s);
  void AddUnitDefinition(std::unique_ptr<UnitDef> u);

  std::vector<Equality*> equality() const;
  std::vector<Define*> defines() const;
  std::vector<Specification*> specs() const;
  std::vector<UnitDef*> unit_definition() const;

 private:
  std::vector<std::unique_ptr<Equality>> equality_;
  std::vector<std::unique_ptr<Define>> defines_;
  std::vector<std::unique_ptr<Specification>> specs_;
  std::vector<std::unique_ptr<UnitDef>> unit_def_;
};

class VisitNodes {
 public:
  // Get the address of the object as a pointer, even if it's a temporary.
  VisitNodes* as_ptr() { return this; }

  ABSL_MUST_USE_RESULT virtual bool operator()(const UnitExp&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const UnitDef&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const Equality&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const LiteralValue&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const NamedValue&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const PowerExp&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const ProductExp&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const QuotientExp&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const SumExp&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const DifExp&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const NegativeExp&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const Define&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const Specification&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const Document&) = 0;

  // Only exact matches are allowed. Suppress all conversions.
  template <class T>
  bool operator()(const T&) = delete;
};

// A Visitor that can emmit errors.
class VisitNodesWithErrors : public VisitNodes {
 public:
  using ErrorSink = std::function<void(const std::string&)>;
  VisitNodesWithErrors(ErrorSink e) : sink_(e) {}

  static void DefaultSink(const std::string& e) {
    std::cerr << e << std::flush;
  }

 private:
  friend class ErrorMessage;
  ErrorSink sink_;
};

class VisitNodesWithErrors;

/////////////////////////////
// A helper for creating error messages.
class ErrorMessage {
 public:
  ErrorMessage(const char* file, int line, VisitNodesWithErrors* v, const NodeI& n)
      : ErrorMessage(file, line, v, n.location()) {}
  ErrorMessage(const char* file, int line, VisitNodesWithErrors* v, const Loc& l)
     : visitor_(v) {
#ifndef NDEBUG
    get() << "(" << file << ":" << line << ") ";
#endif  // NDEBUG
    get() << l << ": ";
  }
  ~ErrorMessage() {
    str_ << "\n";
    visitor_->sink_(str_.str());
  }

  std::ostream& get() { return str_; }

 private:
  std::stringstream str_;
  VisitNodesWithErrors* visitor_;
};

#define SYM_ERROR(x) ErrorMessage(__FILE__, __LINE__, this, (x)).get()

}  // namespace tbd

#endif  // TBD_AST_H_
