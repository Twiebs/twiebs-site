%title(Understanding and working with C++ Lambdas)
%date(2016,05,05)
%permalink(2016-05-05)

I love the idea of C++ Lambdas but they are often unwieldly to work with and the details of exactly what they are doing are opaque at best.
I often find myself doubting what the compiler might be doing when the only thing i ever want it to do is create a static function
whoes only accessable from the local namespace of the callsite.  One of the main reasons I find myself reaching into the toolbox for a lambda
is to use recursion.  Many times a recursive procdure is only called in two places; at the inital condition and within the routine itself.  
This is the perfect use for a lambda since there is no reason to polute the global namespace with a procedure that is only invoked one time from another.

Lets take a look at some very simple conventional lambda examples.
This is a simplified version of how labmdas are typicaly used.  Either they are 
plain functions with no capture list, a function with just capture list or a combination of both.

####TLDR
Use non-static function pointers to declare lambdas

```
//Example 1
auto transform_stuff = [](int stuff) -> int {
  int result = stuff * 7;
  return result;
};

//Example 2
int thing = 3;
auto transform_thing = [&thing]() {
  thing = thing * 7;
};

//Example 3
auto transform_things_and_stuff = [&thing](int stuff) -> int {
  thing = thing * 7;
  int result = stuff * 7;
  return result;
}

int resultA = transform_stuff(2);
transform_thing();
int resultB = transform_things_and_stuff(2);

```

This would be idealy what we would want a recursive lambda to look like (Actualy its not ideal at all it should look exactly like other function declerations 
but this is what we are stuck with) however; c++ makes things more complicated then that.  The auto keyword in the C++ standard resolves the type only after the lambda
has been fully defined.  Since factorial is called inside of factorial the type of factorial has not been determined and this will fail to compile

```

//Fails to compile
auto factorial = [](int n) {
  if (n == 0) return 1;
  return n * factorial(n - 1);
}

//Compiles since factoirals type is now known
//Also notice that we must capture factorial by refrence
std::function<int (int)> factorial = [&factorial](int n) -> int { 
  if (n == 0) return 1;
  return n * factorial(n - 1);
};


```

Thats some serious dark magic.  Whats the machine code look like?
This was compiled using clang 3.7.1 with -O3 enabled

```
0000000000400810 <main>:
  400810:	53                   	push   rbx
  400811:	48 83 ec 30          	sub    rsp,0x30
  400815:	48 8d 7c 24 08       	lea    rdi,[rsp+0x8]
  40081a:	48 89 7c 24 08       	mov    QWORD PTR [rsp+0x8],rdi
  40081f:	48 c7 44 24 20 a0 08 	mov    QWORD PTR [rsp+0x20],0x4008a0
  400826:	40 00 
  400828:	48 c7 44 24 18 e0 08 	mov    QWORD PTR [rsp+0x18],0x4008e0
  40082f:	40 00 
  400831:	c7 44 24 2c 09 00 00 	mov    DWORD PTR [rsp+0x2c],0x9
  400838:	00 
  400839:	48 8d 74 24 2c       	lea    rsi,[rsp+0x2c]
  40083e:	e8 5d 00 00 00       	call   4008a0 <_ZNSt17_Function_handlerIFiiEZ4mainE3$_0E9_M_invokeERKSt9_Any_dataOi>
  400843:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
  400848:	48 85 c0             	test   rax,rax
  40084b:	74 0f                	je     40085c <main+0x4c>
  40084d:	48 8d 7c 24 08       	lea    rdi,[rsp+0x8]
  400852:	ba 03 00 00 00       	mov    edx,0x3
  400857:	48 89 fe             	mov    rsi,rdi
  40085a:	ff d0                	call   rax
  40085c:	31 c0                	xor    eax,eax
  40085e:	48 83 c4 30          	add    rsp,0x30
  400862:	5b                   	pop    rbx
  400863:	c3                   	ret    
  400864:	48 89 c3             	mov    rbx,rax
  400867:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
  40086c:	48 85 c0             	test   rax,rax
  40086f:	74 0f                	je     400880 <main+0x70>
  400871:	48 8d 7c 24 08       	lea    rdi,[rsp+0x8]
  400876:	ba 03 00 00 00       	mov    edx,0x3
  40087b:	48 89 fe             	mov    rsi,rdi
  40087e:	ff d0                	call   rax
  400880:	48 89 df             	mov    rdi,rbx
  400883:	e8 68 fe ff ff       	call   4006f0 <_Unwind_Resume@plt>
  400888:	48 89 c7             	mov    rdi,rax
  40088b:	e8 80 00 00 00       	call   400910 <__clang_call_terminate>
  400890:	48 89 c7             	mov    rdi,rax
  400893:	e8 78 00 00 00       	call   400910 <__clang_call_terminate>
  400898:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
  40089f:	00 

00000000004008a0 <_ZNSt17_Function_handlerIFiiEZ4mainE3$_0E9_M_invokeERKSt9_Any_dataOi>:
  4008a0:	53                   	push   rbx
  4008a1:	48 83 ec 10          	sub    rsp,0x10
  4008a5:	8b 1e                	mov    ebx,DWORD PTR [rsi]
  4008a7:	b8 01 00 00 00       	mov    eax,0x1
  4008ac:	85 db                	test   ebx,ebx
  4008ae:	74 1c                	je     4008cc <_ZNSt17_Function_handlerIFiiEZ4mainE3$_0E9_M_invokeERKSt9_Any_dataOi+0x2c>
  4008b0:	48 8b 3f             	mov    rdi,QWORD PTR [rdi]
  4008b3:	8d 43 ff             	lea    eax,[rbx-0x1]
  4008b6:	89 44 24 0c          	mov    DWORD PTR [rsp+0xc],eax
  4008ba:	48 83 7f 10 00       	cmp    QWORD PTR [rdi+0x10],0x0
  4008bf:	74 11                	je     4008d2 <_ZNSt17_Function_handlerIFiiEZ4mainE3$_0E9_M_invokeERKSt9_Any_dataOi+0x32>
  4008c1:	48 8d 74 24 0c       	lea    rsi,[rsp+0xc]
  4008c6:	ff 57 18             	call   QWORD PTR [rdi+0x18]
  4008c9:	0f af c3             	imul   eax,ebx
  4008cc:	48 83 c4 10          	add    rsp,0x10
  4008d0:	5b                   	pop    rbx
  4008d1:	c3                   	ret    
  4008d2:	e8 e9 fd ff ff       	call   4006c0 <_ZSt25__throw_bad_function_callv@plt>
  4008d7:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
  4008de:	00 00 

00000000004008e0 <_ZNSt14_Function_base13_Base_managerIZ4mainE3$_0E10_M_managerERSt9_Any_dataRKS3_St18_Manager_operation>:
  4008e0:	83 fa 02             	cmp    edx,0x2
  4008e3:	74 0b                	je     4008f0 <_ZNSt14_Function_base13_Base_managerIZ4mainE3$_0E10_M_managerERSt9_Any_dataRKS3_St18_Manager_operation+0x10>
  4008e5:	83 fa 01             	cmp    edx,0x1
  4008e8:	75 0e                	jne    4008f8 <_ZNSt14_Function_base13_Base_managerIZ4mainE3$_0E10_M_managerERSt9_Any_dataRKS3_St18_Manager_operation+0x18>
  4008ea:	48 89 37             	mov    QWORD PTR [rdi],rsi
  4008ed:	31 c0                	xor    eax,eax
  4008ef:	c3                   	ret    
  4008f0:	48 8b 06             	mov    rax,QWORD PTR [rsi]
  4008f3:	48 89 07             	mov    QWORD PTR [rdi],rax
  4008f6:	eb 0e                	jmp    400906 <_ZNSt14_Function_base13_Base_managerIZ4mainE3$_0E10_M_managerERSt9_Any_dataRKS3_St18_Manager_operation+0x26>
  4008f8:	85 d2                	test   edx,edx
  4008fa:	75 0a                	jne    400906 <_ZNSt14_Function_base13_Base_managerIZ4mainE3$_0E10_M_managerERSt9_Any_dataRKS3_St18_Manager_operation+0x26>
  4008fc:	48 c7 07 b8 09 40 00 	mov    QWORD PTR [rdi],0x4009b8
  400903:	31 c0                	xor    eax,eax
  400905:	c3                   	ret    
  400906:	31 c0                	xor    eax,eax
  400908:	c3                   	ret    
  400909:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]

```

Here are the relevant parts that we are intrested in




##Dont use std::function

##Never use the `static` keyword

