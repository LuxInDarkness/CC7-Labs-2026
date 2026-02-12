# Lab 3 Interrupt Handler Documentation
## C Code
### timer_init function
-`PUT32(CM_PER_TIMER2_CLKCTRL, 0x2);` This enables the timer clock, the value 0x2 is the value indicated by the documentation to enable the timer clock.

-`PUT32(INTC_MIR_CLEAR2, 1 << 4);` Unmasks (sets the bit to 1) IRQ 68, this is because this bit is dedicated to the DMTIMER2 event, additionally, there are 128 unique possible interrupts, all divided between 4 banks, INTC_MIR_CLEAR0, INTC_MIR_CLEAR1, INTC_MIR_CLEAR2, and INTC_MIR_CLEAR3, as we required the bit 68, that would be the bit 4 (5th positionally) of the third bank INTC_MIR_CLEAR2.

-`PUT32(INTC_ILR68, 0x0);` Configures interrupt priority, priorities go from 0x0 to 0x7, 0x0 being the highest and 0x7 the lowest.

-`PUT32(TCLR, 0x0);` Stops the timer.

-`PUT32(TISR, 0x7);` Clears pending interrupts.

-`PUT32(TLDR, 0xFE91CA00);` Sets the load value to 2 seconds to expire, this works setting the value to 0xFFFFFFFF minus the equivalent ticks for 2 seconds, due to the clock only going up and resetting after trigerring the interrupt.

-`PUT32(TCRR, 0xFE91CA00);` The previous command set the "reset value", this one sets the current value, same explanation as before.

-`PUT32(TIER, 0x2);` Enables the interrupt that ocurrs when the clock overflows.

-`PUT32(TCLR, 0x3);` Starts the timer in auto reload, meaning it will go back to 0xFE91CA00 after overflowing and start counting again.

### timer_irq_handler function
-`PUT32(TISR, 0x2);` Clears the pending timer interrupt.

-`PUT32(INTC_CONTROL, 0x1);` Let's the controller know the interrupt is acknowledged and being worked on.

-`os_write("Tick\n");` Prints "Tick".

## Assembler Code
### irq_handler
-`sub   lr, lr, #4` Equivalent to lr-=4.

-`stmfd sp!, {r0-r12, lr}` Store multiple full descending, saves all relevant registers to the stack.

-`bl timer_irq_handler` Branch link to timer_irq_handler (which is in our C Code).

-`ldmfd sp!, {r0-r12, lr}` Load multiple full descending, restores all relevant registers from the stack.

-`subs  pc, lr, #4` Equivalent to pc=lr-4, but also updates the CPSR (Current Program Status Register) flags based on the result.

### enable_irq
-`mrs r0, cpsr` Move register from status, copies the values of a program status register into a normal register.

-`bic r0, r0, #0x80` Bit clear, sets the eight bit of CPSR to 0, enabling the irq mask bit (0 means exception not masked, 1 would be exception masked). 

-`msr cpsr_c, r0` Move status from register, copies the values of a normal register into a program status register.

-`bx lr` Branch exchange, returns to the return address.