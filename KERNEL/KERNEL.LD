OUTPUT_FORMAT("coff-go32")
ENTRY(start)
SECTIONS
{
  .text : {
    btext  =  . ; _btext = .;
    *(.text)
    etext  =  . ; _etext = .;
    . = ALIGN(0x200);
  }
  .data  ALIGN(0x200) : {
    bdata  =  . ; _bdata = .;
    *(.data)
     edata  =  . ; _edata = .;
     . = ALIGN(0x200);
  }
  .bss  SIZEOF(.data) + ADDR(.data) :
  {
    bbss  =  . ; _bbss = .;
    *(.bss)
    *(COMMON)
     end = . ; _end = .;
     . = ALIGN(0x200);
  }
}
