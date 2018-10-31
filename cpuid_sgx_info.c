/*** SGX capabilities checker
 *
 * - Checks if SGX is available and enabled 
 * - Prints SGX capabilities based on the CPUID 
 * 
 * Anjuna security, 2018
 */

#include <stdio.h>

enum sgx_status {
    ENABLED = 0,
    DISABLED_IN_BIOS,
    NOT_AVAILABLE_ON_CPU,
}

static inline void native_cpuid(unsigned int *eax, unsigned int *ebx,
                                unsigned int *ecx, unsigned int *edx)
{
    /* Input: EAX = leaf, ECX = sub-leaf */
    asm volatile("cpuid" 
        : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
        : "0" (*eax), "2" (*ecx) );
    
    /* Uncomment for more verbosity or debug */
    // printf("eax: 0x%x ebx: 0x%x ecx: 0x%x edx: 0x%x\n", *eax, *ebx, *ecx, *edx);
}

int main() {
    unsigned int eax = 0, ebx = 0, ecx = 0, edx = 0;
    
    printf("\nCPUID Leaf 07H, extended feature bits (EAX=07H, ECX=0H)\n");
    eax = 7;
    ecx = 0;
    native_cpuid(&eax, &ebx, &ecx, &edx);

    if (0 == ((ebx >> 2) & 0x1)) {
        printf("SGX is not supported on this CPU\n");
        return DISABLED_IN_BIOS;
    }
    
    printf("SGX supported: 1\n");
    printf("\nCPUID Leaf 12H, Sub-Leaf 0 of Intel SGX Capabilities (EAX=12H, ECX=0)\n");
    eax = 0x12;
    ecx = 0;
    native_cpuid(&eax, &ebx, &ecx, &edx);

    if ((0 == eax) && (0 == ebx) && (0 == ecx) && (0 == edx)){
        printf("SGX is disabled in BIOS\n");
        return NOT_AVAILABLE_ON_CPU;
    }

    printf("SGX1 supported: %d\n", eax & 0x1);
    printf("SGX2 supported: %d\n", (eax >> 1) & 0x1);
    printf("SGX ENCLV instruction supported: %d\n", (eax >> 5) & 0x1);
    printf("SGX ENCLS instruction supported: %d\n", (eax >> 6) & 0x1);
    printf("MISCSELECT.EXINFO supported: %d\n", ebx & 0x1);
    printf("MaxEnclaveSize_Not64: 0x%x\n", edx & 0xFF);
    printf("MaxEnclaveSize_64: 0x%x\n", (edx >> 8) & 0xFF);

    printf("\nCPUID Leaf 12H, Sub-Leaf 1 of Intel SGX Capabilities (EAX=12H,ECX=1)\n");
    eax = 0x12;
    ecx = 1;
    native_cpuid(&eax, &ebx, &ecx, &edx);
    printf("SECS.ATTRIBUTES valid bit fields: 0x%08x%08x%08x%08x\n", edx, ecx, ebx, eax);

    int i = 2;
    int sub_leaf_valid = 1;
    while(sub_leaf_valid && (i < 32)){
        printf("\nCPUID Leaf 12H, Sub-Leaf %d of Intel SGX Capabilities (EAX=12H,ECX=%d)\n", i, i);
        eax = 0x12;
        ecx = i++;
        native_cpuid(&eax, &ebx, &ecx, &edx);
        switch (eax & 0xF) {
            case 0:
                printf("Invalid sub-leaf\n");
                sub_leaf_valid = 0;
                break;
            case 1:
                printf("SGX EPC section base 0x%08x%08x\n", ebx & 0xFFFFF, eax & 0xFFFFF000);
                printf("SGX EPC section size 0x%08x%08x\n", edx & 0xFFFFF, ecx & 0xFFFFF000);
                break;
            default:
                printf("Unknown type of SGX sub-leaf\n");
                sub_leaf_valid = 0;
                break;
        }
    } 
    
    return ENABLED;
}
