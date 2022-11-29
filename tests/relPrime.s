.section text
   # SP = 0xC000;
   lui   C00`16
   lac   a0
   setsp a0
   
   # a0 = relPrime(switches0);
   lui   FF8`16 # 0x05
   lac   a0
   lm    a0, a0
   jal   relPrime
   
   # hex0 = a0;
   lui   FFF`16 # 0x0C
   addi  C`16
   lac   a1
   sm    a1, a0
   
   # return;
   jal   0


relPrime: # 0x14
   push  ra
   push  s0
   push  s1
   
   # int m = 2;
   sac   a0
   lac   s1
   saci  2
   
   # while(gcd(n, m) != 1) {
   relPrime_Loop: # 0x1D
      lac   s0
      swp   a0, s1
      lac   a1
      jal   gcd
      saci -1
      add   a1
      bez   relPrime_End
      
      # m = m + 1;
      sac   s0
      addi  1
      jal   relPrime_Loop
   # }
   relPrime_End: # 0x2B
   
   # return m;
   pop   s1
   pop   s0
   pop   ra
   jalr  ra


gcd: # 0x32
   ##if(a == 0) return b;
   sac   a0
   bnez  gcd_NoReturn
   sac   a1
   lac   a0
   jalr  ra
   gcd_NoReturn: # 0x38
   
   push  ra
   
   # while(b != 0) {
   gcd_Loop: # 0x3A
      sac   a1
      bez   gcd_End
      
      ##if(b < a) {
      slt   a0
      bez   gcd_DecB
         # a = a - b;
         sac   a0
         sub   a1
         lac   a0
         jal   gcd_Loop
      # } else {
      gcd_DecB: # 0x45
         # b = b - a;
         sac   a1
         sub   a0
         lac   a1
         jal   gcd_Loop
      # }
   # }
   gcd_End: # 0x4A
   
   pop  ra
   jalr ra