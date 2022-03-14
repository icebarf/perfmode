#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum file_list_enum {
    LED_FILE,
    AFAN_FILE,
    AFAN_FILE_2,
    FFAN_FILE,
    FFAN_FILE_2
};

const char* file_list[5] = {
    [LED_FILE] = "/sys/class/leds/asus::kbd_backlight/brightness",
    [AFAN_FILE] = "/sys/devices/platform/asus-nb-wmi/throttle_thermal_policy",
    [AFAN_FILE_2] = "/sys/devices/platform/asus-nb-wmi/fan_boost_mode",
    [FFAN_FILE] = "/sys/devices/platform/faustus/throttle_thermal_policy",
    [FFAN_FILE_2] = "/sys/devices/platform/faustus/fan_boost_mode"};

enum operations {
    /* LED operations */
    OFF,
    MIN,
    MED,
    MAX,

    /* Fan Control operations */
    SILENT,
    BALANCED,
    TURBO,

    /* Help and others */
    led,
    fan,
    help,
    invalid_op
};

enum kmodules { asus_nb_wmi, faustus, nomodule };

enum kfiles {
    LED_MAIN,

    ASUS_FAN_MAIN,
    ASUS_FAN_OTHER,
    ASUS_FAN_BOTH,

    FAUSTUS_FAN_MAIN,
    FAUSTUS_FAN_OTHER,
    FAUSTUS_FAN_BOTH
};

enum lengths_expected {
    FAN_OR_LED = 4,

    SILENT_LN = 6,
    BALANCED_LN = 8,
    TURBO_LN = 5,

    LED_ARG_LN = 3,

    HELP_LN = 5
};

enum codes {
    SUCCESS,
    FAIL,

    /* errors */
    INVALID_ARGV,
    BAD_FP,
    NO_MODULE_FOUND,
    NO_PERMISSION,
    INVALID_ARG_FUN,
    NO_ARG,

    /* weird */
    UNWN
};

void report_err(enum codes error, const char* str)
{
    switch (error) {
    case INVALID_ARGV:
        puts("Perfmode: Invalid arguments\n"
             "View help with: perfmode -help");
        exit(1);

    case BAD_FP:
        printf("Perfmode: Bad file pointer %s\n", str);
        exit(1);

    case NO_MODULE_FOUND:
        puts("Perfmode: Kernel modules are not available!\n"
             "Visit https://github.com/icebarf/perfmode#troubleshooting");
        exit(1);

    case NO_PERMISSION:
        printf("Perfmode: No permission for %s\n", str);
        exit(1);

    case INVALID_ARG_FUN:
        printf("Perfmode: Invalid Argument to function %s\n", str);
        exit(1);

    case UNWN:
        printf("Perfmode: UNWN error - %s\n", str);
        exit(1);

    default:
        puts("Perfmode: Invalid Error Reported!");
        exit(1);
    }
}

static inline void report_msg(const char* str)
{
    printf("Perfmode: %s\n", str);
    exit(0);
}

void report(enum codes codes, enum codes err, const char* str)
{
    switch (codes) {
    case FAIL:
        report_err(err, str);
        exit(1);

    case SUCCESS:
        report_msg(str);
        exit(1);

    default:
        report_err(INVALID_ARG_FUN, "report()");
        exit(1);
    }
}

void print_help(void)
{
    puts("Perfmode - Manage performance mode of your asus laptop\n"

         "Usage:\n"
         "\tperfmode -option arg\n"

         "\nOptions:\n"

         "Fan Control:\n"

         "\t-fan TURBO         TURBO Mode\n"
         "\t-fan BALANCED      BALANCED Mode\n"
         "\t-fan SILENT        SILENT Mode\n"

         "Keyboard Backlight:\n"

         "\t-led OFF           Turn OFF Backlight\n"
         "\t-led MIN           MINimum Backlight\n"
         "\t-led MED           MEDium  Backlight\n"
         "\t-led MAX           MAXimum Backlight\n"

         "Help:\n"

         "\t-help          Display help\n"
         "Visit github for more info or updates: "
         "https://github.com/icebarf/perfmode");
}

void handle_fan_or_led(char* argv[], signed char* operation,
                       signed char* operation_other)
{
    if (strncmp(argv[1], "-fan", FAN_OR_LED) == 0) {

        if (strnlen(argv[2], 11)) {
            if (strncmp(argv[2], "silent", SILENT_LN) == 0) {
                *operation = SILENT;
                *operation_other = fan;
                return;
            }

            if (strncmp(argv[2], "balanced", BALANCED_LN) == 0) {
                *operation = BALANCED;
                *operation_other = fan;
                return;
            }

            if (strncmp(argv[2], "turbo", TURBO_LN) == 0) {
                *operation = TURBO;
                *operation_other = fan;
                return;
            }

            /* If doesn't match any of the cases above throw error */
            report(FAIL, INVALID_ARGV, NULL);
        }
    }

    if (strncmp(argv[1], "-led", FAN_OR_LED) == 0) {

        if (strnlen(argv[2], 5)) {
            if (strncmp(argv[2], "off", LED_ARG_LN) == 0) {
                *operation = OFF;
                *operation_other = led;
                return;
            }

            if (strncmp(argv[2], "min", LED_ARG_LN) == 0) {
                *operation = MIN;
                *operation_other = led;
                return;
            }

            if (strncmp(argv[2], "med", LED_ARG_LN) == 0) {
                *operation = MED;
                *operation_other = led;
                return;
            }

            if (strncmp(argv[2], "max", LED_ARG_LN) == 0) {
                *operation = MAX;
                *operation_other = led;
                return;
            }

            /* If doesn't match any of the cases above throw error */
            report(FAIL, INVALID_ARGV, NULL);
        }
    }

    /* If doesn't match any of the cases above throw error */
    report(FAIL, INVALID_ARGV, NULL);
}

void parse_argv(int argc, char* argv[], signed char* operation,
                signed char* operation_other)
{
    if (argc < 2) {
        *operation = help;
        return;
    }

    if ((argc > 1) && (argc < 4)) {
        size_t argv_len = strnlen(argv[1], 5);
        if ((argv_len != 0) && (argv_len == FAN_OR_LED) && (argc > 2)) {
            handle_fan_or_led(argv, operation, operation_other);
            return;
        }

        if ((argv_len != 0) && (argv_len == HELP_LN) && (argc == 2)) {
            *operation = help;
            return;
        }

        /* If doesn't match any of the above cases throw error */
        *operation = invalid_op;
        report(FAIL, INVALID_ARGV, NULL);
    }

    /* If doesn't match any of the above cases throw error */
    *operation = invalid_op;
    report(FAIL, INVALID_ARGV, NULL);
}

void identify_kmodules(signed char* kmodule)
{
    /* Check for asus_nb_wmi */
    FILE* asus_fp = popen("lsmod | grep ^asus_nb_wmi", "r");

    if (asus_fp == NULL)
        report(FAIL, BAD_FP, NULL);

    char outbuf[25];
    if (fread(outbuf, 1, sizeof(outbuf), asus_fp) > 0) {
        *kmodule = asus_nb_wmi;
        return;
    }

    /* Check for faustus */
    FILE* faustus_fp = popen("lsmod | grep ^faustus", "r");

    if (faustus_fp == NULL)
        report(FAIL, BAD_FP, NULL);

    char f_outbuf[25];
    if (fread(f_outbuf, 1, sizeof(outbuf), faustus_fp) > 0) {
        *kmodule = faustus;
        return;
    }

    /* If none of the above modules exist */
    report(FAIL, NO_MODULE_FOUND, NULL);
}

void identify_kfiles(const signed char* kmodule, signed char* kfile,
                     const signed char* operation)
{
    switch (*operation) {
    case led: {
        int access_retval = access(file_list[LED_FILE], W_OK);

        /* access() - returns 0 on SUCCESS - W_OK checks if write is allowed
         */
        if (access_retval != 0)
            report(FAIL, NO_PERMISSION, file_list[LED_FILE]);

        *kfile = LED_MAIN;
        break;
    }
    case fan: {
        int fan_files[2] = {0};
        switch (*kmodule) {
        case asus_nb_wmi: {
            int files_av[2] = {0};
            files_av[0] = access(file_list[AFAN_FILE], F_OK | W_OK);
            files_av[1] = access(file_list[AFAN_FILE_2], F_OK | W_OK);

            if (files_av[0] == -1 && files_av[1] == -1) {
                fan_files[0] = 1;
                break;
            }
            if ((files_av[0] | files_av[1]) == 0) {
                *kfile = ASUS_FAN_BOTH;
                break;
            }

            if (files_av[0] == 0) {
                *kfile = ASUS_FAN_MAIN;
                break;
            }

            if (files_av[1] == 0) {
                *kfile = ASUS_FAN_OTHER;
                break;
            }
            break;
        }

        case faustus: {
            int files_av[2] = {0};
            files_av[0] = access(file_list[FFAN_FILE], F_OK | W_OK);
            files_av[1] = access(file_list[FFAN_FILE_2], F_OK | W_OK);

            if (files_av[0] == -1 && files_av[1] == -1) {
                fan_files[1] = 1;
                break;
            }

            if ((files_av[0] | files_av[1]) == 0) {
                *kfile = FAUSTUS_FAN_BOTH;
                break;
            }

            if (files_av[0] == 0) {
                *kfile = FAUSTUS_FAN_MAIN;
                break;
            }
            if (files_av[1] == 0) {
                *kfile = FAUSTUS_FAN_OTHER;
                break;
            }
            break;
        }
        default:
            report(FAIL, INVALID_ARG_FUN, "identify_kfiles()");
        }

        /* error handling */
        if (fan_files[0] == 1 || fan_files[1] == 1)
            report(FAIL, NO_PERMISSION, "fan handling policy file");

        break;
    }
    default:
        report(FAIL, INVALID_ARG_FUN, "identify_kfiles()");
    }
}

const char* which_file(const signed char* kfile)
{
    switch (*kfile) {
    case LED_MAIN:
        return file_list[LED_FILE];

    /* default to main file if
     * both files are present
     * for both fasutus and asus
     */
    case ASUS_FAN_BOTH:
    case ASUS_FAN_MAIN:
        return file_list[AFAN_FILE];

    case ASUS_FAN_OTHER:
        return file_list[AFAN_FILE_2];

    case FAUSTUS_FAN_BOTH:
    case FAUSTUS_FAN_MAIN:
        return file_list[FFAN_FILE];

    case FAUSTUS_FAN_OTHER:
        return file_list[FFAN_FILE_2];

    default:
        return NULL;
    }
}

void write_file(const char* file, const char ch, const enum operations ops)
{
    FILE* fp = fopen(file, "w");
    if (fp == NULL) {
        report(FAIL, BAD_FP, "at write_file()");
        report(FAIL, NO_PERMISSION, file);
    }

    char* mode = NULL;
    char* opmode = NULL;

    switch (ops) {
    /* fans */
    case SILENT:
        mode = "silent";
        opmode = "fans";
        break;

    case BALANCED:
        mode = "balanced";
        opmode = "fans";
        break;

    case TURBO:
        mode = "turbo";
        opmode = "fans";
        break;

    /* leds */
    case OFF:
        mode = "off";
        opmode = "leds";
        break;

    case MIN:
        mode = "min";
        opmode = "leds";
        break;

    case MED:
        mode = "med";
        opmode = "leds";
        break;

    case MAX:
        mode = "max";
        opmode = "leds";
        break;

    default:
        mode = NULL;
        opmode = NULL;
        report(FAIL, UNWN, "Unknown edge case for operations");
    }

    char msg[35];
    char err_msg[35];
    snprintf(msg, 35, "Set %s to %s", opmode, mode);
    snprintf(err_msg, 35, "Couldn't set %s to %s", opmode, mode);

    if (fputc((int)ch, fp) == (int)ch) {
        report(SUCCESS, NO_ARG, msg);
    } else {
        fclose(fp);
        report(FAIL, UNWN, err_msg);
    }
}

void do_operation_fan(const signed char* kfile, const signed char* operation)
{
    const char* file = which_file(kfile);

    switch (*operation) {
    case SILENT:
        write_file(file, '2', SILENT);
        break;

    case BALANCED:
        write_file(file, '0', BALANCED);
        break;

    case TURBO:
        write_file(file, '1', TURBO);
        break;

    default:
        report(FAIL, INVALID_ARG_FUN, "do_operation()");
    }
}

void do_operation_led(const signed char* kfile, const signed char* operation)
{
    const char* file = which_file(kfile);

    switch (*operation) {
    case OFF:
        write_file(file, '0', OFF);
        break;

    case MIN:
        write_file(file, '1', MIN);
        break;

    case MED:
        write_file(file, '2', MED);
        break;

    case MAX:
        write_file(file, '3', MAX);
        break;
    }
}

void do_operation(const signed char* kfile, const signed char* operation)
{
    switch (operation[1]) {
    case fan:
        do_operation_fan(kfile, operation);
        break;

    case led:
        do_operation_led(kfile, operation);
        break;

    default:
        report(FAIL, INVALID_ARG_FUN, "do_operation()");
    }
}

int main(int argc, char* argv[])
{
    /* check what operation to do */
    signed char operation[2] = {0};
    parse_argv(argc, argv, operation, &operation[1]);

    if (operation[0] == help)
        print_help(), exit(1);

    /* check what kernel module is available on system */
    signed char kmodule = 0;
    identify_kmodules(&kmodule);

    /* Check which files are available */
    signed char kfile = 0;
    identify_kfiles(&kmodule, &kfile, &operation[1]);

    /* finally perform the operation */
    do_operation(&kfile, operation);

    return 0;
}