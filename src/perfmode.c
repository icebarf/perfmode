#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* modes */
enum modes {
    /* Fans */
    TURBO_MODE = '1',
    BALANCED_MODE = '0',
    SILENT_MODE = '2',

    /* LED Backlight */
    LED_OFF = '0',
    LED_MIN = '1',
    LED_MED = '2',
    LED_MAX = '3'
};

/* file integers */
enum files {
    ASUS_POL = 0,
    FAUS_POL = 1,
    FAUS_POL_2 = 2
};

/* enum to file locations */
enum file_locations {
    APOL_FILE,
    ALED_FILE,
    FPOL_FILE,
    FPOL2_FILE
};

/* Different policy files available under different kernel modules */
const char* (file_list[4]) = {
    [APOL_FILE] = "/sys/devices/platform/asus-nb-wmi/throttle_thermal_policy",
    [ALED_FILE] = "/sys/devices/platform/asus-nb-wmi/leds/asus::kbd_backlight/brightness",
    [FPOL_FILE] = "/sys/devices/platform/faustus/fan_boost_mode",
    [FPOL2_FILE] = "/sys/devices/platform/faustus/throttle_thermal_policy"
};
   
/* Global flag like array for policy files available */
enum FILE_FL_ENUM {
    _APOL,
    _FPOL,
    _FPOL2,
    _ALED
};

uint8_t FILES_FL[4];

/* Check for modules */
static uint8_t check_module_loaded()
{

    uint8_t retval = -1;

    /* check for asus_nb_wmi */
    FILE* a_module_fp = popen("lsmod | grep asus_nb_wmi", "r");
    char outbuf[25];
    if (fread(outbuf, 1, sizeof(outbuf), a_module_fp) > 0) {
        retval = 0;
        return retval;
    }

    /* check for faustus */
    FILE* f_module_fp = popen("lsmod | grep faustus", "r");
    char f_outbuf[25];
    if (fread(f_outbuf, 1, sizeof(f_outbuf), f_module_fp) > 0) {
        retval = 0;
        return retval;
    }

    return retval;
}

static void check_policies()
{
    int a_pol_exists = -1;
    int f_pol_exists = -1;
    int f_pol_exists_2 = -1;
    int a_led_exists = -1;

    /* Check if policy file exists */
    a_pol_exists = access(file_list[APOL_FILE], F_OK);
    f_pol_exists = access(file_list[FPOL_FILE], F_OK);
    f_pol_exists_2 = access(file_list[FPOL2_FILE], F_OK);
    a_led_exists = access(file_list[ALED_FILE], F_OK);

    /* Return value of access is zero if there is no error, otherwise -1 */
    /* Sets the global FILE array - makes the existence of policy files known
     * to the program */

    if (a_pol_exists == 0) {
        FILES_FL[_APOL] = 1;
    } else if (f_pol_exists == 0) {
        FILES_FL[_FPOL] = 1;
    } else if (f_pol_exists_2 == 0) {
        FILES_FL[_FPOL2] = 1;
    } else if (a_led_exists == 0) {
        FILES_FL[_ALED] = 1;
    }

    /* If not files are found - print error and exit */
    if (a_pol_exists == -1 && f_pol_exists == -1 &&
            f_pol_exists_2 == -1 && a_led_exists) {
        puts("Perfmode: Not enugh permissions");
        exit(1);
    }
}

static void print_help()
{
    puts("Perfmode - Manage performance mode of your asus laptop\n"
         "Usage:\n"

         "\tperfmode -op|--option\n"
         "\nor\n\tperfmode\n\n"
         "Without arguments, A GUI is launched\n"

         "Note: Elevated permissions are required for perfmode to work i.e "
         "sudo\n\n"

         "Options:\n"

         "\nFan Control:\n"

         "\t--turbo         Turbo Mode\n"
         "\t -t\n\n"

         "\t--balanced      Balanced Mode\n"
         "\t -b\n\n"

         "\t--silent        Silent Mode\n"
         "\t -s\n\n"

         "Keyboard Backlight:\n"

         "\t-l off           Turn off Backlight\n"
         "\t-l min           Minimum Backlight\n"
         "\t-l med           Medium  Backlight\n"
         "\t-l max           Maximum Backlight\n\n"

         "Help:\n"

         "\t--help          Display this help menu\n"
         "\t -h\n"
         "Visit github for more info or updates: https://github.com/icebarf/perfmode");
}

static uint8_t parse_flags(int argc, char* argv[])
{
    /* Check for help */
    if ((strncmp(argv[1], "--help", strlen(argv[1])) == 0) ||
        (strncmp(argv[1], "-h", strlen(argv[1])) == 0)) {
        return 0;
    }

    /* Check for turbo mode */
    if ((strncmp(argv[1], "--turbo", strlen(argv[1])) == 0) ||
        (strncmp(argv[1], "-t", strlen(argv[1])) == 0)) {
        return 1;
    }

    /* Check for balanced mode */
    if ((strncmp(argv[1], "--balanced", strlen(argv[1])) == 0) ||
        (strncmp(argv[1], "-b", strlen(argv[1])) == 0)) {
        return 2;
    }

    /* Check silent mode */
    if ((strncmp(argv[1], "--silent", strlen(argv[1])) == 0) ||
        (strncmp(argv[1], "-s", strlen(argv[1])) == 0)) {
        return 3;
    }

    /* Check for Keyboard Backlight arguments */
    if (strncmp(argv[1], "-l", strlen(argv[1])) == 0 && (argc == 3)) {
        /* backlight off */
        if (strncmp(argv[2], "off", strlen(argv[2])) == 0) {
            return LED_OFF;
        }

        /* backlight min */
        if (strncmp(argv[2], "min", strlen(argv[2])) == 0) {
            return LED_MIN;
        }

        /* backlight med */
        if (strncmp(argv[2], "med", strlen(argv[2])) == 0) {
            return LED_MED;
        }

        /* backlight max */
        if (strncmp(argv[2], "max", strlen(argv[2])) == 0) {
            return LED_MAX;
        }
    }

    return -1;
}

static void write_to_policy(uint8_t pol_file, uint8_t mode)
{
    FILE* fp;

    switch (pol_file) {
    case 0: {
        fp = fopen(file_list[APOL_FILE], "w");
        break;
    }
    case 1: {
        fp = fopen(file_list[FPOL_FILE], "w");
        break;
    }
    case 2: {
        fp = fopen(file_list[FPOL2_FILE], "w");
        break;
    }
    default: {
        fp = NULL;
    }
    }

    if (fp == NULL) {
        puts("Perfmode: Could not open Policy file - insufficient permissions!");
        exit(1);
    }

    int ch;
    if ((ch = fputc(mode, fp)) != mode) {

        puts("Perfmode: Could not write to Policy file - insufficient permissions!");

        fclose(fp);
        exit(1);

    } else 
    {
        puts("Perfmode: Fan Mode set");
    }
    
    fclose(fp);
}

static void handle_led(uint8_t pol_file, uint8_t mode)
{
    FILE* fp = fopen(file_list[ALED_FILE], "w");
    if (fp == NULL) {
        puts("Perfmode: Could not open Policy file - insufficient permissions!");
    }

    int ch;
    if ((ch = fputc(mode, fp)) != mode) {
        puts("Perfmode: Could not write to Policy file - insufficient permissions!");
    } else {
        puts("Perfmode: Keyboard Backlight level set");
    }
    fclose(fp);
}

int main(int argc, char* argv[])
{
    if(argc < 2){print_help(); return 0;}

    /* Check if modules are loaded */
    uint8_t retval = check_module_loaded();
    if (retval) {
        puts("Perfmode: Kernel modules not loaded!\n\
Please visit https://github.com/icebarf/perfmode/#troubleshooting for more information!");
        return 0;
    }

    /* Check for existence of policy files and if can write to policy file */
    check_policies();

    uint8_t mode = parse_flags(argc, argv);
    uint8_t pol_file = -1;

    if (FILES_FL[_APOL] || FILES_FL[_ALED]) {
        pol_file = 0;
    }
    if (FILES_FL[_FPOL]) {
        pol_file = 1;
    }
    if (FILES_FL[_FPOL2]) {
        pol_file = 2;
    }

    switch (mode) {

    case 0: {
        print_help();
        break;
    }
    case 1: {
        write_to_policy(pol_file, TURBO_MODE);
        break;
    }
    case 2: {
        write_to_policy(pol_file, BALANCED_MODE);
        break;
    }
    case 3: {
        write_to_policy(pol_file, SILENT_MODE);
        break;
    }
    case LED_OFF: {
        handle_led(pol_file, LED_OFF);
        break;
    }
    case LED_MIN: {
        handle_led(pol_file, LED_MIN);
        break;
    }
    case LED_MED: {
        handle_led(pol_file, LED_MED);
        break;
    }
    case LED_MAX: {
        handle_led(pol_file, LED_MAX);
        break;
    }

    default: {
        puts("Perfmode: Invalid arguments passed");
    }
    }
    return 0;
}
