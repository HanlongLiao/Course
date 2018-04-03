.586
.model flat, stdcall
option casemap:NONE

include msvcrt.inc
includelib msvcrt.lib
include Irvine32.inc

fopen PROTO C: dword, :dword
fscanf PROTO C: dword, :dword, :dword
printf PROTO C: dword, :VARARG

.data
Msg  byte "Time: %d ns", 0ah, 0
fname byte "F:\\vscodework\\Compiling\\test100000.txt", 0
format byte "rb+", 0
Array dword 100 dup(0)
dformat db "%d", 0
fp dword ?
i dword ?
array dword ?
start dword ?
endss   dword ?
arraysort dword ?
left dword ?
m dword ?
right dword ?
n dword ?
j dword ?
k dword ?
aux dword ?
num1 dword 0
num2 dword 214748364
sec dword 3
datasize byte "Data Size: %d",0ah, 0
time byte "Time: "

.code
Merge proc
          push        ebp  
          mov         ebp,esp  
          mov         eax, dword ptr[ebp + 8]
          mov         arraysort, eax
          mov         eax, dword ptr[ebp + 12]
          mov         left, eax
		  mov         eax, dword ptr[ebp + 16]
		  mov         m, eax
		  mov         eax, dword ptr[ebp + 20]
		  mov         right, eax
          push        ebx  
          push        esi  
          push        edi  
          lea         edi,[ebp-620D8h]  
          mov         ecx,18836h  
          mov         eax,0CCCCCCCCh  

          rep stos    dword ptr es:[edi]  
	
          mov         dword ptr [aux],0  
          push        61AA4h  
          push        0  
          lea         eax,[ebp-61AACh]  
          push        eax  
          add         esp,0Ch  
	
          mov         eax,dword ptr [left]  
          mov         dword ptr [n],eax  
          mov         ecx,dword ptr [m]  
          add         ecx,1  
          mov         dword ptr [j],ecx  
          mov         dword ptr [k],0  
          jmp         s4 
s7:
          mov         eax,dword ptr [k]  
          add         eax,1  
          mov         dword ptr [k],eax  
s4:
          mov         eax,dword ptr [right]  
          sub         eax,dword ptr [left]  
          cmp         dword ptr [k],eax  
          jg          s5  
          mov         eax,dword ptr [m]  
          add         eax,1  
          cmp         dword ptr [n],eax  
          jne         s6  
          mov         eax,dword ptr [k]  
          mov         ecx,dword ptr [j]  
          mov         edx,dword ptr [arraysort]  
          mov         ecx,dword ptr [edx+ecx*4]  
          mov         dword ptr aux[eax*4],ecx  
          mov         edx,dword ptr [j]  
          add         edx,1  
          mov         dword ptr [j],edx  
          jmp         s7  
s6:
          mov         eax,dword ptr [right]  
          add         eax,1  
          cmp         dword ptr [j],eax  
          jne         s8
          mov         eax,dword ptr [k]  
          mov         ecx,dword ptr [n]  
          mov         edx,dword ptr [arraysort]  
          mov         ecx,dword ptr [edx+ecx*4]  
          mov         dword ptr aux[eax*4],ecx  
          mov         edx,dword ptr [n]  
          add         edx,1  
          mov         dword ptr [n],edx  
          jmp        s7 
s8:
          mov         eax,dword ptr [n]  
          mov         ecx,dword ptr [arraysort]  
          mov         edx,dword ptr [j]  
          mov         esi,dword ptr [arraysort]  
          mov         eax,dword ptr [ecx+eax*4]  
          cmp         eax,dword ptr [esi+edx*4]  
          jge         s9  
          mov         eax,dword ptr [k]  
          mov         ecx,dword ptr [n]  
          mov         edx,dword ptr [arraysort]  
          mov         ecx,dword ptr [edx+ecx*4]  
          mov         dword ptr aux[eax*4],ecx  
          mov         edx,dword ptr [n]  
          add         edx,1  
          mov         dword ptr [n],edx  
          jmp         s7  
          jmp         s10  
s9:
          mov         eax,dword ptr [k]  
          mov         ecx,dword ptr [j]  
          mov         edx,dword ptr [arraysort]  
          mov         ecx,dword ptr [edx+ecx*4]  
          mov         dword ptr aux[eax*4],ecx  
          mov         edx,dword ptr [j]  
          add         edx,1  
          mov         dword ptr [j],edx  
s10:
          jmp         s7
s5:
          mov         eax,dword ptr [left]  
          mov         dword ptr [n],eax  
          mov         dword ptr [j],0  
          jmp         s11 
s13:
          mov         eax,dword ptr [n]  
          add         eax,1  
          mov         dword ptr [n],eax  
          mov         ecx,dword ptr [j]  
          add         ecx,1  
          mov         dword ptr [j],ecx  
s11:
          mov         eax,dword ptr [n]  
          cmp         eax,dword ptr [right]  
          jg         s12 
          mov         eax,dword ptr [n]  
          mov         ecx,dword ptr [arraysort]  
          mov         edx,dword ptr [j]  
          mov         edx,dword ptr aux[edx*4]  
          mov         dword ptr [ecx+eax*4],edx  
          jmp        s13 
s12:
          push        edx  
          mov         ecx,ebp   
          pop         edx  
          pop         edi  
          pop         esi  
          pop         ebx  
          mov         ecx,dword ptr [ebp-4]  
          xor         ecx,ebp  
          add         esp,620D8h  
          cmp         ebp,esp  
          mov         esp,ebp  
          pop         ebp  
          ret  
Merge endp 

MergeSort proc 
          push        ebp  
          mov         ebp,esp  
          sub         esp,0CCh
		  mov         eax, dword ptr[ebp + 8]
          mov         array, eax
		  mov         eax, dword ptr[ebp + 12]  
		  mov         start, eax
		  mov         eax, dword ptr[ebp + 16]
		  mov         endss, eax
          push        ebx  
          push        esi  
          push        edi  
          lea         edi,[ebp-0CCh]  
          mov         ecx,33h  
          mov         eax,0CCCCCCCCh  
          rep stos    dword ptr es:[edi]  
	
          mov         eax,dword ptr [start]  
          cmp         eax,dword ptr [endss]  
          jge        s3

          mov         eax,dword ptr [start]  
          add         eax,dword ptr [endss]  
          cdq  
          sub         eax,edx  
          sar         eax,1  
          mov         dword ptr [ebp-8],eax  
          mov         eax,dword ptr [ebp-8]  
          push        eax  
          mov         ecx,dword ptr [start]  
          push        ecx  
          mov         edx,dword ptr [array]  
          push        edx  
          call        MergeSort 
          add         esp,0Ch  
          mov         eax,dword ptr [endss]  
          push        eax  
          mov         ecx,dword ptr [ebp-8]  
          add         ecx,1  
          push        ecx  
          mov         edx,dword ptr [array]  
          push        edx  
          call        MergeSort  
          add         esp,0Ch  
          mov         eax,dword ptr [endss]  
          push        eax  
          mov         ecx,dword ptr [ebp-8]  
          push        ecx  
          mov         edx,dword ptr [start]  
          push        edx  
          mov         eax,dword ptr [array]  
          push        eax  
          call        Merge 
          add         esp,10h  
s3:
          pop         edi  
          pop         esi  
          pop         ebx  
          add         esp,0CCh  
          cmp         ebp,esp   
          mov         esp,ebp  
          pop         ebp  
          ret  
MergeSort endp

main proc
		  invoke      fopen, offset fname, offset format
		  mov         dword ptr [fp],eax 
          mov         dword ptr [i],0  
s2:
          mov         eax,dword ptr [i]  
          lea         ecx,Array[eax*4]  
          mov         edx,dword ptr [fp]        
		  invoke      fscanf, edx, offset dformat, ecx    
          cmp         eax,0FFFFFFFFh  
          je          s1 
          mov         eax,dword ptr [i]  
          add         eax,1  
          mov         dword ptr [i],eax  
          jmp         s2  
s1:
		  rdtsc
	      mov num1, eax
	      xor edx, edx
          mov         eax,dword ptr [i]  
          sub         eax,1  
          push        eax  
          push        0  
          push        offset Array 
          call        MergeSort 
          add         esp,0Ch 
		  rdtsc
		  sub         eax, num1
		  cmp		  num1, 0
		  jnl		  t1
		  add		  eax, num2
t1:
		  xor		  edx, edx
		  mul		  sec
		  cmp		  edx, 0
		  jz		  t2
		  inc		  eax
t2:
		 invoke printf, offset datasize, offset i
		 invoke printf, offset time, eax
         ret
main endp
end main