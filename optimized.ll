; ModuleID = 'llvm-build/test.ll'
source_filename = "llvm-build/test.ll"

define void @test(i1 %a, i1 %b) {
entry:
  %nested.if.combined = and i1 %a, %b
  br i1 %nested.if.combined, label %inner.then, label %exit

inner.then:                                       ; preds = %entry
  ret void

exit:                                             ; preds = %entry
  ret void
}
