# SkyCPU Core - 8 / 16 / 32 bits virtual CPU
## by SkyWodd

This project is a basic but complete virtual CPU engine.
It is inspired by 8051 architecture and AVR-8 instructions.

**Warning: the core is not fully debugged / functionnal as now.**

---

### SkyCPU specifications

The SkyCPU has :
* 32 (8 bits) general purpose registers
* one 16-bits program counter register
* one 16-bits stack pointer register

For a better emulation efficiency there no status register !
So, to avoid coding problems the instructions set is pretty big.

By default the SkyCPU core use 65536 bytes (16 bits address bus) of RAM for the virtual cpu memory.
This memory contain both CODE AND DATA binaries of the executed program (von neumann typed architecture) allowing self-modifying programs to be created.

Remark: the last general purpose register (r31) is protected against access as >8-bits value.
In fact you have three extra bytes of general purpose register indirectly accessible using r31 value overflow.

---

### Opcodes structure

#### Instructions opcode

Format: ( o o o o o o b b ) (8 bits)
Bits fields :
* o = instruction code (6 bits)
* b = bits mode (2 bits)

##### Instruction codes
<pre>
Code	Instruction 	Description
0 	NOP		nothing
1 	RET		POP PC
2 	JMP		PC = A
3 	CALL	PUSH PC, PC = A
4 	PUSH	RAM[--SP] = A
5 	BRK		breakpoint(A)
6 	INT		interrupt(A)
7 	INC		A = A + 1
8 	DEC		A = A - 1
9 	CLR		A = 0
10	SET		A = MAX_VALUE
11	NOT		A = ~A
12	NEG		A = !A
13	SWAP	A = swap(A)
14	JNN		JMP if (A)
15	JN		JMP if !(A)
16	SNN		SKIP if (A)
17	SN		SKIP if !(A)
18	POP		A = RAM[SP++]
19	ADD		A = A + B
20	SUB		A = A - B
21	MUL		A = A * B
22	DIV		A = A / B
23	AND		A = A &amp; B
24	NAND	A = ~(A &amp; B)
25	OR		A = A | B
26	NOR		A = ~(A | B)
27	XOR		A = A ^ B
28	SBI		A |= 1 &lt;&lt; B
29	CLI		A &amp;=  ~(1 &lt;&lt; B)
30	LSL		A = A &lt;&lt; B
31	LSR		A = A &gt;&gt; B
32	ROL		A = ((A &amp; MSB_MASK) ? LSB_MASK : 0) | (A &lt;&lt; B)
33	ROR		A = ((A &amp; LSB_MASK) ? MSB_MASK : 0) | (A &gt;&gt; B)
34	MOV		A = B
35	CXH		tmp = A, A = B, B = tmp
36	JE		JMP if A == B
37	JNE		JMP if A != B
38	JG		JMP if A &gt; B
39	JGE		JMP if A &gt;= B
40	JL		JMP if A &lt; B
41	JLE		JMP if A &lt;= B
42	JBC		JMP if !(A &amp; (1 &lt;&lt; B))
43	JBS		JMP if A &amp; (1 &lt;&lt; B)
44	SE		SKIP if A == B
45	SNE		SKIP if A != B
46	SG		SKIP if A &gt; B
47	SGE		SKIP if A &gt;= B
48	SL		SKIP if A &lt; B
49	SLE		SKIP if A &lt;= B
50	SBC		SKIP if !(A &amp; (1 &lt;&lt; B))
51	SBS 	SKIP if A &amp; (1 &lt;&lt; B)
</pre>

##### Bits modes
<pre>
Code	Mode
0	invalid mode
1	8 bits mode
2	16 bits mode
3	32 bits mode
</pre>

#### Arguments opcode

Format: ( c p s r r r r r ) (8 bits)
Bits fields :
* c = constant value if set, regular register if clear
* p = pointer if set, raw value if clear
* s = special function register (SP, PC) if set, general purpose register if clear OR "inline constant" (if c = 1) if set, normal constant if clear
* r = register code OR inline constant value (if c = 1 && s = 1)

##### General purpose register codes
<pre>
Code 	Register name
0 	r0
1 	r1
2 	r2
3 	r3
4 	r4
5 	r5
6 	r6
7 	r7
8 	r8
9 	r9
10	r10
11	r11
12	r12
13	r13
14	r14
15	r15
16	r16
17	r17
18	r18
19	r19
20	r20
21	r21
22	r22
23	r23
24	r24
25	r25
26	r26
27	r27
28	r28
29	r29
30	r30
31	r31
</pre>

##### Special function register codes
<pre>
Code	Register name
0	PC (program counter)
1	SP (stack pointer)
</pre>

#### Bitcode stream example

The following bitstream :
<pre>0x15, 0x80, 0x2A</pre>

Is equal to :
<pre>[ 0 0 0 1 0 1 0 1 ], [ 1 0 0 0 0 0 0  0 ], [ 0 0 1 0 1 0 1 0 ]</pre>

Let make some decoding :
<pre>[ 0 0 0 1 0 1 0 1 ] -> instruction byte = BRK in 8 bits mode
[ 1 0 0 0 0 0 0 0 ] -> argument A byte = constant value, not a pointer, not inline
[ 0 0 1 0 1 0 1 0 ] -> constant value (1 byte because we are working in 8 bits mode) = 42</pre>

Or finaly in pure assembly code (using my own assembly notation) :
<pre>BRK.b #42</pre>

#### Currently in progress
* Debugging of cpu core
* Brainstorming on the INT operation callback
* Coding & testing SkyASM assembler program