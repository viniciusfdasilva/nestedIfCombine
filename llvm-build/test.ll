define void @test(i1 %a, i1 %b) {
entry:
  br i1 %a, label %outer.then, label %exit

outer.then:
  br i1 %b, label %inner.then, label %exit

inner.then:
  ret void

exit:
  ret void
}

