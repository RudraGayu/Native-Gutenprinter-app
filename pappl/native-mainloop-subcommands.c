
#include <gimp-print/gimp-print.h>
#include <gutenprint/gutenprint.h>
#include "native-mainloop-subcommands.h"

//Still Build Encountered failure

int 
nativeMainLoopSubAddPrinters()
{
stp_init();
// Have to work on these methods , buld it , link it to gutenprint and then test then serially
// By serially I mean I will first check if the printer is getting added or not
// And then check if they are displayed or not
// then I will use these libgutenprint library for setting options 
stpi_init_printer_list(void);
}


int
nativeMainLoopSubShowPrinters()
{
stp_init();

 
  int   i;  /* Looping var */
  const stp_printer_t *printer; /* Pointer to printer driver */



for (i = 0; i < stp_printer_model_count(); i++)
    if ((printer = stp_get_printer_by_index(i)) != NULL)
    {
      const char *device_id;
      if (!strcmp(stp_printer_get_family(printer), "ps") ||
   !strcmp(stp_printer_get_family(printer), "raw"))
        continue;

      device_id = stp_printer_get_device_id(printer);
//    printf("%s \n", stp_printer_get_driver(printer) );
//    printf("%s \n", stp_printer_get_manufacturer(printer) );
   printf("%s \n", stp_printer_get_long_name(printer) );
    }

return 0;
}