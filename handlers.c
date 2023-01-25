#include <stdio.h>
#include <string.h>

__attribute__ ((section (".stack")))
__attribute__((unused))
static char __stack[1024*1024];


void main();

extern void __bss_start();
extern void __bss_end();
extern void __rodata_start();
extern void __rodata_end();
extern void __data_start();
extern void __data_end();
extern void __stack_start();
extern void __stack_end();

struct SCB {
	volatile unsigned int cpuid;               /* +00 */
	volatile unsigned int icsr;                /* +04 */
	volatile unsigned int vtor;                /* +08 */
	volatile unsigned int aircr;               /* +0c */
	volatile unsigned int scr;                 /* +10 */
	volatile unsigned int ccr;                 /* +14 */
	volatile unsigned int shpr1;               /* +18 */
	volatile unsigned int shpr2;               /* +1c */
	volatile unsigned int shpr3;               /* +20 */
	volatile unsigned int shcsr;               /* +24 */
	volatile unsigned int cfsr;                /* +28 */
	volatile unsigned int hfsr;                /* +2c */
	volatile unsigned int dfsr;                /* +30 */
	volatile unsigned int mmfar;               /* +34 */
	volatile unsigned int bfar;                /* +38 */
	volatile unsigned int afsr;                /* +3c */
	volatile unsigned int __reserved1[0x12];   /* +40 -- +84 */
	volatile unsigned int cpacr;               /* +88 */
	volatile unsigned int __reserved2;         /* +8c */
};

#define __SCB ((struct SCB *)0xe000ed00UL)

static void enable_fpu() {
	/**
	 *  Enable access to CP10, CP11
	 */
	__SCB->cpacr |= 0x00f00000U;
}

void DefaultResetHandler() {
	memset(__bss_start, 0,
	       (unsigned long)__bss_end - (unsigned long)__bss_start);
	enable_fpu();
	main();
}

static void dump_info_HardFault() {
	unsigned int hfsr = __SCB->hfsr;

	printf("\n");
	printf("hfsr = %08x\n", hfsr);
	printf("  debugevt = %d (hfsr[31])\n", (hfsr >> 31) & 1);
	printf("  forced   = %d (hfsr[30])\n", (hfsr >> 30) & 1);
	printf("  vecttbl  = %d (hfsr[ 1])\n", (hfsr >>  1) & 1);
}

static const struct exc_info {
	const char *name;
	void (*dump_info)();
} __exc_info[] = {
	{ .name = NULL,           .dump_info = NULL },
	{ .name = "Reset",        .dump_info = NULL },
	{ .name = "NMI",          .dump_info = NULL },
	{ .name = "HardFault",    .dump_info = dump_info_HardFault },
	{ .name = "MemManage",    .dump_info = NULL },
	{ .name = "BusFault",     .dump_info = NULL },
	{ .name = "UsageFault",   .dump_info = NULL },
	{ .name = NULL,           .dump_info = NULL },
	{ .name = NULL,           .dump_info = NULL },
	{ .name = NULL,           .dump_info = NULL },
	{ .name = NULL,           .dump_info = NULL },
	{ .name = "SVCall",       .dump_info = NULL },
	{ .name = "DebugMonitor", .dump_info = NULL },
	{ .name = NULL,           .dump_info = NULL },
	{ .name = "PendSV",       .dump_info = NULL },
	{ .name = "SysTick",      .dump_info = NULL },
};

#define EXC_INFO_COUNT (sizeof(__exc_info)/sizeof(__exc_info[0]))

struct ExceptionStack {
	unsigned int r13; /* old sp */
	unsigned int r4;
	unsigned int r5;
	unsigned int r6;
	unsigned int r7;
	unsigned int r8;
	unsigned int r9;
	unsigned int r10;
	unsigned int r11;
	unsigned int r0;
	unsigned int r1;
	unsigned int r2;
	unsigned int r3;
	unsigned int r12;
	unsigned int r14; /* lr */
	unsigned int pc;
	unsigned int xpsr;
};

__attribute__((weak))
void __DefaultExceptionHandler(unsigned int ipsr, struct ExceptionStack *regs) {
	const struct exc_info *info = NULL;

	printf("\n\n");
	printf("Unhandled exception occurred\n");
	printf("\n");
	if (ipsr < EXC_INFO_COUNT) {
		info = &__exc_info[ipsr];
		if (info->name) {
			printf("ipsr = %08x (%s)\n", ipsr, info->name);
		} else {
			printf("ipsr = %08x (Unknown Exception)\n", ipsr);
		}
	} else {
		printf("ipsr = %08x (External interrupt #%u)\n", ipsr, ipsr - 16);
	}
	printf("\n");
	printf("r0  = %08x         r1  = %08x\n", regs->r0,  regs->r1);
	printf("r2  = %08x         r3  = %08x\n", regs->r2,  regs->r3);
	printf("r4  = %08x         r5  = %08x\n", regs->r4,  regs->r5);
	printf("r6  = %08x         r7  = %08x\n", regs->r6,  regs->r7);
	printf("r8  = %08x         r9  = %08x\n", regs->r8,  regs->r9);
	printf("r10 = %08x         r11 = %08x\n", regs->r10, regs->r11);
	printf("r12 = %08x         r13 = %08x (sp)\n", regs->r12, regs->r13);
	printf("r14 = %08x (lr)    r15 = %08x (pc)\n", regs->r14, regs->pc);
	printf("xPSR = %08x\n", regs->xpsr);

	if (info->dump_info) {
		info->dump_info();
	}

	while (1)
		;
}

__attribute__((naked))
void DefaultExceptionHandler() {
	asm volatile (
		"mrs r0, ipsr\n"
		"and r0, r0, 0x1f\n"
		"ldr r2, [sp, #0x1c]\n"
		"asr r2, r2, #7\n"
		"and r2, r2, #4\n"
		"add r3, sp, #0x20\n"
		"orr r3, r3, r2\n"
		"stmdb sp!, {r3-r11}\n"
		"mov r1, sp\n"
		"b __DefaultExceptionHandler\n"
		:
		:
		: "r0"
	);
}

void ResetHandler() __attribute__((weak, alias("DefaultResetHandler")));
void NMIHandler() __attribute__((weak, alias("DefaultExceptionHandler")));
void HardFaultHandler() __attribute__((weak, alias("DefaultExceptionHandler")));
void MemManageHandler() __attribute__((weak, alias("DefaultExceptionHandler")));
void BusFaultHandler() __attribute__((weak, alias("DefaultExceptionHandler")));
void UsageFaultHandler() __attribute__((weak, alias("DefaultExceptionHandler")));
void SVCallHandler() __attribute__((weak, alias("DefaultExceptionHandler")));
void DebugMonitorHandler() __attribute__((weak, alias("DefaultExceptionHandler")));
void PendSVHandler() __attribute__((weak, alias("DefaultExceptionHandler")));
void SysTickHandler() __attribute__((weak, alias("DefaultExceptionHandler")));

void IRQ0Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ1Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ2Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ3Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ4Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ5Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ6Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ7Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ8Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ9Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ10Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ11Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ12Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ13Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ14Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ15Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ16Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ17Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ18Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ19Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ20Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ21Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ22Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ23Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ24Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ25Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ26Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ27Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ28Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ29Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ30Handler() __attribute__((weak, alias("DefaultExceptionHandler")));
void IRQ31Handler() __attribute__((weak, alias("DefaultExceptionHandler")));

__attribute__ ((section (".vector")))
__attribute__((unused))
static void *vector_table[] = {
	/*  0 Initial SP */ __stack_end,
	/*  1 Reset      */ ResetHandler,
	/*  2 NMI        */ NMIHandler,
	/*  3 HardFault  */ HardFaultHandler,
	/*  4 MemManage  */ MemManageHandler,
	/*  5 BusFault   */ BusFaultHandler,
	/*  6 UsageFault */ UsageFaultHandler,
	/*  7 Reserved   */ NULL,
	/*  8 Reserved   */ NULL,
	/*  9 Reserved   */ NULL,
	/* 10 Reserved   */ NULL,
	/* 11 SVCall     */ SVCallHandler,
	/* 12 Reserved   */ DebugMonitorHandler,
	/* 13 Reserved   */ NULL,
	/* 14 PendSV     */ PendSVHandler,
	/* 15 SysTick    */ SysTickHandler,

	/* 16 IRQ0       */ IRQ0Handler,
	/* 17 IRQ1       */ IRQ1Handler,
	/* 18 IRQ2       */ IRQ2Handler,
	/* 19 IRQ3       */ IRQ3Handler,
	/* 20 IRQ4       */ IRQ4Handler,
	/* 21 IRQ5       */ IRQ5Handler,
	/* 22 IRQ6       */ IRQ6Handler,
	/* 23 IRQ7       */ IRQ7Handler,
	/* 24 IRQ8       */ IRQ8Handler,
	/* 25 IRQ9       */ IRQ9Handler,
	/* 26 IRQ10      */ IRQ10Handler,
	/* 27 IRQ11      */ IRQ11Handler,
	/* 28 IRQ12      */ IRQ12Handler,
	/* 29 IRQ13      */ IRQ13Handler,
	/* 30 IRQ14      */ IRQ14Handler,
	/* 31 IRQ15      */ IRQ15Handler,
	/* 32 IRQ16      */ IRQ16Handler,
	/* 33 IRQ17      */ IRQ17Handler,
	/* 34 IRQ18      */ IRQ18Handler,
	/* 35 IRQ19      */ IRQ19Handler,
	/* 36 IRQ20      */ IRQ20Handler,
	/* 37 IRQ21      */ IRQ21Handler,
	/* 38 IRQ22      */ IRQ22Handler,
	/* 39 IRQ23      */ IRQ23Handler,
	/* 40 IRQ24      */ IRQ24Handler,
	/* 41 IRQ25      */ IRQ25Handler,
	/* 42 IRQ26      */ IRQ26Handler,
	/* 43 IRQ27      */ IRQ27Handler,
	/* 44 IRQ28      */ IRQ28Handler,
	/* 45 IRQ29      */ IRQ29Handler,
	/* 46 IRQ30      */ IRQ30Handler,
	/* 47 IRQ31      */ IRQ31Handler,
};
