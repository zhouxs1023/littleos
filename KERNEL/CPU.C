/* CPU ID routines for 386+ CPU's
*
*  Written by Phil Frisbie, Jr. (pfrisbie@geocities.com)
*
*  Parts adapted from the cpuid algorithm by Robert Collins (rcollins@x86.org)
*
*  and from Cyrix sample code.
*
*/

/*
 * Adapted by Pascal Lacroix for LittleOS
 * cpu.c - cpu detection and identification
 */

#include <string.h>

/* added for io: */
#include <pio.h>

#ifndef TRUE
	#define TRUE	1
	#define FALSE	0
#endif

long 	reg_eax, reg_ebx, reg_edx, reg_ecx;
char 	*unknown_vendor = "NoVendorName";
char	*cyrix = "CyrixInstead";


char	cpu_vendor[16];		/* Vendor String, or Unknown */
int	cpu_family;		/* 3=386, 4=486, 5=Pentium, 6=PPro, 7=Pentium ||?, etc */
int	cpu_model;		/* other details such as SX, DX, overdrive, etc. */
int	cpu_fpu = FALSE;	/* TRUE or FALSE */
int	cpu_mmx = FALSE;	/* TRUE or FALSE */
int	cpu_cpuid = FALSE;	/* Whether the cpu supported the cpuid instruction */
				/* if TRUE, you can trust the information returned */
				/* if FALSE, be careful... ;) */


int is_486(void) /* return TRUE for 486+, and FALSE for 386 */
{
	int result;

asm	(
	"pushf ;"			/* save EFLAGS */
	"popl	%%eax ;"		/* get EFLAGS */
	"movl	%%eax, %%ecx ;"		/* temp storage EFLAGS */
	"xorl	$0x40000, %%eax ;"	/* change AC bit in EFLAGS */
	"pushl	%%eax ;"		/* put new EFLAGS value on stack */
	"popf ;"			/* replace current EFLAGS value */
	"pushf ;"			/* get EFLAGS */
	"popl	%%eax ;"		/* save new EFLAGS in EAX */
	"cmpl	%%ecx, %%eax ;"		/* compare temp and new EFLAGS */
	"jz	0f ;"
	"movl	$1, %%eax ;"		/* 80486 present */
	"jmp	1f ;"

	"0:"
	"movl	$0, %%eax ;"		/* 80486 not present */

	"1:"
	"pushl	%%ecx ;"		/* get original EFLAGS */
	"popf ;"			/* restore EFLAGS */
	: "=a" (result)
	:
	: "eax", "ecx", "memory" );

	return result;

}


int is_386DX(void) /* return TRUE for 386DX, and FALSE for 386SX */
{
	int result;

asm	(
	"movl	%%cr0,%%edx ;"		/* Get CR0 */
	"pushl	%%edx ;"		/* save CR0 */
	"andb	$0xef, %%dl ;"		/* clear bit4 */
	"movl	%%edx, %%cr0 ;" 	/* set CR0 */
	"movl	%%cr0, %%edx ;"		/* and read CR0 */
	"andb	$0x10, %%dl ;"		/* bit4 forced high? */
	"popl	%%edx ;"		/* restore reg w/ CR0 */
	"movl	%%edx, %%cr0 ;"		/* restore CR0 */
	"movl	$1, %%eax ;"		/* TRUE, 386DX */
	"jz	0f ;"
	"movl	$0, %%eax ;"		/* FALSE, 386SX */

	"0:"
	: "=a" (result)
	:
	: "%edx", "memory" );

	return result;
}

int is_fpu(void)
{
	int result;

asm (
	"fninit ;"
	"movl    $0x5a5a, %%eax ;"
	"fnstsw  %%eax ;"
	"cmpl    $0, %%eax ;"
	"jne     0f ;"
	"movl    $1, %%eax ;"
	"jmp     1f ;"

	"0:"
	"movl    $0, %%eax ;"

	"1:"
	: "=a" (result)
	:
	: "%eax", "memory" );

	return result;
}

int is_cyrix(void)
{
	int result;

asm (
	"xorw     %%ax, %%ax ;"		/* clear eax */
	"sahf ;"			/* clear flags, bit 1 is always 1 in flags */
	"movw     $5, %%ax ;"
	"movw     $2, %%bx ;"
	"div      %%bl ;"		/* do an operation that does not change flags */
	"lahf ;"			/* get flags */
	"cmpb     $2, %%ah ;"		/* check for change in flags */
	"jne      0f ;"			/* flags changed not Cyrix */
	"movl     $1, %%eax ;"		/* TRUE Cyrix CPU */
	"jmp      1f ;"

	"0:"
	"movl     $0, %%eax ;"		/* FALSE NON-Cyrix CPU */

	"1:"
	: "=a" (result)
	:
	: "%eax", "%ebx", "memory" );

	return result;

}

void cx_w(char index, char value)
{
	
asm	("pushf");			/* save flags */
asm	("cli");			/* clear interrupt in flags */
	outportb(0x22, index);		/* tell CPU which config. register */
	outportb(0x23, value);		/* write to CPU config. register */
asm	("popf");			/* restore flags */
}

char cx_r(char index)
{
	char value;

asm	("pushf");			/* save flags */
asm	("cli");			/* clear interrupt in flags */
	outportb(0x22, index);		/* tell CPU which config. register */
	value = inportb(0x23);		/* read CPU config, register */
asm	("popf");			/* restore flags */
	return value;
}

#define UNKNOWN   0xff
#define Cx486_pr  0xfd  /* ID Register not supported, software created */
#define Cx486S_a  0xfe  /* ID Register not supported, software created */
#define CR2_MASK  0x4   /* LockNW */
#define CR3_MASK  0x80  /* Resereved bit 7 */

void cyrix_type(void)
{
	char temp, orgc2, newc2, orgc3, newc3;
	int cr2_rw=FALSE, cr3_rw=FALSE, type;

	type = UNKNOWN;

	/* Test Cyrix c2 register read/writable */

	orgc2 = cx_r (0xc2);		/* get current c2 value */

	newc2 = orgc2 ^ CR2_MASK;	/* toggle test bit */
	cx_w (0xc2, newc2);		/* write test value to c2 */
	cx_r (0xc0);			/* dummy read to change bus */

	if (cx_r (0xc2) != orgc2)	/* did test bit toggle */
		cr2_rw = TRUE;		/* yes bit changed */

	cx_w (0xc2, orgc2);		/* return c2 to original value */

	/* end c2 read writeable test */

	/* Test Cyrix c3 register read/writable */

	orgc3 = cx_r (0xc3);		/* get current c3 value */

	newc3 = orgc3 ^ CR3_MASK;	/* toggle test bit */
	cx_w (0xc3, newc3);		/* write test value to c3 */
	cx_r (0xc0);			/* dummy read to change bus */

	if (cx_r (0xc3) != orgc3)	/* did test bit change */
		cr3_rw = TRUE;		/* yes it did */

	cx_w (0xc3, orgc3);		/* return c3 to original value */

	/* end c3 read writeable test */

	if ((cr2_rw && cr3_rw) || (!cr2_rw && cr3_rw)) /*DEV ID register ok */
	{
		/* <<<<<<< READ DEVICE ID Reg >>>>>>>> */
		type = cx_r (0xfe);	/* lower byte gets IDIR0 */
	}

	else if (cr2_rw && !cr3_rw)	/* Cx486S A step */
	{
		type = Cx486S_a;	/* lower byte */
	}

	else if (!cr2_rw && !cr3_rw)	/* Pre ID Regs. Cx486SLC or DLC */
	{
		type = Cx486_pr;	/* lower byte */
	}

	/* This could be broken down more, but is it needed? */
	if (type < 0x30 || type > 0xfc)
	{
		cpu_family = 4;		/* 486 class-including 5x86 */
		cpu_model = 15;		/* Unknown */
	}
	else if (type < 0x50)
	{
		cpu_family = 5;		/* Pentium class-6x86 and Media GX */
		cpu_model = 15;		/* Unknown */
	}
	else	
	{
		cpu_family = 6;		/* Pentium || class- 6x86MX */
		cpu_model = 15;		/* Unknown */
		cpu_mmx = TRUE;
	}
}

int is_cpuid_supported(void)
{
	int result;

asm	(
	"pushfl ;"			/* get extended flags */
	"popl     %%eax ;"
	"movl     %%eax, %%ebx ;"	/* save current flags */
	"xorl     $0x200000, %%eax ;"	/* toggle bit 21 */
	"pushl    %%eax ;"		/* put new flags on stack */
	"popfl ;"			/* flags updated now in flags */
	"pushfl ;"			/* get extended flags */
	"popl     %%eax ;"
	"xorl     %%ebx, %%eax ;"	/* if bit 21 r/w then supports cpuid */
	"jz      0f ;"
	"movl     $1, %%eax ;"
	"jmp      1f ;"

	"0:"

	"movl     $0, %%eax ;"

	"1:"
	: "=a" (result)
	:
	: "%eax", "%ebx", "memory" );

	return result;
}

void get_cpuid_info(long cpuid_levels) /* This is so simple! */
{ 
asm (
	"cpuid ;"
	"movl	%%eax, _reg_eax ;"	/* reg_eax = eax */
	"movl	%%ebx, _reg_ebx ;"	/* reg_ebx = ebx */
	"movl	%%ecx, _reg_ecx ;"	/* reg_ecx = ecx */
	"movl	%%edx, _reg_edx ;"	/* reg_edx = edx */
	: :"a" (cpuid_levels)
	: "%eax", "%ebx", "ecx", "edx", "memory" );


}

void check_cpu(void) /* This is the function to call to set the globals */
{
	long cpuid_levels;
	long vendor_temp[3];

	memset(cpu_vendor, 0, 16);
	if (is_cpuid_supported ())
	{
		cpu_cpuid = TRUE;
		reg_eax = reg_ebx = reg_ecx = reg_edx = 0;
		get_cpuid_info(0);
		cpuid_levels = reg_eax;
		vendor_temp[0] = reg_ebx;
		vendor_temp[1] = reg_edx;
		vendor_temp[2] = reg_ecx;
		memcpy(cpu_vendor, vendor_temp, 12);
		if (cpuid_levels > 0)
		{
			reg_eax = reg_ebx = reg_ecx = reg_edx = 0;
			get_cpuid_info (1);
			cpu_family = (reg_eax & 0xf00) >> 8;
			cpu_model = (reg_eax & 0xf0) >> 4;
			cpu_fpu = (reg_edx & 1 ? TRUE : FALSE);
			cpu_mmx = (reg_edx & 0x800000 ? TRUE: FALSE);
		}
	}
	else
	{
		memcpy(cpu_vendor, unknown_vendor, 12);
		cpu_fpu = is_fpu();
		if (!is_486())
		{
			if (is_386DX())	/* It is a 386DX */
			{
				cpu_family = 3;	/* 386 */
				cpu_model = 0;	/* DX */
			}
			else		/* It is a 386SX */
			{
				cpu_family = 3;	/* 386 */
				cpu_model = 1;	/* SX */
			}
		}
		else			/* It is a 486+ */
		{
			if(is_cyrix())
			{
				memcpy(cpu_vendor, cyrix, 12);
				cyrix_type();
			}
			else
			{
				cpu_family = 4;	/* 486 */
				cpu_model = 15;	/* unknown */
			}
		}
	}
}

/* added by Pascal */
void init_cpu(void)
{
	check_cpu();
        printk("\eRCPU:\eN %c86 (Vendor: %s%s%s%s)\n", '0' + cpu_family,
        	cpu_vendor, cpu_cpuid?", cpuid":"", cpu_fpu ?", fpu":"", cpu_mmx?", mmx":"");
}

/* deleted by Pascal Lacroix
int main(void)
{
	check_cpu();
	printf("CPU has cpuid instruction? %s\n", cpu_cpuid ? "yes": "no");
	printf("CPU vender is %s\n", cpu_vendor);
	printf("CPU family is %d\n", cpu_family);
	printf("CPU model is %d\n", cpu_model);
	printf("CPU has fpu? %s\n", cpu_fpu ? "yes": "no");
	printf("CPU has mmx? %s\n", cpu_mmx ? "yes": "no");
	return 1;
}
*/

