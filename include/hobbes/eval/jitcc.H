
#ifndef HOBBES_EVAL_JITCC_HPP_INCLUDED
#define HOBBES_EVAL_JITCC_HPP_INCLUDED

#include <hobbes/lang/expr.H>  // Exprs, ExprPtr, LetRec::Binding
#include <hobbes/lang/type.H>  // PolyTypePtr, MonoTypePtr, MonoTypes, TEnvPtr
#include <hobbes/lang/tylift.H>  // typedb
#include <hobbes/util/region.H>  // region

#include <hobbes/util/llvm.H>
#include <hobbes/eval/func.H>

#include <llvm/IR/ValueHandle.h>
#include <map>
#include <string>
#include <vector>

namespace llvm {
class GlobalVariable;
}

namespace hobbes {

// an operation, which can emit some specialized assembly code
class jitcc;
struct op {
  virtual ~op();

  // type : reports the functional type of this operator (may be polymorphic)
  virtual PolyTypePtr type(typedb&) const = 0;

  // apply : produces some assembly code out of a JIT compiler,
  //         assuming the given input/output types and with expressions provided for arguments
  virtual llvm::Value* apply(jitcc* ev, const MonoTypes& tys, const MonoTypePtr& rty, const Exprs& es) = 0;
};

class ORCJIT;
class Globals;
class VTEnv;
class ConstantList;

// a JIT compiler for monotyped expressions
class jitcc {
public:
  jitcc(const TEnvPtr&);
  ~jitcc();

  const TEnvPtr&     typeEnv() const;
  llvm::IRBuilder<>* builder() const;
  llvm::Module*      module();

  // get the address of a bound symbol
  void* getSymbolAddress(const std::string&);

  // define a global from a primitive expression
  void defineGlobal(const std::string& vname, const ExprPtr& unsweetExp);

  // define a global on some existing memory
  void bindGlobal(const std::string& vn, const MonoTypePtr& ty, void* x);

  // is there a definition of the named symbol?
  bool isDefined(const std::string&) const;

  // compile a named or anonymous expression (into the current instruction stream)
  // these assume that expressions have explicit (mono-)type annotations
  llvm::Value* compile(const ExprPtr& exp);
  llvm::Value* compile(const std::string& vname, const ExprPtr& exp);

  // backtrack on local-scope to compile an expression that uses only global data
  llvm::Value* compileAtGlobalScope(const ExprPtr& exp);

  // compile a function or a set of mutually-recursive functions
  llvm::Function* compileFunction(const std::string& name, const str::seq& argns, const MonoTypes& argtys, const ExprPtr& exp);
  void            compileFunctions(const LetRec::Bindings&, std::vector<llvm::Function*>* result);
  void            compileFunctions(const LetRec::Bindings&);

  // compile an allocation statement (to dynamically allocate some data)
  llvm::Value* compileAllocStmt(size_t sz, size_t asz, llvm::Type* mty, bool zeroMem = false);
  llvm::Value* compileAllocStmt(llvm::Value* sz, llvm::Value* asz, llvm::Type* mty, bool zeroMem = false);

  // begin a function with the given name, argument type list, return type
  llvm::Function* allocFunction(const std::string& fname, const MonoTypes& argl, const MonoTypePtr& rty);

  // bind within local scopes, and begin/end new nested local scopes
  void pushScope();
  void bindScope(const std::string& vn, llvm::Value* v);
  void popScope();

  // produce some machine code from a function specification (input names, input types, expression body)
  void* reifyMachineCodeForFn(const MonoTypePtr& reqTy, const str::seq& names, const MonoTypes& tys, const ExprPtr& exp);
  void releaseMachineCode(void*);

  // bind a low-level function definition
  void bindInstruction(const std::string&, op*);

  // find a low-level function definition by name
  op* lookupOp(const std::string&) const;

  // lookup a variable, either in local scopes, globals, or constants
  llvm::Value* lookupVar(const std::string&, const MonoTypePtr&);

  // find a function by name (returns nullptr if not found)
  llvm::Function* lookupFunction(const std::string&);

  // maybe get a pointer to global data
  //   this will give a nullptr either if the variable is in local scope, or if there is no global variable with that name
  llvm::GlobalVariable* lookupVarRef(const std::string&);

  // produce a constant reference to an interned string
  llvm::Value* internConstString(const std::string&);

  // get the machine code produced for a given expression
  using bytes = std::vector<uint8_t>;
  bytes machineCodeForExpr(const ExprPtr&);

  // inline all global definitions within an expression
  ExprPtr inlineGlobals(const ExprPtr&);

  // allocate some global data attached to this JIT
  void* memalloc(size_t, size_t);
private:
  TEnvPtr tenv;

  // produce some machine code for a compiled function
  void* getMachineCode(llvm::Function*, llvm::JITEventListener* listener = nullptr);

  std::unique_ptr<llvm::Module> currentModule;

  // support incremental construction of LLVM assembly sequences
  std::unique_ptr<llvm::IRBuilder<>> irbuilder;

  // the bound root function environment
  using FuncEnv = std::map<std::string, op *>;
  FuncEnv fenv;

  // keep track of variables and local scopes during compilation
  std::unique_ptr<VTEnv> vtenv;
  bool ignoreLocalScope;

  // compile sets of mutually-recursive functions (as a special case, single-function compilation)
  struct UCF {
    const std::string& name;
    const str::seq&    argns;
    MonoTypes          argtys;
    const ExprPtr&     exp;

    llvm::Function*    result;

    inline UCF(const std::string& name, const str::seq& argns, const MonoTypes& argtys, const ExprPtr& exp)
      : name(name), argns(argns), argtys(argtys), exp(exp) { }
  };
  using UCFS = std::vector<UCF>;
  void unsafeCompileFunctions(UCFS*);

  // keep track of global variables
  std::unique_ptr<Globals> globals;

  // keep track of global data (in case we need to dynamically allocate global variables of any type)
  region globalData;
  size_t pushGlobalRegion();
  void popGlobalRegion(size_t x);

  // keep track of global constants
  std::unique_ptr<ConstantList> constants;
  llvm::Value* loadConstant(const std::string&);

  // keep some interned strings, helpful for global constants and debug info
  using InternConstVars = std::unordered_map<std::string, std::string>;
  InternConstVars internConstVars;

  llvm::GlobalVariable* lookupGlobalVar(const std::string&);

  // keep track of monotyped definitions as expressions
  // (in case we want to inline them later)
  using GlobalExprs = std::map<std::string, ExprPtr>;
  GlobalExprs globalExprs;

  std::unique_ptr<ORCJIT> orcjit;
};

// shorthand for compilation over a sequence of expressions
using Values = std::vector<llvm::Value *>;

Values compile(jitcc*, const Exprs&);
Values compileArgs(jitcc*, const Exprs&);

}

#endif

