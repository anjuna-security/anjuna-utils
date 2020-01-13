/*** SGX capabilities checker
 *
 * - Checks if SGX is available and enabled 
 * - Prints SGX capabilities based on the CPUID 
 * 
 * Anjuna security, 2020
 */

#include <stdio.h>

enum sgx_status {
    ENABLED = 0,
    NOT_AVAILABLE_ON_CPU,
    DISABLED_IN_BIOS,
};

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
    
    printf("CPUID Leaf 07H, extended feature bits (EAX=07H, ECX=0H)\n");
    eax = 7;
    ecx = 0;
    native_cpuid(&eax, &ebx, &ecx, &edx);

    if (0 == ((ebx >> 2) & 0x1)) {
        printf("SGX is not supported on this CPU\n");
        return NOT_AVAILABLE_ON_CPU;
    }
    
    printf("SGX is supported on this CPU\n");
    printf("Flexible Launch Control supported: %d \n", ((ecx >> 30) & 0x1));
    
    printf("\nCPUID Leaf 12H, Sub-Leaf 0 of Intel SGX Capabilities (EAX=12H, ECX=0)\n");
    eax = 0x12;
    ecx = 0;
    native_cpuid(&eax, &ebx, &ecx, &edx);

    if ((0 == eax) && (0 == ebx) && (0 == ecx) && (0 == edx)){
        printf("SGX is disabled in BIOS\n");
        return DISABLED_IN_BIOS;
    }

    printf("SGX1 supported: %d\n", eax & 0x1);
    printf("SGX2 supported: %d\n", (eax >> 1) & 0x1);
    printf("SGX ENCLV instruction supported: %d\n", (eax >> 5) & 0x1);
    printf("SGX ENCLS instruction supported: %d\n", (eax >> 6) & 0x1);
    printf("MISCSELECT.EXINFO supported: %d\n", ebx & 0x1);
    unsigned int size_power = edx & 0xFF;
    printf("MaxEnclaveSize in 32-bit mode: 0x%X (%lu MB)\n", size_power, 1UL << (size_power - 20));
    size_power = (edx >> 8) & 0xFF;
    printf("MaxEnclaveSize in 64-bit mode: 0x%X (%lu MB)\n", size_power, 1UL << (size_power - 20));

    printf("\nCPUID Leaf 12H, Sub-Leaf 1 of Intel SGX Capabilities (EAX=12H,ECX=1)\n");
    eax = 0x12;
    ecx = 1;
    native_cpuid(&eax, &ebx, &ecx, &edx);
    printf("SECS.ATTRIBUTES valid bit fields: 0x%08X%08X%08X%08X\n", edx, ecx, ebx, eax);

    
    int sub_leaf_valid = 1;
    size_t total_epc_mem = 0;
    size_t section_base = 0;
    size_t section_size = 0;
    for (int i = 2; sub_leaf_valid && (i < 32); i++){
        printf("\nCPUID Leaf 12H, Sub-Leaf %d of Intel SGX Capabilities (EAX=12H,ECX=%d)\n", i, i);
        eax = 0x12;
        ecx = i;
        native_cpuid(&eax, &ebx, &ecx, &edx);
        switch (eax & 0xF) {
            case 0:
                printf("Total EPC memory: %lu MB\n", total_epc_mem >> 20);
                sub_leaf_valid = 0;
                break;
            case 1: // the only valid type, is "Section has confidentiality and integrity protection"
                section_base = ((ebx & 0xFFFFFUL) << 32) + (eax & 0xFFFFF000);
                section_size = ((edx & 0xFFFFFUL) << 32) + (ecx & 0xFFFFF000);
                printf("SGX EPC section base %16p \n", (void *)section_base);
                printf("SGX EPC section size %16p (%lu MB)\n", (void *)section_size, section_size >> 20);
                total_epc_mem += section_size;
                break;
            default:
                printf("Unknown type of SGX sub-leaf\n");
                sub_leaf_valid = 0;
                break;
        }
    } 
    
    return ENABLED;
}
