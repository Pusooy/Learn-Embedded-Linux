 ENTRY(_start)
 
 SECTIONS {
   . = 0x80100000;

   . = ALIGN(4);
   .text :
   {
   *(.text)
   }

   . = ALIGN(4);
   .data :
   {
   *(.data)
   }

   . = ALIGN(4);
   .bss :
   {
   *(.bss)
   }
 }