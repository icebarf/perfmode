#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum file_list_enum
{
    LED_FILE,
    AFAN_FILE,
    AFAN_FILE_2,
    FFAN_FILE,
    FFAN_FILE_2
};

static const char* file_list[5] = 
{
    [LED_FILE] = "/sys/class/leds/asus::kbd_backlight/brightness",
    [AFAN_FILE] = "/sys/devices/platform/asus-nb-wmi/throttle_thermal_policy",
    [AFAN_FILE_2] = "/sys/devices/platform/asus-nb-wmi/fan_boost_mode",
    [FFAN_FILE] = "/sys/devices/platform/faustus/throttle_thermal_policy",
    [FFAN_FILE_2] = "/sys/devices/platform/faustus/fan_boost_mode"
};

enum operations
{
    /* LED operations */
    off,
    min,
    med,
    max,

    /* Fan Control operations */
    silent,
    balanced,
    turbo,

    /* Help and others */
    led,
    fan,
    help,
    invalid_op
};

enum kmodules {
    asus_nb_wmi,
    faustus,
    nomodule
};

enum kfiles {
    led_main,
    
    asus_fan_main,
    asus_fan_other,
    asus_fan_both,

    faustus_fan_main,
    faustus_fan_other,
    faustus_fan_both
};

enum lengths_expected
{
    fan_or_led = 4,

    silent_l = 6,
    balanced_l = 8,
    turbo_l = 5,

    led_arg_len = 3,

    help_l = 5
};

enum codes
{
    success,
    fail,

    /* errors */
    invalid_argv,
    bad_fp,
    no_module_found,
    no_permission,
    invalid_argument_to_function,
    no_arg,

    /* weird */
    unknown
};

static void
report_err (enum codes error, const char *str)
{
    switch(error)
    {
        case invalid_argv:
            puts("Perfmode: Invalid arguments\n"
                    "View help with: perfmode -help");
            exit(1);

        case bad_fp:
            printf("Perfmode: Bad file pointer %s\n", str);
            exit(1);
        
        case no_module_found:
            puts("Perfmode: Kernel modules are not available!\n"
                    "Visit https://github.com/icebarf/perfmode#troubleshooting");
            exit(1);

        case no_permission:
            printf("Perfmode: No permission for %s\n", str);
            exit(1);

        case invalid_argument_to_function:
            printf("Perfmode: Invalid Argument to function %s\n", str);
            exit(1);

        case unknown:
            printf("Perfmode: Unknown error - %s\n", str);
            exit(1);
        
        default:
            puts("Perfmode: Invalid Error Reported!");
            exit(1);
    }
}

static inline void
report_msg(const char* str)
{
   printf("Perfmode: %s\n", str);
   exit(0);
}

static void
report(enum codes codes, enum codes err, const char* str)
{
    switch(codes)
    {
        case fail:
            report_err(err,str);
            exit(1);

        case success:
            report_msg(str);
            exit(1);

        default:
            report_err(invalid_argument_to_function, "report()");
            exit(1);
    }
}

static void
print_help(void)
{
    puts("Perfmode - Manage performance mode of your asus laptop\n"

         "Usage:\n"
         "\tperfmode -option\n"

         "Note: Elevated permissions are required: Run with sudo\n"

         "Options:\n"

         "Fan Control:\n"

         "\t-fan turbo         Turbo Mode\n"
         "\t-fan balanced      Balanced Mode\n"
         "\t-fan silent        Silent Mode\n"

         "Keyboard Backlight:\n"

         "\t-led off           Turn off Backlight\n"
         "\t-led min           Minimum Backlight\n"
         "\t-led med           Medium  Backlight\n"
         "\t-led max           Maximum Backlight\n"

         "Help:\n"

         "\t-help          Display help\n"
         "Visit github for more info or updates: https://github.com/icebarf/perfmode");
}

static void 
handle_fan_or_led(char* argv[], signed char* operation, signed char* operation_other)
{
    if (strncmp(argv[1], "-fan", fan_or_led) == 0)
    {
        size_t argv_len = strnlen(argv[2], 11);
        if (argv_len != 0)
        {
            if (strncmp(argv[2], "silent", argv_len) == 0)
            {
                *operation = silent;
                *operation_other = fan;
                return;
            }

            if (strncmp(argv[2], "balanced", argv_len) == 0)
            {
                *operation = balanced;
                *operation_other = fan;
                return;
            }

            if (strncmp(argv[2], "turbo", argv_len) == 0)
            {
                *operation = turbo;
                *operation_other = fan;
                return;
            }

            /* If doesn't match any of the cases above throw error */
            report(fail, invalid_argv, NULL);
        }
    }

    if (strncmp(argv[1], "-led", fan_or_led) == 0)
    {
        size_t argv_len = strnlen(argv[2], 5);
        if (argv_len != 0)
        {
            if (strncmp(argv[2], "off", argv_len) == 0)
            {
                *operation = off;
                *operation_other = led;
                return;
            }

            if (strncmp(argv[2], "min", argv_len) == 0)
            {
                *operation = min;
                *operation_other = led;
                return;
            }

            if (strncmp(argv[2], "med", argv_len) == 0)
            {
                *operation = med;
                *operation_other = led;
                return;
            }

            if (strncmp(argv[2], "max", argv_len) == 0)
            {
                *operation = max;
                *operation_other = led;
                return;
            }

            /* If doesn't match any of the cases above throw error */
            report(fail, invalid_argv, NULL);
        }
    }

    /* If doesn't match any of the cases above throw error */
    report(fail, invalid_argv, NULL);

}

static void
parse_argv (int argc, char* argv[], signed char* operation, signed char* operation_other)
{
    if (argc < 2)
    {
        *operation = help;
        return;
    }
    
    if ((argc > 1) && (argc < 4))
    {
        size_t argv_len = strnlen(argv[1], 5);
        if ((argv_len != 0) && (argv_len == fan_or_led) && (argc > 2))
        {
           handle_fan_or_led (argv, operation, operation_other);
           return;
        }

        if ((argv_len != 0) && (argv_len == help_l) && (argc == 2))
        {
            *operation = help;
            return;
        }

        /* If doesn't match any of the above cases throw error */
        *operation = invalid_op;
        report(fail, invalid_argv, NULL);
    }

    /* If doesn't match any of the above cases throw error */
    *operation = invalid_op;
    report(fail, invalid_argv, NULL);
}

static void
identify_kmodules(signed char* kmodule)
{
   /* Check for asus_nb_wmi */ 
    FILE* asus_fp = popen("lsmod | grep ^asus_nb_wmi", "r");

    if (asus_fp == NULL)
        report_err(bad_fp, NULL);

    char outbuf[25]; 
    if (fread(outbuf, 1, sizeof(outbuf), asus_fp) > 0)
    {
        *kmodule = asus_nb_wmi;
        return;
    }

   /* Check for faustus */ 
    FILE* faustus_fp = popen("lsmod | grep ^faustus", "r");

    if (faustus_fp == NULL)
        report_err(bad_fp, NULL);

    char f_outbuf[25]; 
    if (fread(f_outbuf, 1, sizeof(outbuf), faustus_fp) > 0)
    {
        *kmodule = faustus;
        return;
    }

    /* If none of the above modules exist */
    report(fail, no_module_found, NULL);
}

static void
identify_kfiles (const signed char* kmodule, signed char* kfile, const signed char* operation)
{
   switch(*operation)
   {
    case led:
        {
            int access_retval = access(file_list[LED_FILE], F_OK |  W_OK);
            
            /* access() - returns 0 on success - F_OK checks for
             * existence of files and W_OK checks if write is allowed
             */
            if (access_retval != 0)
                report(fail, no_permission, file_list[LED_FILE]);

            *kfile = led_main;
            break;
        }
    case fan:
        {
           int fan_files[2] = {0};
           switch(*kmodule)
           {
            case asus_nb_wmi:
                {
                    int files_av[2] = {0};
                    files_av[0] = access(file_list[AFAN_FILE], F_OK | W_OK);
                    files_av[1] = access(file_list[AFAN_FILE_2], F_OK | W_OK);
                    
                    if (files_av[0] == -1 && files_av[1] == -1)
                    {
                        fan_files[0] = 1;
                        break;
                    }
                    if ((files_av[0] | files_av[1]) == 0)
                    {
                        *kfile = asus_fan_both;
                        break;
                    }

                    if (files_av[0] == 0)
                    {
                        *kfile = asus_fan_main;
                        break;
                    }

                    if (files_av[1] == 0)
                    {
                        *kfile = asus_fan_other;
                        break;
                    }
                    break;
                }

            case faustus:
                {
                    int files_av[2] = {0};
                    files_av[0] = access(file_list[FFAN_FILE], F_OK | W_OK);
                    files_av[1] = access(file_list[FFAN_FILE_2], F_OK | W_OK);
                    
                    if (files_av[0] == -1 && files_av[1] == -1)
                    {
                        fan_files[1] = 1; break;
                    }

                    if ((files_av[0] | files_av[1]) == 0)
                    {
                        *kfile = faustus_fan_both;
                        break;
                    }

                    if (files_av[0] == 0)
                    {
                        *kfile = faustus_fan_main;
                        break;
                    }
                    if (files_av[1] == 0)
                    {
                        *kfile = faustus_fan_other;
                        break;
                    }
                    break;
                }
             default:
                report(fail, invalid_argument_to_function, "identify_kfiles()");
            }

           /* error handling */
           if (fan_files[0] == 1 || fan_files[1] == 1)
               report(fail, no_permission, "fan handling policy file");

           break;
        }
        default:
            report(fail, invalid_argument_to_function, "identify_kfiles()");
    } 
}

static const char*
which_file(const signed char* kfile)
{
    switch(*kfile)
    {
        case led_main:
            return file_list[LED_FILE];

        /* default to main file if
         * both files are present
         * for both fasutus and asus
         */
        case asus_fan_both:
        case asus_fan_main:
            return file_list[AFAN_FILE];

        case asus_fan_other:
            return file_list[AFAN_FILE_2];

        case faustus_fan_both:
        case faustus_fan_main:
            return file_list[FFAN_FILE];

        case faustus_fan_other:
            return file_list[FFAN_FILE_2];
       
        default:
           return NULL; 
    }
}

static void
fan_silent(const char* file)
{
    FILE* fp = fopen(file, "w");
    if(fp == NULL)
        report(fail, bad_fp, "at fan_silent()");

    if(fputc('2', fp) == '2')
    {
        fclose(fp);
        report(success, no_arg, "Set fans to silent mode");
    }
    else
    { 
        fclose(fp);
        report(fail, unknown, "Couldn't set fans to silent mode");
    }
}

static void
fan_balanced(const char* file)
{
    FILE* fp = fopen(file, "w");
    if (fp == NULL)
        report(fail, bad_fp, "at fan_balanced()");

    if(fputc('0', fp) == '0')
    {
        fclose(fp);
        report(success, no_arg, "Set fans to balanced mode");
    }
    else
    {
        fclose(fp);
        report(fail , unknown, "Couldn't set fans to balanced mode");
    }
}

static void
fan_turbo(const char* file)
{
    FILE* fp = fopen(file, "w");
    if (fp == NULL)
        report(fail, bad_fp, "at fan_balanced()");

    if(fputc('1', fp) == '1')
    {
        report(success, no_arg, "Set fans to turbo mode");
    }
    else
    {
        fclose(fp);
        report(fail , unknown, "Couldn't set fans to turbo mode");
    }
}

static void
do_operation_fan(const signed char* kfile, const signed char* operation)
{
    assert((*kfile >= asus_fan_main) && (*kfile <= asus_fan_both));

    switch(*operation)
    {
        case silent: {
            const char* file = which_file(kfile);
            fan_silent(file);
            break;
        }

        case balanced: {
            const char* file = which_file(kfile);
            fan_balanced(file);
            break;
        }
        case turbo: {
            const char* file = which_file(kfile);
            fan_turbo(file);
            break;
        }
        default:
            report(fail, invalid_argument_to_function, "do_operation()");
    }
}

static void
led_off(const char* file)
{
    FILE* fp = fopen(file, "w");
    if (fp == NULL)
        report(fail, bad_fp, "at led_off()");

    if (fputc('0', fp) == '0')
    {
        fclose(fp);
        report(success, no_arg, "Turned off leds");
    }
    else 
    {
        fclose(fp);
        report(fail, unknown, "Couldn't turn off leds");
    }
}

static void
led_min(const char* file)
{
    FILE* fp = fopen(file, "w");
    if (fp == NULL)
        report(fail, bad_fp, "at led_min()");

    if (fputc('1', fp) == '1')
    {
        fclose(fp);
        report(success, no_arg, "Set leds to minimum brightness");
    }
    else 
    {
        fclose(fp);
        report(fail, unknown, "Couldn't set leds to min brightness");
    }
}

static void
led_med(const char* file)
{
    FILE* fp = fopen(file, "w");
    if (fp == NULL)
        report(fail, bad_fp, "at led_med()");

    if (fputc('2', fp) == '2')
    {
        fclose(fp);
        report(success, no_arg, "Set leds to medium brightness");
    }
    else 
    {
        fclose(fp);
        report(fail, unknown, "Couldn't set leds to med brightness");
    }
}

static void
led_max(const char* file)
{
    FILE* fp = fopen(file, "w");
    if (fp == NULL)
        report(fail, bad_fp, "at led_max()");

    if (fputc('3', fp) == '3')
    {
        fclose(fp);
        report(success, no_arg, "Set leds to maximum brightness");
    }
    else 
    {
        fclose(fp);
        report(fail, unknown, "Couldn't set leds to max brightness");
    }
}

static void
do_operation_led(const signed char* kfile, const signed char* operation)
{
    assert(*kfile == led_main);

    switch(*operation)
    {
        case off: {
            const char* file = which_file(kfile);
            led_off(file);
            break;
        }

        case min: {
            const char* file = which_file(kfile);
            led_min(file);
            break;
        }

        case med: {
            const char* file = which_file(kfile);
            led_med(file);
            break;
        }

        case max: {
            const char* file = which_file(kfile);
            led_max(file);          
            break;
        }
    }
}

static void
do_operation(const signed char* kfile, const signed char *operation)
{
    switch(operation[1])
    {
        case fan:
            do_operation_fan(kfile, operation);
            break;

        case led:
            do_operation_led(kfile, operation);
            break;

        default:
            report_err(invalid_argument_to_function, "do_operation");
    }
}

int 
main (int argc, char* argv[])
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
