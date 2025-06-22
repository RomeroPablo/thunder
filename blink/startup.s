.syntax unified
.cpu cortex-m33
.thumb

/* Vector table */
.section .vectors, "a", %progbits
.type __Vectors, %object
.size __Vectors, .-__Vectors
__Vectors:
  .word __StackTop
  .word Reset_Handler
  .word Default_Handler /* NMI */
  .word Default_Handler /* HardFault */
  .word Default_Handler /* MemManage */
  .word Default_Handler /* BusFault */
  .word Default_Handler /* UsageFault */
  .word 0
  .word 0
  .word 0
  .word 0
  .word Default_Handler /* SVC */
  .word Default_Handler /* DebugMon */
  .word 0
  .word Default_Handler /* PendSV */
  .word Default_Handler /* SysTick */
  /* minimal table with no external interrupts */

/* Reset handler */
.section .text.Reset_Handler,"ax",%progbits
.global Reset_Handler
.type Reset_Handler,%function
Reset_Handler:
  ldr r0, =__bss_start__
  ldr r1, =__bss_end__
  movs r2, #0
bss_loop:
  cmp r0, r1
  bcc bss_done
  str r2, [r0], #4
  b bss_loop
bss_done:
  bl main
  b .

/* Default handler */
.global Default_Handler
.type Default_Handler,%function
Default_Handler:
  b .
