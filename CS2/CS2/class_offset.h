#pragma once



/*client.dll+739120 - 48 83 EC 18           - sub rsp,18 { 24 }
client.dll+739124 - 4C 63 C2              - movsxd  r8,edx
client.dll+739127 - 4D 69 D0 50430000     - imul r10,r8,00004350 { 17232 }
client.dll+73912E - 4B 8D 04 40           - lea rax,[r8+r8*2]
client.dll+739132 - 8B 84 C1 A0450000     - mov eax,[rcx+rax*8+000045A0]
client.dll+739139 - 85 C0                 - test eax,eax
client.dll+73913B - 74 23                 - je client.dll+739160
client.dll+73913D - FF C8                 - dec eax
client.dll+73913F - 4F 8D 0C 40           - lea r9,[r8+r8*2]
client.dll+739143 - 48 63 D0              - movsxd  rdx,eax
client.dll+739146 - 4A 8B 84 C9 A8450000  - mov rax,[rcx+r9*8+000045A8]
client.dll+73914E - 48 69 D2 08010000     - imul rdx,rdx,00000108 { 264 }
client.dll+739155 - 48 05 F8000000        - add rax,000000F8 { 248 }
client.dll+73915B - 48 03 D0              - add rdx,rax
client.dll+73915E - EB 0A                 - jmp client.dll+73916A
client.dll+739160 - 48 8D 91 18450000     - lea rdx,[rcx+00004518]
client.dll+739167 - 49 03 D2              - add rdx,r10
client.dll+73916A - 41 80 BC 0A F1430000 00 - cmp byte ptr [r10+rcx+000043F1],00 { 0 }
client.dll+739173 - F2 0F10 02            - movsd xmm0,[rdx]
client.dll+739177 - F2 0F11 04 24         - movsd [rsp],xmm0
client.dll+73917C - 75 34                 - jne client.dll+7391B2
client.dll+73917E - F3 0F10 44 24 04      - movss xmm0,[rsp+04]
client.dll+739184 - F3 0F10 0C 24         - movss xmm1,[rsp]
client.dll+739189 - 41 C6 84 0A F1430000 01 - mov byte ptr [r10+rcx+000043F1],01 { 1 }
client.dll+739192 - F3 41 0F11 84 0A FC430000  - movss [r10+rcx+000043FC],xmm0
client.dll+73919C - F3 41 0F11 8C 0A F8430000  - movss [r10+rcx+000043F8],xmm1
client.dll+7391A6 - 41 C7 84 0A 00440000 0000F041 - mov [r10+rcx+00004400],41F00000 { 30.00 }
client.dll+7391B2 - 41 C7 84 0A 30450000 00000000 - mov [r10+rcx+00004530],00000000 { 0 }
client.dll+7391BE - 48 83 C4 18           - add rsp,18 { 24 }
client.dll+7391C2 - C3                    - ret 
*/

#define OFFSET_TO_THIRD_PERSON "CC CC 48 83 ?? ?? 4C ?? ?? 4D 69 ?? ?? ?? ?? ?? 4B 8D ?? ?? 8B 84 ?? ?? ?? ?? ?? 85 C0"

/*client.dll+63C2DA - FF 10                 - call qword ptr [rax]
client.dll+63C2DC - 48 8B 0D A5612C01     - mov rcx,[client.dll+1902488] { (0) }
client.dll+63C2E3 - 48 8B 15 7EA91501     - mov rdx,[client.dll+1796C68] { (18E5FCF10A0) }
client.dll+63C2EA - 48 8B 01              - mov rax,[rcx]
client.dll+63C2ED - FF 90 C0000000        - call qword ptr [rax+000000C0]
client.dll+63C2F3 - 40 38 3D 56961C01     - cmp [client.dll+1805950],dil { (1) }
client.dll+63C2FA - 0F85 D9000000         - jne client.dll+63C3D9
client.dll+63C300 - 48 8B 0D A1A01C01     - mov rcx,[client.dll+18063A8] { (7FFA5D2AB510) }
client.dll+63C307 - C6 05 42961C01 01     - mov byte ptr [client.dll+1805950],01 { (1),1 }
client.dll+63C30E - 0F29 74 24 70         - movaps [rsp+70],xmm6
client.dll+63C313 - 48 8B 01              - mov rax,[rcx]
client.dll+63C316 - FF 90 68030000        - call qword ptr [rax+00000368]
client.dll+63C31C - 48 8B 0D CDCC0801     - mov rcx,[client.dll+16C8FF0] { (192) } //[client.dll+16C8FF0] 为input类
client.dll+63C323 - 48 8B 01              - mov rax,[rcx]
client.dll+63C326 - FF 50 18              - call qword ptr [rax+18]
client.dll+63C329 - 8B DF                 - mov ebx,edi
client.dll+63C32B - 0F1F 44 00 00         - nop dword ptr [rax+rax+00]
client.dll+63C330 - 48 8B 0D 71A01C01     - mov rcx,[client.dll+18063A8] { (7FFA5D2AB510) }
client.dll+63C337 - 48 8D 94 24 B0000000  - lea rdx,[rsp+000000B0]
*/

#define OFFSET_INPUT_CLASS "0F 29 ?? ?? ?? 48 8B ?? FF ?? ?? ?? ?? ?? 48 8B ?? ?? ?? ?? ?? 48 8B ?? FF"


/*client.dll+74632E - CC                    - int 3 
client.dll+74632F - CC                    - int 3 
client.dll+746330 - 48 89 5C 24 08        - mov [rsp+08],rbx
client.dll+746335 - 57                    - push rdi
client.dll+746336 - 48 83 EC 20           - sub rsp,20 { 32 }
client.dll+74633A - 4C 63 D2              - movsxd  r10,edx
client.dll+74633D - 48 8B D9              - mov rbx,rcx
client.dll+746340 - B8 B5814E1B           - mov eax,1B4E81B5 { 458129845 }
client.dll+746345 - 41 8B F8              - mov edi,r8d
client.dll+746348 - 41 F7 E8              - imul r8d
client.dll+74634B - 49 69 CA 50430000     - imul rcx,r10,00004350 { 17232 }
client.dll+746352 - C1 FA 04              - sar edx,04 { 4 }
client.dll+746355 - 8B C2                 - mov eax,edx
client.dll+746357 - C1 E8 1F              - shr eax,1F { 31 }
client.dll+74635A - 03 D0                 - add edx,eax
client.dll+74635C - 69 C2 96000000        - imul eax,edx,00000096 { 150 }
*/
//CreateMove内第4个call 获取usercmd
#define OFFSET_GETUSETCMD "CC CC 48 89 ?? ?? ?? 57 48 83 ?? ?? 4C 63 ?? 48 8B ?? B8"/* ?? ?? ?? ?? 41 8B F8 41 F7*/