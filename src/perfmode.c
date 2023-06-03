#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum file_list_enum {
    LED_FILE,
    ASUS_THERMAL_POLICY,
    ASUS_FAN_POLICY,
    FSTS_THERMAL_POLICY,
    FSTS_FAN_POLICY
};

static const char* file_list[] = {
    [LED_FILE] = "/sys/class/leds/asus::kbd_backlight/brightness",
    [ASUS_THERMAL_POLICY] =
        "/sys/devices/platform/asus-nb-wmi/throttle_thermal_policy",
    [ASUS_FAN_POLICY] = "/sys/devices/platform/asus-nb-wmi/fan_boost_mode",
    [FSTS_THERMAL_POLICY] =
        "/sys/devices/platform/faustus/throttle_thermal_policy",
    [FSTS_FAN_POLICY] = "/sys/devices/platform/faustus/fan_boost_mode"};

enum operations {
    /* LED */
    OFF,
    MIN,
    MED,
    MAX,

    /* Fan Control Modes */
    SILENT,
    BALANCED,
    TURBO,

    /* Thermal policy - Silent is common to both Fan and Thermal policy */
    OVERBOOST,
    DEFAULT,

    /* Reading */
    GET,
};

enum operators {
    led,
    fan,
    thermal,
    help,
};

/* operations */
#define OFF_s "off" /* the s stands for string */
#define MIN_s "min"
#define MED_s "med"
#define MAX_s "max"
#define SILENT_s "silent"
#define SILENT_ss "s" /* the ss stands for short-string*/
#define BALANCED_s "balanced"
#define BALANCED_ss "b"
#define TURBO_s "turbo"
#define TURBO_ss "t"
#define OVERBOOST_s "overboost"
#define OVERBOOST_ss "ob"
#define DEFAULT_s "default"
#define DEFAULT_ss "df"
#define GET_s "get"
#define GET_ss "g"

/* operators */
#define LED_s "-led"
#define LED_ss "-l"
#define FAN_s "-fan"
#define FAN_ss "-f"
#define THERMAL_s "-thermal"
#define THERMAL_ss "-t"
#define HELP_s "-help"
#define HELP_ss "-h"

enum kmodules { asus_nb_wmi, faustus, nomodule };

/* !!!! Will only work with char[], DO NOT USE FOR ANYTHING ELSE!!!!!! */
#define const_strlen(X) (sizeof(X) - 1)

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

[[noreturn]] void report_err(enum codes error, const char* str)
{
    switch (error) {
    case INVALID_ARGV:
        puts("Perfmode: Invalid arguments\n"
             "View help with: perfmode -help");
        break;

    case BAD_FP:
        printf("Perfmode: Bad file pointer %s\n", str);
        break;

    case NO_MODULE_FOUND:
        puts("Perfmode: Kernel modules are not available!\n"
             "Visit https://github.com/icebarf/perfmode#troubleshooting");
        break;

    case NO_PERMISSION:
        printf("Perfmode: No permission for %s.\n", str);
        break;

    case INVALID_ARG_FUN:
        printf("Perfmode: Invalid Argument to function %s\n", str);
        break;

    case UNWN:
    default:
        puts("Perfmode: Invalid Error Reported!");
    }
    exit(1);
}

static inline void report_msg(const char* str)
{
    printf("Perfmode: %s\n", str);
}

void report(enum codes codes, enum codes err, const char* str)
{
    switch (codes) {
    case FAIL:
        report_err(err, str);

    case SUCCESS:
        report_msg(str);
        break;

    default:
        report_err(INVALID_ARG_FUN, "report()");
    }
}

#define BOLD "\033[1m"
#define ITALIC "\033[3m"
#define ULINE "\033[4m"
#define RESET "\033[m"

#define FG_RED "\033[31m"
#define FG_GREEN "\033[32m"

// clang-format off
void print_help(void)
{
    puts(FG_GREEN
         "Perfmode - Manage performance mode of your asus laptop\n" RESET

         BOLD ULINE FG_RED "Usage\n" RESET "\tperfmode -option arg\n"

         BOLD ULINE FG_RED "\nOptions\n" RESET

         BOLD ULINE FG_RED "\nFan Control\n" RESET

         FG_RED "\t-fan turbo"RESET"          Turbo Mode\n"
         FG_RED "\t-fan balanced"RESET"       Balanced Mode\n"
         FG_RED "\t-fan silent"RESET"         Silent Mode\n"

         BOLD ULINE FG_RED "\nThermal Policy\n" RESET

         FG_RED"\t-thermal overboost"RESET"  Overboost Mode\n"
         FG_RED"\t-thermal default"RESET"    Default Mode\n"
         FG_RED"\t-thermal silent"RESET"     Silent Mode\n"

         BOLD ULINE FG_RED "\nKeyboard Backlight\n" RESET

         FG_RED "\t-led off"RESET"            Turn off Backlight\n"
         FG_RED "\t-led min"RESET"            Minimum Backlight\n"
         FG_RED "\t-led med"RESET"            Medium  Backlight\n"
         FG_RED "\t-led max"RESET"            Maximum Backlight\n"

         BOLD ULINE FG_RED "\nCommon option for all kinds of operations\n" RESET
         FG_RED "\tget"RESET"                 get the current thermal, led, fan mode\n"
         "                            eg. perfmode -fan get or perfmode -led get\n"
         BOLD ULINE FG_RED "Help\n" RESET

         FG_RED"\t-help"RESET"               Display help\n"

         BOLD ULINE FG_RED "\nNotes\n" RESET
         "\n1. Using either fan control or thermal policy options is fine.\n"
         "   There is no strict requirement that both should be used."
         "   Simply use one of those or both, or whichever is currently supported on"
         " your asus laptop.\n"
         "\n2. -fan, -thermal, -led and -help can be substituted with"
         " -f, -t, -l and -h respectively.\n"
         "\n3. Along with (2), operations such as silent, balanced,\n"
         "   turbo, overboost and get are substitutible with s, b, t, ob, df and g respectively.\n"

         ITALIC FG_GREEN "\nVisit github for more info or updates: "
         "https://github.com/icebarf/perfmode\n" RESET);
}

// clang-format on

/* Only specific to the following two functions */
#define operator_cmp(s2) strncmp(argv[1], s2, operator_len + 1)
#define operation_cmp(s2) strncmp(argv[2], s2, operation_len + 1)

void handle_fan_or_led(char* argv[], enum operators* operator,
                       enum operations * operation)
{
    size_t operator_len = strlen(argv[1]);
    size_t operation_len = strlen(argv[2]);

    if (operator_cmp(FAN_s) == 0 || operator_cmp(FAN_ss) == 0) {

        if (operation_cmp(SILENT_s) == 0 || operation_cmp(SILENT_ss) == 0) {
            *operation = SILENT;
            *operator= fan;
            return;
        } else if (operation_cmp(BALANCED_s) == 0 ||
                   operation_cmp(BALANCED_ss) == 0) {
            *operation = BALANCED;
            *operator= fan;
            return;
        } else if (operation_cmp(TURBO_s) == 0 ||
                   operation_cmp(TURBO_ss) == 0) {
            *operation = TURBO;
            *operator= fan;
            return;
        } else if (operation_cmp(GET_s) == 0 || operation_cmp(GET_ss) == 0) {
            *operation = GET;
            *operator= fan;
            return;
        }

        report(FAIL, INVALID_ARGV, NULL);
    }

    if (operator_cmp(THERMAL_s) == 0 || operator_cmp(THERMAL_ss) == 0) {

        if (operation_cmp(SILENT_s) == 0 || operation_cmp(SILENT_ss) == 0) {
            *operation = SILENT;
            *operator= thermal;
            return;
        } else if (operation_cmp(DEFAULT_s) == 0 ||
                   operation_cmp(DEFAULT_ss) == 0) {
            *operation = DEFAULT;
            *operator= thermal;
            return;
        } else if (operation_cmp(OVERBOOST_s) == 0 ||
                   operation_cmp(OVERBOOST_ss) == 0) {
            *operation = OVERBOOST;
            *operator= thermal;
            return;
        } else if (operation_cmp(GET_s) == 0 || operation_cmp(GET_ss) == 0) {
            *operation = GET;
            *operator= thermal;
            return;
        }

        report(FAIL, INVALID_ARGV, NULL);
    }

    if (operator_cmp(LED_s) == 0 || operator_cmp(LED_ss) == 0) {

        if (operation_cmp(OFF_s) == 0) {
            *operation = OFF;
            *operator= led;
            return;

        } else if (operation_cmp(MIN_s) == 0) {
            *operation = MIN;
            *operator= led;
            return;

        } else if (operation_cmp(MED_s) == 0) {
            *operation = MED;
            *operator= led;
            return;

        } else if (operation_cmp(MAX_s) == 0) {
            *operation = MAX;
            *operator= led;
            return;
        } else if (operation_cmp(GET_s) == 0 || operation_cmp(GET_ss) == 0) {
            *operation = GET;
            *operator= led;
            return;
        }

        report(FAIL, INVALID_ARGV, NULL);
    }

    report(FAIL, INVALID_ARGV, NULL);
}

void parse_argv(int argc, char* argv[], enum operators* operator,
                enum operations * operation)
{
    if (argc < 2) {
        *operator= help;
        return;
    }
    size_t operator_len = strlen(argv[1]);

    assert(argv[1] != NULL);
    if (argc >= 2 && argc < 4) {
        if (operator_cmp(HELP_s) == 0 || operator_cmp(HELP_ss) == 0) {
            *operator= help;
            return;
        } else {
            if (argc != 3)
                goto fail;
            handle_fan_or_led(argv, operator, operation);
            return;
        }
    }

fail:
    report(FAIL, INVALID_ARGV, NULL);
}

static inline bool file_exists(enum file_list_enum to_check,
                               enum operations operation)
{
    uint8_t perms = F_OK | W_OK;

    if (operation == GET)
        perms = F_OK | W_OK;

    if (access(file_list[to_check], perms) == -1)
        return false;

    return true;
}

/* We can assume that either asus_nb_wmi or faustus modules will be loaded
 * but not both. From this assumption we can always assume that in the
 * `fan` and `thermal` cases, either a single file will be selected or none.
 * see: https://github.com/hackbnw/faustus#disable-original-modules
 */
void identify_files(enum file_list_enum* file, enum operators operator,
                          enum operations operation)
{
    switch (operator) {
    case led:
        if (file_exists(LED_FILE, operation))
            *file = LED_FILE;
        else 
            report(FAIL, NO_PERMISSION, file_list[LED_FILE]);
	
        return;

    case fan:
        if (file_exists(ASUS_FAN_POLICY, operation))
            *file = ASUS_FAN_POLICY;
        else if (file_exists(FSTS_FAN_POLICY, operation))
            *file = FSTS_FAN_POLICY;
        else 
            report(FAIL, NO_PERMISSION, "module files. "
                   "Make sure either you're running as root or "
                   "have the requried kernel modules");
        
        return;

    case thermal:
        if (file_exists(ASUS_THERMAL_POLICY, operation))
            *file = ASUS_THERMAL_POLICY;
        else if (file_exists(FSTS_THERMAL_POLICY, operation))
            *file = FSTS_THERMAL_POLICY;
        else 
            report(FAIL, NO_PERMISSION, "module files. "
                   "Make sure either you're running as root or "
                   "have the requried kernel modules");
        
        return;

    default:
        report(FAIL, INVALID_ARG_FUN, "identify_kfiles()");
    }
}

static void write_file_log(enum operators operator, enum operations operation,
                           char** operator_str, char** operation_str,
                           char (*msg)[100], char (*errmsg)[100])
{
    switch (operator) {
    case led:
        *operator_str = LED_s + 1; /* skip the `-` */
        break;
    case fan:
        *operator_str = FAN_s + 1;
        break;
    case thermal:
        *operator_str = THERMAL_s + 1;
        break;
    default:
        report(FAIL, INVALID_ARG_FUN, "write_file_log()");
    }

    switch (operation) {
    case OFF:
        *operation_str = OFF_s;
        break;
    case MIN:
        *operation_str = MIN_s;
        break;
    case MED:
        *operation_str = MED_s;
        break;
    case MAX:
        *operation_str = MAX_s;
        break;
    case SILENT:
        *operation_str = SILENT_s;
        break;
    case BALANCED:
        *operation_str = BALANCED_s;
        break;
    case TURBO:
        *operation_str = TURBO_s;
        break;
    case OVERBOOST:
        *operation_str = OVERBOOST_s;
        break;
    case DEFAULT:
        *operation_str = DEFAULT_s;
        break;
    default:
        report(FAIL, INVALID_ARG_FUN, "write_file_log()");
    }

    snprintf(*msg, 100, "Set %s policy to %s", *operator_str, *operation_str);
    snprintf(*errmsg, 100, "Couldn't set %s policy to %s", *operator_str,
             *operation_str);
}

// clang-format off
void read_file(const char* file, enum operators operator)
{
    FILE* f = fopen(file, "r");
    int current = fgetc(f);
    if (current == EOF)
        report(FAIL, NO_PERMISSION, "Unable to perform a read");
 
   switch (operator)
    {
        case led:
            switch (current)
            {
                case '0': fprintf(stdout, OFF_s "\n"); break;
                case '1': fprintf(stdout, MIN_s "\n"); break;
                case '2': fprintf(stdout, MED_s "\n"); break;
                case '3': fprintf(stdout, MAX_s "\n"); break;
            }
            break;
        case fan:
            switch (current)
            {
                case '0': fprintf(stdout, BALANCED_s "\n"); break;
                case '1': fprintf(stdout, TURBO_s "\n"); break;
                case '2': fprintf(stdout, SILENT_s "\n"); break;
            }
            break;
        case thermal:
            switch (current)
            {
                case '0': fprintf(stdout, DEFAULT_s "\n"); break;
                case '1': fprintf(stdout, OVERBOOST_s "\n"); break;
                case '2': fprintf(stdout, SILENT_s "\n"); break;
            }
            break;
        default:
            report(FAIL, INVALID_ARG_FUN, "invalid operator to read_file()\n");
    }
}

// clang-format on

void write_file(const char* file, int8_t ch, enum operators operator,
                enum operations operation)
{
    FILE* fp = fopen(file, "w");
    if (fp == NULL) 
        report(FAIL, BAD_FP, "at write_file()");

    /* Logging stuff */
    char *operator_str, *operation_str;
    char msg[100], errmsg[100];
    write_file_log(operator, operation, &operator_str, &operation_str, &msg,
                   &errmsg);

    if (fputc(ch, fp) == ch) {
        report(SUCCESS, NO_ARG, msg);
        fclose(fp);
	
    } else {
        fclose(fp);
        report(FAIL, UNWN, errmsg);
	exit(1);
    }
}

void do_operation_fan(enum file_list_enum kfile, enum operators operator,
                      enum operations operation)
{
    const char* file = file_list[kfile];

    switch (operation) {
    case SILENT:
        write_file(file, '2', operator, operation);
        break;

    case BALANCED:
        write_file(file, '0', operator, operation);
        break;

    case TURBO:
        write_file(file, '1', operator, operation);
        break;

    case GET:
        read_file(file, operator);
        break;

    default:
        report(FAIL, INVALID_ARG_FUN, "do_operation_fan()");
    }
}

void do_operation_thermals(enum file_list_enum kfile, enum operators operator,
                           enum operations operation)
{
    const char* file = file_list[kfile];

    switch (operation) {
    case SILENT:
        write_file(file, '2', operator, operation);
        break;

    case DEFAULT:
        write_file(file, '0', operator, operation);
        break;

    case OVERBOOST:
        write_file(file, '1', operator, operation);
        break;

    case GET:
        read_file(file, operator);
        break;

    default:
        report(FAIL, INVALID_ARG_FUN, "do_operation_fan()");
    }
}

void do_operation_led(enum file_list_enum kfile, enum operators operator,
                      enum operations operation)
{
    const char* file = file_list[kfile];

    switch (operation) {
    case OFF:
        write_file(file, '0', operator, operation);
        break;

    case MIN:
        write_file(file, '1', operator, operation);
        break;

    case MED:
        write_file(file, '2', operator, operation);
        break;

    case MAX:
        write_file(file, '3', operator, operation);
        break;

    case GET:
        read_file(file, operator);
        break;

    default:
        report(FAIL, INVALID_ARG_FUN, "do_operation_fan()");
    }
}

void do_action(enum file_list_enum kfile, enum operators operator,
               enum operations operation)

{
    switch (operator) {
    case fan:
        do_operation_fan(kfile, operator, operation);
        break;

    case thermal:
        do_operation_thermals(kfile, operator, operation);
        break;

    case led:
        do_operation_led(kfile, operator, operation);
        break;

    default:
        report(FAIL, INVALID_ARG_FUN, "do_action()");
    }
}

int main(int argc, char* argv[])
{
    /* check what operation to do */
    enum operators operator;
    enum operations operation;
    parse_argv(argc, argv, &operator, & operation);

    if (operator== help)
        print_help(), exit(1);

    /* Check which files are available */
    enum file_list_enum kfile;
    identify_files(&kfile, operator, operation);

    /* finally perform the operation */
    do_action(kfile, operator, operation);

    return 0;
}
