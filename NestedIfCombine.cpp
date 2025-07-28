// NestedIfCombine.cpp
#include "NestedIfCombine.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PatternMatch.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"


using namespace llvm;
using namespace PatternMatch;

#define DEBUG_TYPE "nested-if-combine"

// Statistic for tracking how many nested ifs were combined
STATISTIC(NumCombined, "Number of nested ifs combined");

// Try to combine nested if-statements into a single conditional
static bool combineNestedIfs(BranchInst *OuterBI) {
  // Outer branch must be conditional
  if (!OuterBI || !OuterBI->isConditional())
    return false;

  // Get the 'then' and 'else' successors of the outer branch
  BasicBlock *OuterThen = OuterBI->getSuccessor(0);
  BasicBlock *OuterElse = OuterBI->getSuccessor(1);

  // Outer 'then' block must contain only a single instruction: a conditional branch
  if (OuterThen->size() != 1)
    return false;

  // Get the inner branch instruction
  auto *InnerBI = dyn_cast<BranchInst>(OuterThen->getTerminator());
  if (!InnerBI || !InnerBI->isConditional())
    return false;

  // Get the 'then' and 'else' successors of the inner branch
  BasicBlock *InnerThen = InnerBI->getSuccessor(0);
  BasicBlock *InnerElse = InnerBI->getSuccessor(1);

  // For transformation to be safe, the inner 'else' must be the same as the outer 'else'
  if (InnerElse != OuterElse)
    return false;

  // Debug output showing the match
  LLVM_DEBUG(dbgs() << "Found nested ifs in " << OuterBI->getFunction()->getName()
                    << "\n  Outer: " << *OuterBI
                    << "\n  Inner: " << *InnerBI << "\n");

  // Combine the two conditions using logical AND (&&)
  IRBuilder<> Builder(OuterBI);
  Value *CombinedCond = Builder.CreateAnd(
      OuterBI->getCondition(),
      InnerBI->getCondition(),
      "nested.if.combined");

  // Replace the outer branch with a new one that uses the combined condition
  Builder.SetInsertPoint(OuterBI);
  Builder.CreateCondBr(CombinedCond, InnerThen, OuterElse);

  // Remove the old branch instructions
  OuterBI->eraseFromParent();
  InnerBI->eraseFromParent();

  // Delete the now empty 'then' block from the outer branch
  OuterThen->eraseFromParent();

  // Update statistics
  ++NumCombined;
  return true;
}

// Entry point for the pass: processes each function
PreservedAnalyses NestedIfCombinePass::run(Function &F,
                                           FunctionAnalysisManager &AM) {
  bool Changed = false;
  SmallVector<BranchInst *, 8> Worklist;

  // Collect all conditional branches in the function
  for (BasicBlock &BB : F)
    if (auto *BI = dyn_cast<BranchInst>(BB.getTerminator()))
      if (BI->isConditional())
        Worklist.push_back(BI);

  // Try to combine nested ifs for each conditional branch
  for (BranchInst *BI : Worklist)
    Changed |= combineNestedIfs(BI);

  // Return the appropriate PreservedAnalyses depending on whether we modified anything
  return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "NestedIfCombine", LLVM_VERSION_STRING,
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
           llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
          if (Name == "nested-if-combine") {
            FPM.addPass(NestedIfCombinePass());
            return true;
          }
          return false;
        });
    }
  };
}

