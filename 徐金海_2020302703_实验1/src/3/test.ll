; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

@g2 = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  store i32 10, ptr %2, align 4
  store i32 1, ptr %7, align 4
  store i32 1, ptr %8, align 4
  store i32 1, ptr %8, align 4
  br label %9

9:                                                ; preds = %20, %0
  %10 = load i32, ptr %8, align 4
  %11 = icmp sgt i32 %10, 5
  br i1 %11, label %12, label %15

12:                                               ; preds = %9
  %13 = load i32, ptr %8, align 4
  %14 = icmp slt i32 %13, 10
  br label %15

15:                                               ; preds = %12, %9
  %16 = phi i1 [ false, %9 ], [ %14, %12 ]
  br i1 %16, label %17, label %23

17:                                               ; preds = %15
  %18 = load i32, ptr %8, align 4
  %19 = call i32 @fibonacci(i32 noundef %18)
  store i32 %19, ptr %6, align 4
  br label %20

20:                                               ; preds = %17
  %21 = load i32, ptr %8, align 4
  %22 = add nsw i32 %21, 1
  store i32 %22, ptr %8, align 4
  br label %9, !llvm.loop !4

23:                                               ; preds = %15
  br label %24

24:                                               ; preds = %23, %34
  %25 = load i32, ptr %7, align 4
  %26 = load i32, ptr %2, align 4
  %27 = call i32 @add(i32 noundef %26, i32 noundef 10)
  %28 = mul nsw i32 %25, %27
  store i32 %28, ptr %5, align 4
  %29 = load i32, ptr %7, align 4
  %30 = add nsw i32 %29, 1
  store i32 %30, ptr %7, align 4
  %31 = load i32, ptr %5, align 4
  %32 = icmp sgt i32 %31, 100
  br i1 %32, label %33, label %34

33:                                               ; preds = %24
  br label %35

34:                                               ; preds = %24
  br label %24

35:                                               ; preds = %33
  %36 = load i32, ptr %2, align 4
  %37 = call i32 @fibonacci(i32 noundef %36)
  store i32 %37, ptr %4, align 4
  %38 = load i32, ptr %2, align 4
  %39 = call i32 @add(i32 noundef %38, i32 noundef 10)
  store i32 %39, ptr %3, align 4
  %40 = load i32, ptr %3, align 4
  %41 = load i32, ptr @g2, align 4
  %42 = add nsw i32 %40, %41
  store i32 %42, ptr %3, align 4
  %43 = load i32, ptr %3, align 4
  ret i32 %43
}

declare dso_local i32 @fibonacci(i32 noundef) #1

declare dso_local i32 @add(i32 noundef, i32 noundef) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="none" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 2}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 2}
!3 = !{!"clang version 15.0.7"}
!4 = distinct !{!4, !5}
!5 = !{!"llvm.loop.mustprogress"}
