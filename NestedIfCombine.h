// NestedIfCombine.h
#ifndef LLVM_TRANSFORMS_SCALAR_NESTEDIFCOMBINE_H
#define LLVM_TRANSFORMS_SCALAR_NESTEDIFCOMBINE_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class NestedIfCombinePass : public PassInfoMixin<NestedIfCombinePass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

} // namespace llvm

#endif
