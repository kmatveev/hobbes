
#ifndef HOBBES_UTIL_LLVM_HPP_INCLUDED
#define HOBBES_UTIL_LLVM_HPP_INCLUDED

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wctor-dtor-privacy"

#include <llvm/Config/llvm-config.h>

#include <hobbes/lang/type.H>
#include <hobbes/util/ptr.H>
#include <hobbes/util/array.H>

#include <hobbes/lang/type.H>

#include <llvm/Analysis/Passes.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Signals.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils/Cloning.h>
/* #include <llvm/Transforms/Vectorize.h> */
#include <llvm/Bitstream/BitstreamReader.h>
#include <llvm/Bitstream/BitstreamWriter.h>

#include <llvm/Support/raw_os_ostream.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JITEventListener.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>

#include <llvm/IR/Verifier.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Object/ELFObjectFile.h>

#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>

#pragma GCC diagnostic pop

namespace hobbes {

using int128_t = __int128;

#define INT128_TO_UINT64_ARRAY_REF_NAMED_a_a_a(x)                              \
  const uint64_t aa_aa_aa[] = {                                                \
      static_cast<uint64_t>(static_cast<unsigned __int128>(x) &                \
                            0xFFFFFFFFFFFFFFFF),                               \
      static_cast<uint64_t>(static_cast<unsigned __int128>(x) >> 64U)};        \
  const auto a_a_a = llvm::ArrayRef<uint64_t> { aa_aa_aa }

inline llvm::orc::ThreadSafeContext& threadSafeContext() {
  static auto c = llvm::orc::ThreadSafeContext(std::make_unique<llvm::LLVMContext>());
  return c;
}

template <typename Fn> auto withContext(Fn fn) -> decltype(auto) {
  auto lk = threadSafeContext().getLock();
  return fn(*threadSafeContext().getContext());
}

using Types = std::vector<llvm::Type *>;
using Constants = std::vector<llvm::Constant *>;
using Values = std::vector<llvm::Value *>;

// type utilities
inline llvm::Type* voidType() {
  return withContext([](llvm::LLVMContext& c) { return llvm::Type::getVoidTy(c); });
}

inline llvm::Type* boolType() {
  return withContext([](llvm::LLVMContext& c) { return llvm::Type::getInt1Ty(c); });
}

inline llvm::Type* charType() {
  return withContext([](llvm::LLVMContext& c) { return llvm::Type::getInt8Ty(c); });
}

inline llvm::Type* byteType() {
  return charType();
}

inline llvm::Type* shortType() {
  return withContext([](llvm::LLVMContext& c) { return llvm::Type::getInt16Ty(c); });
}

inline llvm::Type* intType() {
  return withContext([](llvm::LLVMContext& c) { return llvm::Type::getInt32Ty(c); });
}

inline llvm::Type* longType() {
  return withContext([](llvm::LLVMContext& c) { return llvm::Type::getInt64Ty(c); });
}

inline llvm::Type* int128Type() {
  return withContext([](llvm::LLVMContext& c) { return llvm::Type::getIntNTy(c, 128); });
}

inline llvm::Type* floatType() {
  return withContext([](llvm::LLVMContext& c) { return llvm::Type::getFloatTy(c); });
}

inline llvm::Type* doubleType() {
  return withContext([](llvm::LLVMContext& c) { return llvm::Type::getDoubleTy(c); });
}

inline llvm::PointerType* ptrType() {
  return withContext([](llvm::LLVMContext& c) { return llvm::PointerType::getUnqual(c); });
}

inline llvm::ArrayType* arrayType(llvm::Type* ty, size_t sz) {
  // we can be a little dishonest in describing the type of arrays of unit, since LLVM barfs on arrays of unit
  // and we should never need to look at the "data" in an array of unit values _anyway_!
  return llvm::ArrayType::get(ty->isVoidTy() ? boolType() : ty, static_cast<uint64_t>(sz));
}

inline llvm::StructType* packedRecordType(const Types& tys) {
  return withContext([&tys](llvm::LLVMContext& c) { return llvm::StructType::get(c, tys, true); });
}

inline llvm::StructType* recordType(const Types& tys) {
  return withContext([&tys](llvm::LLVMContext& c) { return llvm::StructType::get(c, tys); });
}

inline llvm::StructType* recordType(llvm::Type* t0) {
  return recordType(list(t0));
}

inline llvm::StructType* recordType(llvm::Type* t0, llvm::Type* t1) {
  return recordType(list(t0, t1));
}

inline llvm::StructType* recordType(llvm::Type* t0, llvm::Type* t1, llvm::Type* t2) {
  return recordType(list(t0, t1, t2));
}

inline llvm::StructType* recordType(llvm::Type* t0, llvm::Type* t1, llvm::Type* t2, llvm::Type* t3) {
  return recordType(list(t0, t1, t2, t3));
}

inline llvm::StructType* recordType(llvm::Type* t0, llvm::Type* t1, llvm::Type* t2, llvm::Type* t3, llvm::Type* t4) {
  return recordType(list(t0, t1, t2, t3, t4));
}

inline llvm::StructType* recordType(llvm::Type* t0, llvm::Type* t1, llvm::Type* t2, llvm::Type* t3, llvm::Type* t4, llvm::Type* t5) {
  return recordType(list(t0, t1, t2, t3, t4, t5));
}

inline llvm::StructType* recordType(llvm::Type* t0, llvm::Type* t1, llvm::Type* t2, llvm::Type* t3, llvm::Type* t4, llvm::Type* t5, llvm::Type* t6) {
  return recordType(list(t0, t1, t2, t3, t4, t5, t6));
}

inline llvm::FunctionType* functionType(const Types& argTys, llvm::Type* rty) {
  return llvm::FunctionType::get(rty, argTys, false);
}

inline llvm::StructType* varArrayType(llvm::Type* elemty, size_t sz = 1) {
  return recordType(longType(), arrayType(elemty, sz));
}

// casting
inline llvm::Value* cast(llvm::IRBuilder<>* b, llvm::Type* ty, llvm::Value* v) {
  return b->CreateBitCast(v, ty);
}

inline llvm::Constant* ccast(llvm::Type* ty, llvm::Constant* v) {
  return llvm::ConstantExpr::getBitCast(v, ty);
}

// constant utilities
inline llvm::Constant* cvalue(bool x) {
  return llvm::Constant::getIntegerValue(boolType(), llvm::APInt(1, x ? 1 : 0, false));
}

inline llvm::Constant* cvalue(char x) {
  return llvm::Constant::getIntegerValue(charType(), llvm::APInt(8, x, true));
}

inline llvm::Constant* cvalue(unsigned char x) {
  return llvm::Constant::getIntegerValue(charType(), llvm::APInt(8, x, false));
}

inline llvm::Constant* cvalue(short x) {
  return llvm::Constant::getIntegerValue(shortType(), llvm::APInt(16, x, false));
}

inline llvm::Constant* cvalue(int x) {
  return llvm::Constant::getIntegerValue(intType(), llvm::APInt(32, x, true));
}

inline llvm::Constant* cvalue(unsigned int x) {
  return llvm::Constant::getIntegerValue(intType(), llvm::APInt(32, x, true));
}

inline llvm::Constant* cvalue(int64_t x) {
  return llvm::Constant::getIntegerValue(longType(), llvm::APInt(64, x, true));
}

inline llvm::Constant* cvalue(int128_t x) {
  INT128_TO_UINT64_ARRAY_REF_NAMED_a_a_a(x);
  return llvm::Constant::getIntegerValue(int128Type(), llvm::APInt(128, a_a_a));
}

inline llvm::Constant* cvalue(float x) {
  return withContext(
      [x](llvm::LLVMContext& c) { return llvm::ConstantFP::get(c, llvm::APFloat(x)); });
}

inline llvm::Constant* cvalue(double x) {
  return withContext(
      [x](llvm::LLVMContext& c) { return llvm::ConstantFP::get(c, llvm::APFloat(x)); });
}

// constant int utilities (different than constants to LLVM!)
inline llvm::ConstantInt* civalue(bool x) {
  return withContext([x](llvm::LLVMContext& c) {
    return llvm::ConstantInt::get(llvm::IntegerType::get(c, 1), static_cast<uint64_t>(x ? 1 : 0));
  });
}

inline llvm::ConstantInt* civalue(char x) {
  return withContext([x](llvm::LLVMContext& c) {
    return llvm::ConstantInt::get(llvm::IntegerType::get(c, 8), static_cast<uint64_t>(x));
  });
}

inline llvm::ConstantInt* civalue(unsigned char x) {
  return withContext([x](llvm::LLVMContext& c) {
    return llvm::ConstantInt::get(llvm::IntegerType::get(c, 8), static_cast<uint64_t>(x));
  });
}

inline llvm::ConstantInt* civalue(short x) {
  return withContext([x](llvm::LLVMContext& c) {
    return llvm::ConstantInt::get(llvm::IntegerType::get(c, 16), static_cast<uint64_t>(x));
  });
}

inline llvm::ConstantInt* civalue(int x) {
  return withContext([x](llvm::LLVMContext& c) {
    return llvm::ConstantInt::get(llvm::IntegerType::get(c, 32), static_cast<uint64_t>(x));
  });
}

inline llvm::ConstantInt* civalue(unsigned int x) {
  return withContext([x](llvm::LLVMContext& c) {
    return llvm::ConstantInt::get(llvm::IntegerType::get(c, 32), static_cast<uint64_t>(x));
  });
}

inline llvm::ConstantInt* civalue(long long x) {
  return withContext([x](llvm::LLVMContext& c) {
    return llvm::ConstantInt::get(llvm::IntegerType::get(c, 64), static_cast<uint64_t>(x));
  });
}

inline llvm::ConstantInt* civalue(int128_t x) {
  return withContext([x](llvm::LLVMContext& c) {
    INT128_TO_UINT64_ARRAY_REF_NAMED_a_a_a(x);
    return llvm::ConstantInt::get(c, llvm::APInt(128, a_a_a));
  });
}

#undef INT128_TO_UINT64_ARRAY_REF_NAMED_a_a_a

// an intermediate representation for records
using FieldValue = std::pair<std::string, llvm::Value *>;
using RecordValue = std::vector<FieldValue>;

using ConstFieldValue = std::pair<std::string, llvm::Constant *>;
using ConstRecordValue = std::vector<ConstFieldValue>;

// add any standard settings to global variables that we need
inline llvm::GlobalVariable* prepgv(llvm::GlobalVariable* gv, unsigned int align = sizeof(void*)) {
#if LLVM_VERSION_MAJOR >= 10
  gv->setAlignment(llvm::MaybeAlign(align));
#else  
  gv->setAlignment(align);
#endif
  return gv;
}

// constant :: Value -> Bool -> Const
//   try to extract a constant from a value (either by value or reference)
inline llvm::Constant* constant(llvm::Value* v, bool globalPtrRefs) {
  if (auto* gv = llvm::dyn_cast<llvm::GlobalVariable>(v)) {
    if (gv->isConstant()) {
      if (globalPtrRefs) {
        return gv;
      } else {
        return gv->hasInitializer() ? gv->getInitializer() : nullptr;
      }
    } else {
      return nullptr;
    }
  } else if (auto* c = llvm::dyn_cast<llvm::Constant>(v)) {
    return c;
  } else {
    return nullptr;
  }
}

// liftAsGlobalRef :: Constant -> Constant
//   convert a constant to a global reference to the constant
inline llvm::Constant* liftAsGlobalRef(llvm::Module* m, llvm::Constant* c) {
  return
    new llvm::GlobalVariable(
      *m,
      c->getType(),
      true,
      llvm::GlobalVariable::InternalLinkage,
      c
    );
}

// varArrayConstants :: [Value] -> Bool -> [Const]
//   try to extract constants from a set of compiled values -- if even one value is not constant, fail by returning an empty list
inline Constants varArrayConstants(const Values& vs, bool globalPtrRefs) {
  Constants r;
  for (auto *v : vs) {
    if (llvm::Constant* c = constant(v, globalPtrRefs)) {
      r.push_back(c);
    } else {
      return Constants();
    }
  }
  return r;
}

inline Constants liftAsGlobalRefs(llvm::Module* m, const Constants& cs) {
  Constants r;
  for (auto *c : cs) {
    r.push_back(liftAsGlobalRef(m, c));
  }
  return r;
}

inline llvm::Constant* constArray(llvm::Module* m, const Constants& cs, llvm::Type* elemTy, bool boxAsGlobalRefs = false) {
  llvm::ArrayType*  aty  = arrayType(elemTy, cs.size());
  llvm::StructType* saty = varArrayType(elemTy, cs.size());

  long long arrayLen = cs.size();
  Constants ncs      = elemTy->isVoidTy() ? Constants() : cs;

  if (boxAsGlobalRefs) {
    ncs = liftAsGlobalRefs(m, ncs);
  }

  return llvm::ConstantStruct::get(saty, list<llvm::Constant*>(cvalue(arrayLen), llvm::ConstantArray::get(aty, ncs)));
}

inline llvm::Value* tryMkConstVarArray(llvm::IRBuilder<>* b, llvm::Module* m, llvm::Type* elemTy, const Values& vs, bool globalPtrRefs) {
  Constants cs = varArrayConstants(vs, globalPtrRefs);
  if (cs.size() != vs.size()) {
    return nullptr;
  } else {
    llvm::StructType* saty = varArrayType(elemTy, cs.size());

    return
      cast(b, ptrType(),
        new llvm::GlobalVariable(
          *m,
          saty,
          true,
          llvm::GlobalVariable::InternalLinkage,
          constArray(m, cs, elemTy)
        )
      );
  }
}

inline llvm::Constant* padding(size_t len) {
  Constants pad;
  for (size_t i = 0; i < len; ++i) {
    pad.push_back(cvalue(scast<uint8_t>(0)));
  }
  return llvm::ConstantArray::get(arrayType(byteType(), len), pad);
}

inline llvm::Value* memCopy(llvm::IRBuilder<>* b, llvm::Value* dst, uint32_t dstAlign, llvm::Value* src, uint32_t srcAlign, llvm::Value* sz) {
  return b->CreateMemCpy(dst, llvm::MaybeAlign(dstAlign), src, llvm::MaybeAlign(srcAlign), sz);
}

inline llvm::Value* memCopy(llvm::IRBuilder<>* b, llvm::Value* dst, uint32_t dstAlign, llvm::Value* src, uint32_t srcAlign, unsigned int sz) {
  return memCopy(b, dst, dstAlign, src, srcAlign, civalue(sz));
}

inline Constants mergePadding(const Constants& cs, const Record::Members& ms) {
  Constants r;
  size_t i = 0;
  auto m = ms.begin();
  while (m != ms.end()) {
    if (m->field[0] == '.' && m->field[1] == 'p') {
      size_t plen = 0;
      if (auto* fa = is<FixedArray>(m->type)) {
        int64_t asz = fa->requireLength();
        plen = asz < 0 ? 0 : static_cast<size_t>(asz);
      } else {
        throw std::runtime_error("Cannot derive padding for pad field with non-padding type (internal error)");
      }
      r.push_back(padding(plen));
    } else {
      r.push_back(cs[i]);
      ++i;
    }
    ++m;
  }
  return r;
}

inline Types types(const Constants& cs) {
  Types r;
  for (auto *c : cs) {
    r.push_back(c->getType());
  }
  return r;
}

using UnzRecValues = std::pair<std::vector<std::string>, Values>;

inline Constants recordUZConstants(const UnzRecValues& rps, const Record* rty) {
  Constants r;
  for (size_t i = 0; i < rps.second.size(); ++i) {
    if (llvm::Constant* c = constant(rps.second[i], is<Array>(rty->member(rps.first[i])) != nullptr)) { // not pretty ...
      r.push_back(c);
    } else {
      return Constants();
    }
  }
  return r;
}

llvm::Type* toLLVM(const MonoTypePtr&, bool);

inline Constants liftArraysAsGlobals(llvm::Module* m, const Constants& cs, const MonoTypes& tys) {
  Constants r;
  for (size_t i = 0; i < std::min<size_t>(cs.size(), tys.size()); ++i) {
    if (is<Array>(tys[i]) != nullptr) {
      r.push_back(llvm::ConstantExpr::getBitCast(liftAsGlobalRef(m, cs[i]), toLLVM(tys[i], true)));
    } else {
      r.push_back(cs[i]);
    }
  }
  return r;
}

inline llvm::Constant* constantRecord(llvm::Module*, const Constants& cs, const Record* rty) {
  return llvm::ConstantStruct::getAnon(mergePadding(cs, rty->alignedMembers()), true);
}

inline llvm::Value* tryMkConstRecord(llvm::IRBuilder<>*, llvm::Module* m, const RecordValue& rv, const Record* rty) {
  Constants crv = recordUZConstants(unzip(rv), rty);
  if (crv.size() != rv.size()) {
    return nullptr;
  } else {
    Constants         pcs = mergePadding(crv, rty->alignedMembers());
    llvm::StructType* sty = packedRecordType(types(pcs));

    return prepgv(
      new llvm::GlobalVariable(
        *m,
        sty,
        true,
        llvm::GlobalVariable::InternalLinkage,
        llvm::ConstantStruct::getAnon(pcs, true)
      ),
      1
    );
  }
}

// pointer / GEP utilities
inline llvm::Value* offset(llvm::IRBuilder<>* b, llvm::Type* t, llvm::Value* p, llvm::Value* o0) {
  std::vector<llvm::Value*> idxs;
  idxs.push_back(o0);
  return b->CreateGEP(t, p, idxs);
}

inline llvm::Value* offset(llvm::IRBuilder<>* b, llvm::Type* t, llvm::Value* p, int o0) {
  return offset(b, t, p, cvalue(o0));
}

inline llvm::Value* offset(llvm::IRBuilder<>* b, llvm::Type* t, llvm::Value* p, int o0, int o1) {
  std::vector<llvm::Value*> idxs;
  idxs.push_back(cvalue(o0));
  idxs.push_back(cvalue(o1));
  return b->CreateGEP(t, p, idxs);
}

inline llvm::Value* offset(llvm::IRBuilder<>* b, llvm::Type* t, llvm::Value* p, int o0, llvm::Value* o1) {
  std::vector<llvm::Value*> idxs;
  idxs.push_back(cvalue(o0));
  idxs.push_back(o1);
  return b->CreateGEP(t, p, idxs);
}

inline llvm::Value* structOffset(llvm::IRBuilder<>* b, llvm::Type* t, llvm::Value* p, unsigned int fieldOffset) {
  return b->CreateStructGEP(t, p, fieldOffset);
}

inline llvm::ExecutionEngine* makeExecutionEngine(llvm::Module* m, llvm::SectionMemoryManager* smm) {
  std::string err;
  llvm::ExecutionEngine* ee =
    llvm::EngineBuilder(std::unique_ptr<llvm::Module>(m))
      .setErrorStr(&err)
      .setMCJITMemoryManager(std::unique_ptr<llvm::SectionMemoryManager>(smm))
      .create();

  if (ee == nullptr) {
    throw std::runtime_error("Internal error, failed to allocate execution engine with error: " + err);
  }
  return ee;
}


inline llvm::Value* fncall(llvm::IRBuilder<>* b, llvm::Value* vfn, llvm::Type* tfn, const Values& args) {
    (void)tfn;
  if (auto *ty = llvm::dyn_cast<llvm::FunctionType>(tfn)) {
    auto *inst =  b->CreateCall(ty, vfn, args);
    if (ty->getReturnType() == boolType()) {
      inst->addRetAttr(llvm::Attribute::ZExt);
    }
    return inst;
  } else {
    throw std::runtime_error("fncall(...) invoked on non-function type (internal error)");
  }
  return nullptr;

}

inline llvm::Value* fncall(llvm::IRBuilder<>* b, llvm::Value* fn, llvm::Type* tfn, llvm::Value* arg) {
  return fncall(b, fn, tfn, list(arg));
}

inline void maybeInlineFunctionsIn(llvm::Module& m) {
  // If there is an IR function in current module is already large (instructions
  // > `HOBBES_FUNCTION_INLINE_THRESHOLD`), then keep inlining it makes it even
  // larger, 10x larger. We simply stop applying FunctionInlining pass on this
  // module in such kinds of scenario.
  static const auto allFunctionsAreSmall = [](const llvm::Module& m) {
    static unsigned fnInliningThreshold = [] {
      const char* v = std::getenv("HOBBES_FUNCTION_INLINE_THRESHOLD");
      if (v == nullptr) {
        return 30'000U; // arbitrarily chosen, but should be conservative enough
      }
      return static_cast<unsigned>(strtoul(v, nullptr, 10));
    }();

    // if any functions in this module has instruction number above threshold,
    // then should not inline anything
    return std::none_of(m.begin(), m.end(), [](const llvm::Function& f) {
      return f.getInstructionCount() > fnInliningThreshold;
    });
  };

  if (allFunctionsAreSmall(m)) {
    auto mpm = llvm::legacy::PassManager();
    // mpm.add(llvm::createFunctionInliningPass());
    mpm.run(m);
  }
}
}
#endif
