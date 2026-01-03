
#ifndef HOBBES_EVAL_TYPE_HPP_INCLUDED
#define HOBBES_EVAL_TYPE_HPP_INCLUDED

#include <hobbes/lang/type.H>  // MonoTypePtr, MonoTypes
#include <hobbes/util/llvm.H>
#include <vector>

namespace hobbes {

llvm::Type* toLLVM(const MonoTypePtr&, bool asArg = false);
Types toLLVM(const MonoTypes&, bool asArg = false);

}

#endif
