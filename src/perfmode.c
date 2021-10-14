#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define __TURBO_MODE '1'
#define __BALANCED_MODE '0'
#define __SILENT_MODE '2'

static const char* _POLICY_FILE =
    "/sys/devices/platform/asus-nb-wmi/throttle_thermal_policy";

__attribute__((warn_unused_result)) static uint8_t check_module_loaded()
{
    /* if output, module is loaded, proceed successfully otherwise exit*/
    FILE* module_fp = popen("lsmod | grep asus_nb_wmi", "r");

    char outbuf[25];
    if (fread(outbuf, 1, sizeof(outbuf), module_fp) > 0) {
        return 0;
    } else {
        puts("Perfmode_E : asus_nb_wmi module not loaded!");
        return 1;
    }
}

static void check_permissions()
{
    int perm_val = access(_POLICY_FILE, W_OK);
    if (perm_val == -1) {
        perror("Perfmode_E ");
        exit(1);
    }
}

static void print_help()
{
    puts("Perfmode - Manage performance mode of your asus laptop\n"
         "Usage:\n"

         "\tperfmode -op|--option\n"

         "\nNote: Elevated permissions are required for perfmode to work i.e "
         "sudo\n"

         "Options:\n"

         "\t--turbo         Turbo Mode\n"
         "\t -t\n\n"

         "\t--balanced      Balanced Mode\n"
         "\t -b\n\n"

         "\t--silent        Silent Mode\n"
         "\t -s\n\n"

         "\t--help          Display this help menu\n"
         "\t -h\n\n");
}

static void write_to_policy(int mode)
{
    FILE* fp = fopen(_POLICY_FILE, "w");
    if (fp == NULL) {
        puts("Perfmode_E : Could not open Policy file");
        exit(1);
    }

    int ch;
    if ((ch = fputc(mode, fp)) != mode) {
        puts("Perfmode_E : Could not write to Policy file");
    } else {
        puts("Perfmode : Mode set successfully");
    }
}

__attribute__((warn_unused_result)) static uint8_t
parse_flags(const char* argv[])
{
    /* Check for help */
    if ((strncmp(argv[1], "--help", strlen(argv[1])) == 0) ||
        (strncmp(argv[1], "-h", strlen(argv[1])) == 0)) {
        return 0;
    }

    /* Set turbo mode */
    if ((strncmp(argv[1], "--turbo", strlen(argv[1])) == 0) ||
        (strncmp(argv[1], "-t", strlen(argv[1])) == 0)) {
        return 1;
    }

    /* Set balanced bits in flag */
    if ((strncmp(argv[1], "--balanced", strlen(argv[1])) == 0) ||
        (strncmp(argv[1], "-b", strlen(argv[1])) == 0)) {
        return 2;
    }

    /* Set silent bits in flag */
    if ((strncmp(argv[1], "--silent", strlen(argv[1])) == 0) ||
        (strncmp(argv[1], "-s", strlen(argv[1])) == 0)) {
        return 3;
    }

    return -1;
}

int main(int argc, const char* argv[])
{
    /* Error checking */
    /* show help if too few or too many arguments */
    if (argc < 2 || argc > 2) {
        print_help();
        return 0;
    }

    /* Check if asus_nb_wmi module is loaded */
    uint8_t rval = check_module_loaded();
    if (rval) {
        return 0;
    }

    /* Check if we have enough permissions to write to policy file */
    check_permissions();

    uint8_t mode = parse_flags(argv);
    switch (mode) {

    case 0: {
        print_help();
        break;
    }
    case 1: {
        write_to_policy(__TURBO_MODE);
        break;
    }
    case 2: {
        write_to_policy(__BALANCED_MODE);
        break;
    }
    case 3: {
        write_to_policy(__SILENT_MODE);
        break;
    }
    default: {
        puts("Perfmode_E : Invalid arguments passed");
        break;
    }
    }
    return 0;
}