#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Local files */
#include "gui.h"
#include "perfmode.h"

/* modes */
#define __TURBO_MODE '1'
#define __BALANCED_MODE '0'
#define __SILENT_MODE '2'

#define __LED_OFF '0'
#define __LED_MIN '1'
#define __LED_MED '2'
#define __LED_MAX '3'

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

/* Keyboard Backlight files - asus-nb-wmi */
static const char* _ALED_FILE =
    "/sys/devices/platform/asus-nb-wmi/leds/asus::kbd_backlight/brightness";

/* Global variables for different policy file being available */
uint8_t _APOL, _ALED, _FPOL, _FPOL2;

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
    int a_led_exists = -1;

    /* Check if policy file exists */
    a_pol_exists = access(_APOLICY_FILE, F_OK);
    f_pol_exists = access(_FPOLICY_FILE, F_OK);
    f_pol_exists_2 = access(_FPOLICY_FILE_2, F_OK);
    a_led_exists = access(_ALED_FILE, F_OK);

    /* Return value of access is zero
     * if there is no error otherwise -1 */

    if (a_pol_exists == 0) {
        _APOL = 1;
    } else if (f_pol_exists == 0) {
        _FPOL = 1;
    } else if (f_pol_exists_2 == 0) {
        _FPOL2 = 1;
    } else if (a_led_exists == 0) {
        _ALED = 1;
    }

    if (a_pol_exists == -1 && f_pol_exists == -1 && f_pol_exists_2 == -1 &&
        a_led_exists) {
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

         "\nNote: Elevated permissions are required for perfmode to work i.e "
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
         "\t -h");
}

static uint8_t parse_flags(char* argv[])
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
    if (strncmp(argv[1], "-l", strlen(argv[1])) == 0) {
        /* backlight off */
        if (strncmp(argv[2], "off", strlen(argv[2])) == 0) {
            return __LED_OFF;
        }

        /* backlight min */
        if (strncmp(argv[2], "min", strlen(argv[2])) == 0) {
            return __LED_MIN;
        }

        /* backlight med */
        if (strncmp(argv[2], "med", strlen(argv[2])) == 0) {
            return __LED_MED;
        }

        /* backlight max */
        if (strncmp(argv[2], "max", strlen(argv[2])) == 0) {
            return __LED_MAX;
        }
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
        puts("Perfmode: Fan Mode set");
    }
    fclose(fp);
}

static void handle_led(uint8_t pol_file, uint8_t mode)
{
    FILE* fp = fopen(_ALED_FILE, "w");
    if (fp == NULL) {
        puts("Perfmode: Could not open Policy file");
    }

    int ch;
    if ((ch = fputc(mode, fp)) != mode) {
        puts("Perfmode: Could not write to Policy file");
    } else {
        puts("Perfmode: Keyboard Backlight level set");
    }
    fclose(fp);
}

int main(int argc, char* argv[])
{
    /* Error checking */
    /* show help if too few or too many arguments */
    if (argc < 2) {
        /* necessary check for policy files */
        check_policies();

        /* gtk apps */
        GtkApplication* app;
        int status;

        app = gtk_application_new("org.freedesktop.perfmode",
                                  G_APPLICATION_FLAGS_NONE);

        g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
        status = g_application_run(G_APPLICATION(app), argc, argv);
        g_object_unref(app);

        return status;
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

    if (_APOL || _ALED) {
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
    case __LED_OFF: {
        handle_led(pol_file, __LED_OFF);
        break;
    }
    case __LED_MIN: {
        handle_led(pol_file, __LED_MIN);
        break;
    }
    case __LED_MED: {
        handle_led(pol_file, __LED_MED);
        break;
    }
    case __LED_MAX: {
        handle_led(pol_file, __LED_MAX);
        break;
    }

    default: {
        puts("Perfmode: Invalid arguments passed");
    }
    }
    return 0;
}

/* Functions for interfacing with the gui */

/* Keyboard Backlighting */

void led_off(void)
{
    uint8_t pol_file = -1;

    if (_APOL || _ALED) {
        pol_file = 0;
    }
    if (_FPOL) {
        pol_file = 1;
    }
    if (_FPOL2) {
        pol_file = 2;
    }

    handle_led(pol_file, __LED_OFF);
}

void led_min(void)
{
    uint8_t pol_file = -1;

    if (_APOL || _ALED) {
        pol_file = 0;
    }
    if (_FPOL) {
        pol_file = 1;
    }
    if (_FPOL2) {
        pol_file = 2;
    }

    handle_led(pol_file, __LED_MIN);
}

void led_med(void)
{
    uint8_t pol_file = -1;

    if (_APOL || _ALED) {
        pol_file = 0;
    }
    if (_FPOL) {
        pol_file = 1;
    }
    if (_FPOL2) {
        pol_file = 2;
    }

    handle_led(pol_file, __LED_MED);
}

void led_max(void)
{
    uint8_t pol_file = -1;

    if (_APOL || _ALED) {
        pol_file = 0;
    }
    if (_FPOL) {
        pol_file = 1;
    }
    if (_FPOL2) {
        pol_file = 2;
    }

    handle_led(pol_file, __LED_MAX);
}

/* fan control */

void fan_silent(void)
{
    uint8_t pol_file = -1;

    if (_APOL || _ALED) {
        pol_file = 0;
    }
    if (_FPOL) {
        pol_file = 1;
    }
    if (_FPOL2) {
        pol_file = 2;
    }

    write_to_policy(pol_file, __SILENT_MODE);
}

void fan_balanced(void)
{
    uint8_t pol_file = -1;

    if (_APOL || _ALED) {
        pol_file = 0;
    }
    if (_FPOL) {
        pol_file = 1;
    }
    if (_FPOL2) {
        pol_file = 2;
    }

    write_to_policy(pol_file, __BALANCED_MODE);
}
void fan_turbo(void)
{
    uint8_t pol_file = -1;

    if (_APOL || _ALED) {
        pol_file = 0;
    }
    if (_FPOL) {
        pol_file = 1;
    }
    if (_FPOL2) {
        pol_file = 2;
    }

    write_to_policy(pol_file, __TURBO_MODE);
}