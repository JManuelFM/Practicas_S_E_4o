.text
.syntax unified
.thumb
.cpu cortex-m0plus
.type division, %function
.global division
.thumb_func

division:
	movs r2, #0
	loop:
		cmp r0, r1
		blt end
		adds r2, r2, 1
		subs r0, r0, r1
		b loop
	end:
		movs r0, r2
		bx lr
	.end
