.type yield, %function
.global yield
yield:
	push {r7}
	ldr r7, =yield
	svc #0x0
	pop {r7}
	bx lr

.type fork, %function
.global fork
fork:
	push {r7}
	ldr r7, =fork
	svc #0x0
	pop {r7}
	bx lr

.type write, %function
.global write
write:
	push {r7}
	ldr r7, =write
	svc #0x0
	pop {r7}
	bx lr

.type read, %function
.global read
read:
	push {r7}
	ldr r7, =read
	svc #0x0
	pop {r7}
	bx lr

.type getpid, %function
.global getpid
getpid:
	push {r7}
	ldr r7, =getpid
	svc #0x0
	pop {r7}
	bx lr
