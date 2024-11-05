.text
.syntax unified
.thumb
.cpu cortex-m0plus
.type invertir, %function
.global invertir
.thumb_func

invertir:
        movs r1, r0
	movs r2, #32
	movs r3, #0
        loop:
                lsls r0, r0, #1
		ands r3, r1, #31
		adds r2, r2, r3
		lsrs r1, r1, #1
                subs r2, r2, #1
		cmp r2, #0
                bne loop
                BX LR
        .end
