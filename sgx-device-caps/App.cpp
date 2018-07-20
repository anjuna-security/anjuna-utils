#include "sgx_capable.h"
#include <stdio.h>

#define ANSI_RESET "\x1B[0m"
#define ANSI_RED "\x1B[31m"
#define ANSI_GREEN "\x1B[32m"
#define ANSI_YELLOW "\x1B[33m"
#define ANSI_BLUE "\x1B[34m"
#define ANSI_MAGENTA "\x1B[35m"
#define ANSI_CYAN "\x1B[36m"
#define ANSI_WHITE "\x1B[37m"

#define RED(x) ANSI_RED x ANSI_RESET
#define GREEN(x) ANSI_GREEN x ANSI_RESET
#define YELLOW(x) ANSI_YELLOW x ANSI_RESET
#define BLUE(x) ANSI_BLUE x ANSI_RESET
#define MAGENTA(x) ANSI_MAGENTA x ANSI_RESET
#define CYAN(x) ANSI_CYAN x ANSI_RESET
#define WHITE(x) ANSI_WHITE x ANSI_RESET

#define BOLD(x) "\x1B[1m" x ANSI_RESET
#define UNDL(x) "\x1B[4m" x ANSI_RESET

const char *get_device_status_desc(const sgx_device_status_t &status) {
    switch (status) {
        case SGX_ENABLED:
            return "SGX_ENABLED";
        case SGX_DISABLED_REBOOT_REQUIRED:
            return "SGX_DISABLED_REBOOT_REQUIRED: A reboot is required to finish "
                   "enabling SGX";
        case SGX_DISABLED_LEGACY_OS:
            return "SGX is disabled and a Software Control Interface is not available "
                   "to enable it";
        case SGX_DISABLED:
            return "SGX is not enabled on this platform. More details are unavailable";
        case SGX_DISABLED_SCI_AVAILABLE:
            return "SGX is disabled, but a Software Control Interface is available to "
                   "enable it";
        case SGX_DISABLED_MANUAL_ENABLE:
            return "SGX is disabled, but can be enabled manually in the BIOS setup";
        case SGX_DISABLED_HYPERV_ENABLED:
            return "Detected an unsupported version of Windows* 10 with Hyper-V "
                   "enabled";
        case SGX_DISABLED_UNSUPPORTED_CPU:
            return "SGX is not supported by this CPU";
        default:
            return "Unknown status";
    }
}

// Application entry
int SGX_CDECL main() {
    int capable = 0;
    sgx_status_t status = sgx_is_capable(&capable);
    if (status != SGX_SUCCESS) {
        printf(RED("Error calling sgx_is_capable => %d\n"), status);
    } else {
        if (capable) {
            printf(GREEN("This host is SGX capable (sgx_is_capable => %d)\n"), capable);
            // We are already configured with SGX
            return 0;
        } else {
            printf(RED("This host is not currently SGX capable (sgx_is_capable => %d)\n"),
            capable);
            printf("Trying to call sgx_enable_device\n");
            sgx_device_status_t device_status;
            status = sgx_cap_enable_device(&device_status);
            if (status != SGX_SUCCESS) {
                printf(RED("Error calling sgx_enable_device => %d\n"), status);
            } else {
                int enabled = (device_status == SGX_ENABLED) ? 1 : 0;
                const char *color = enabled ? ANSI_GREEN : ANSI_RED;
                printf("%ssgx_cap_enable_device is %s (capabilities=%d)\n", color,
                get_device_status_desc(device_status), device_status);
                printf("%sreturn value = %d", color, enabled);
                return !enabled;
            }
        }
    }
    // If we get here, there was an error
    return 1;
}
