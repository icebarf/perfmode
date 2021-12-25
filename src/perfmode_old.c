#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>

/* bools */
enum bool {
    FALSE,
    TRUE
};

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
    FAUS_POL_2 = 2,
};

/* enum to file locations */
enum file_locations {
    APOL_FILE,
    FPOL_FILE,
    FPOL2_FILE,
    ALED_FILE,
    FLED_FILE
};

/* Different policy files available under different kernel modules */
const char* (file_list[5]) = {
    [APOL_FILE] = "/sys/devices/platform/asus-nb-wmi/throttle_thermal_policy",
    //[ALED_FILE] = "/sys/devices/platform/asus-nb-wmi/leds/asus::kbd_backlight/brightness",
    [FPOL_FILE] = "/sys/devices/platform/faustus/fan_boost_mode",
    [FPOL2_FILE] = "/sys/devices/platform/faustus/throttle_thermal_policy",
    [ALED_FILE] = "/sys/class/leds/asus::kbd_backlight/brightness",
    [FLED_FILE] = "/sys/class/leds/asus::kbd_backlight/brightness"
};
   
/* Global flag like array for policy files available */
enum FILE_FL_ENUM {
    _APOL,
    _FPOL,
    _FPOL2,
    _ALED,
    _FLED,

    FILE_FL_C = 5
};

uint8_t FILES_FL[5] = {1};

/* Error reporting */
enum errors {
    modules_not_loaded,
    module_files_not_available,
    no_permission,
    bad_argv,
    invalid_pol_file
};

static void
report_err(enum errors errors)
{
    switch(errors)
    {
        case modules_not_loaded:
            puts("Perfmode: Kernel modules are not available!\n"
                    "Please visit https://github.com/icebarf/perfmode#troubleshooting - and match the error message there");
            exit(1);

        case module_files_not_available:
            puts("Perfmode: Module files not available!\n"
                    "Please visit https://github.com/icebarf/perfmode#troubleshooting - and match the error message there");
            exit(1);

        case no_permission:
            puts("Perfmode: Insufficient permissions!\n"
                        "Run perfmode with superuser permissions - sudo or doas");
            exit(1);

        case bad_argv:
            puts("Perfmode: Invalid arguments passed\n"
                    "Run perfmode -h for usage!");
            exit(1);

        case invalid_pol_file:
            puts("Perfmode: Invalid policy file integer\n"
                    "This is a bug in code! Please wait for it to be fixed\n"
                    "Report bugs at https://github.com/icebarf/perfmode/issues");
            exit(1);

        default:
            puts("Perfmode: Invalid Error");
            exit(1);
    }
}

/* Check for modules */
static uint8_t
check_module_loaded(void)
{

    uint8_t bad_retval = TRUE;

    /* check for asus_nb_wmi */
    FILE* a_module_fp = popen("lsmod | grep asus_nb_wmi", "r");
    char outbuf[25];
    if (fread(outbuf, 1, sizeof(outbuf), a_module_fp) > 0) {
        bad_retval = FALSE;
        return bad_retval;
    }

    /* check for faustus */
    FILE* f_module_fp = popen("lsmod | grep faustus", "r");
    char f_outbuf[25];
    if (fread(f_outbuf, 1, sizeof(f_outbuf), f_module_fp) > 0) {
        bad_retval = FALSE;
        return bad_retval;
    }

    return bad_retval;
}

static void
check_policies(uint8_t mode)
{
    /* Try to access policy files and we can read from / write to it*/

    switch(mode)
    {
        case 1:
        case 2:
        case 3:
            for (size_t i = 0; i < (FILE_FL_C - 2); i++)
            {
                FILES_FL[i] = access(file_list[i], F_OK);
            }
            break;

        case LED_OFF:
        case LED_MIN:
        case LED_MED:
        case LED_MAX:
            FILES_FL[ALED_FILE] = access(file_list[ALED_FILE], F_OK);
            FILES_FL[FLED_FILE] = access(file_list[FLED_FILE], F_OK);
            break;
    }

    /* Access returns 0 on success which is why
     * we check if FILES_FL array is still set to TRUE or not */
    uint8_t exists = FALSE;

    if(FILES_FL[APOL_FILE] == 1 && FILES_FL[FPOL_FILE] == 1
            && FILES_FL[FPOL2_FILE] == 1 && FILES_FL[ALED_FILE] == 1
            && FILES_FL[FLED_FILE] == 1)
    {
        report_err(module_files_not_available);
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

    return 255;
}

static void write_to_fan_policy(uint8_t pol_file, uint8_t mode)
{
    FILE* fp = NULL;

    switch (pol_file)
    {
    case _APOL:
        fp = fopen(file_list[APOL_FILE], "w");
        break;
    
    case _FPOL:
        fp = fopen(file_list[FPOL_FILE], "w");
        break;
    
    case _FPOL2:
        fp = fopen(file_list[FPOL2_FILE], "w");
        break;

    default:
        report_err(invalid_pol_file);
    }

    if (fp == NULL) {
        report_err(no_permission);
    }

    int ch;
    if ((ch = fputc(mode, fp)) != mode) {
        fclose(fp);
        report_err(no_permission);

    } else 
    {
        fclose(fp);
        puts("Perfmode: Fan Mode set");
    }
    
}

static void write_to_led_policy(uint8_t pol_file, uint8_t mode)
{
    FILE* fp = NULL;
    
    switch(pol_file)
    {
        case _ALED:
            fp = fopen(file_list[ALED_FILE], "w");
            break;

        case _FLED:
            fp = fopen(file_list[FLED_FILE], "w");
            break;

        default:
            report_err(invalid_pol_file);
    }

    if (fp == NULL) {
        report_err(no_permission);
    }

    int ch;
    if ((ch = fputc(mode, fp)) != mode) {
        fclose(fp);
        report_err(no_permission);
    } else {
        fclose(fp);
        puts("Perfmode: Keyboard Backlight level set");
    }
}

static uint8_t
set_pol_file (uint8_t mode)
{
    /* Check which policy files can be used in the 
     * global FILES_FL - Files Flag array */
    uint8_t pol_file = 255;

    switch(mode)
    {
        case 1:
        case 2:
        case 3:
            for(size_t i = 0; i < (FILE_FL_C - 2); i++)
            {
               if(FILES_FL[i])
                  pol_file = i; 
            }
            break;

        case LED_OFF:
        case LED_MIN:
        case LED_MED:
        case LED_MAX:
            if(FILES_FL[ALED_FILE])
                pol_file = ALED_FILE;
            else if (FILES_FL[FLED_FILE])
                pol_file = FLED_FILE;
            break;
    }
    return pol_file;
}

int main(int argc, char* argv[])
{
    if(argc < 2){print_help(); return 0;}

    /* Check if modules are loaded */
    uint8_t bad_retval = check_module_loaded();

    if (bad_retval == TRUE) {
        report_err(modules_not_loaded);
    }

    /* Check flags and run */
    uint8_t mode = parse_flags(argc, argv);

    /* Check for existence of policy files and if can write to policy file */
    check_policies(mode);

    /* Set pol file */ 
    uint8_t pol_file = set_pol_file(mode);

    switch (mode) {

        case 0:
            print_help();
            break;

        case 1:
            write_to_fan_policy(pol_file, TURBO_MODE);
            break;

        case 2:
            write_to_fan_policy(pol_file, BALANCED_MODE);
            break;

        case 3:
            write_to_fan_policy(pol_file, SILENT_MODE);
            break;

        case LED_OFF:
            write_to_led_policy(pol_file, LED_OFF);
            break;

        case LED_MIN:
            write_to_led_policy(pol_file, LED_MIN);
            break;

        case LED_MED:
            write_to_led_policy(pol_file, LED_MED);
            break;

        case LED_MAX: 
            write_to_led_policy(pol_file, LED_MAX);
            break;

        default:
            report_err(bad_argv);
    }
    return 0;
}
