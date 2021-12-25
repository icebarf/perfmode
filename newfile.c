static void
identify_kfiles (const signed char kmodule, signed char kfile, const signed char operation)
{
   switch(operation)
   {
    case led:
        {
            int access_retval = access(file_list[LED_FILE], F_OK |  W_OK);
            
            /* access() - returns 0 on success - F_OK checks for
             * existence of files and W_OK checks if write is allowed
             */
            if (access_retval != 0)
                report_err(no_permission, file_list[LED_FILE]);

            kfile = led_main;
        }
    case fan:
        {
           int fan_files[2] = {0};
           switch(kmodule)
           {
            case asus_nb_wmi:
                {
                    int files_av[2] = {0};
                    files_av[0] = access(file_list[AFAN_FILE], F_OK | W_OK);
                    files_av[1] = access(file_list[AFAN_FILE_2], F_OK | W_OK);
                    
                    if (files_av[0] == -1 && files_av[1] == -1)
                     fan_files[0] = 1;

                    if ((files_av[0] | files_av[1]) == 0)
                        kfile = asus_fan_both;

                    if (files_av[0] == 0)
                        kfile = asus_fan_main;

                    if (files_av[1] == 0)
                        kfile = asus_fan_other;
                }

            case faustus:
                {
                    int files_av[2] = {0};
                    files_av[0] = access(file_list[FFAN_FILE], F_OK | W_OK);
                    files_av[1] = access(file_list[FFAN_FILE_2], F_OK | W_OK);
                    
                    if (files_av[0] == -1 && files_av[1] == -1)
                         fan_files[1] = 1;

                    if ((files_av[0] | files_av[1]) == 0)
                        kfile = faustus_fan_both;

                    if (files_av[0] == 0)
                        kfile = faustus_fan_main;

                    if (files_av[1] == 0)
                        kfile = faustus_fan_other;
                }
             default:
                report_err(invalid_argument_to_function, "identify_kfiles");
            }

           /* error handling */
           if (fan_files[0] == 1 || fan_files[1] == 1)
               report_err(no_permission, "Both files do not exist");
        }
        default:
            report_err(invalid_argument_to_function, "identify_kfiles");
    } 
}
