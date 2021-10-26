#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* modes */
#define __TURBO_MODE '1'
#define __BALANCED_MODE '0'
#define __SILENT_MODE '2'

/* files */
#define __ASUS_POL 0
#define __FAUS_POL 1
#define __FAUS_POL_2 2

/* Different policy files available under different kernel modules */
static const char* _APOLICY_FILE =
    "/sys/devices/platform/asus-nb-wmi/throttle_thermal_policy";
static const char* _FPOLICY_FILE =
    "/sys/devices/platform/faustus/fan_boost_mode";
static const char* _FPOLICY_FILE_2 =
    "/sys/devices/platform/faustus/throttle_thermal_policy";

/* Global variables for different policy file being available */
uint8_t _APOL, _FPOL, _FPOL2;

/* Check for modules */
static uint8_t check_module_loaded()
{
    
    uint8_t retval = -1;

    /* check for asus_nb_wmi */
    FILE* a_module_fp = popen("lsmod | grep asus_nb_wmi", "r");
    char outbuf[25];
    if (fread(outbuf, 1, sizeof(outbuf), a_module_fp) > 0) {
        retval = 0;
    }

    /* check for faustus */
    FILE* f_module_fp = popen("lsmod | grep faustus", "r");
    char f_outbuf[25];
    if (fread(f_outbuf, 1, sizeof(f_outbuf), f_module_fp) > 0) {
        retval = 0;
    }

    return retval;
}

static void check_policies()
{
    int a_pol_exists = -1;
    int f_pol_exists = -1;
    int f_pol_exists_2 = -1;

    /* asus_nb_wmi */
    /* Check if policy file exists */
    a_pol_exists = access(_APOLICY_FILE, F_OK);
    f_pol_exists = access(_FPOLICY_FILE, F_OK);
    f_pol_exists_2 = access(_FPOLICY_FILE_2, F_OK);

    /* Return value of access is zero
     * if there is no error otherwise -1 */

    if (a_pol_exists == 0) {
        _APOL = 1;
    } else if (f_pol_exists == 0) {
        _FPOL = 1;
    } else if (f_pol_exists_2 == 0) {
        _FPOL2 = 1;
    }

    if (a_pol_exists == -1 && f_pol_exists == -1 && f_pol_exists_2 == -1) {
        puts("Perfmode: Not enugh permissions");
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
         "\t -h");
}

static uint8_t parse_flags(const char* argv[])
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

static void write_to_policy(uint8_t pol_file, uint8_t mode)
{
    FILE* fp;

    switch (pol_file) {
    case 0: {
        fp = fopen(_APOLICY_FILE, "w");
        break;
    }
    case 1: {
        fp = fopen(_FPOLICY_FILE, "w");
        break;
    }
    case 2: {
        fp = fopen(_FPOLICY_FILE_2, "w");
        break;
    }
    default: {
        fp = NULL;
    }
    }

    if (fp == NULL) {
        puts("Perfmode: Could not open Policy file");
        exit(1);
    }

    int ch;
    if ((ch = fputc(mode, fp)) != mode) {
        puts("Perfmode: Could not write to Policy file");
    } else {
        puts("Perfmode : Mode set successfully");
    }
}

int main(int argc, const char* argv[])
{
    /* Error checking */
    /* show help if too few or too many arguments */
    if (argc < 2 || argc > 2) {
        print_help();
        return 0;
    }

    /* Check if modules are loaded */
    uint8_t retval = check_module_loaded();
    if (retval) {
        puts("Perfmode: asus_nb_wmi or faustus module not loaded!");
        return 0;
    }

    /* Check for existence of policy files and if can write to policy file */
    check_policies();

    uint8_t mode = parse_flags(argv);
    uint8_t pol_file = -1;

    if (_APOL) {
        pol_file = 0;
    }
    if (_FPOL) {
        pol_file = 1;
    }
    if (_FPOL2) {
        pol_file = 2;
    }

    switch (mode) {

    case 0: {
        print_help();
        break;
    }
    case 1: {
        write_to_policy(pol_file, __TURBO_MODE);
        break;
    }
    case 2: {
        write_to_policy(pol_file, __BALANCED_MODE);
        break;
    }
    case 3: {
        write_to_policy(pol_file, __SILENT_MODE);
        break;
    }
    default: {
        puts("Perfmode: Invalid arguments passed");
    }
    }
    return 0;
}