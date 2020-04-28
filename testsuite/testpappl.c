//
// Main test suite file for the Printer Application Framework
//
// Copyright © 2020 by Michael R Sweet.
//
// Licensed under Apache License v2.0.  See the file "LICENSE" for more
// information.
//
// Usage:
//
//   testpappl [options] ["server name"]
//
// Options:
//
//   --help               Show help
//   --version            Show version
//   -1                   Single queue
//   -A pam-service       Enable authentication using PAM service
//   -c                   Do a clean run (no loading of state)
//   -d spool-directory   Set the spool directory
//   -l log-file          Set the log file
//   -L level             Set the log level (fatal, error, warn, info, debug)
//   -n hostname          Set the hostname
//   -p port              Set the listen port
//

//
// Include necessary headers...
//

#include <config.h>
#include "testpappl.h"


//
// Local functions...
//

static int	usage(int status);


//
// 'main()' - Main entry for test suite.
//

int
main(int  argc,				// I - Number of command-line arguments
     char *argv[])			// I - Command-line arguments
{
  int			i;		// Looping var
  const char		*opt,		// Current option
			*name = NULL,	// System name, if any
			*host = NULL,	// Hostname, if any
			*spool = NULL,	// Spool directory, if any
			*log = NULL,	// Log file, if any
			*auth = NULL;	// Auth service, if any
  int			port = 0;	// Port number, if any
  pappl_loglevel_t	level = PAPPL_LOGLEVEL_DEBUG;
  					// Log level
  bool			clean = false;	// Clean run?
  pappl_soptions_t	soptions = PAPPL_SOPTIONS_ALL;
					// System options
  pappl_system_t	*system;	// System
  pappl_printer_t	*printer;	// Printer
  static pappl_contact_t contact =	// Contact information
  {
    "Michael R Sweet",
    "msweet@example.org",
    "+1-705-555-1212"
  };
  static pappl_version_t versions[1] =	// Software versions
  {
    { "Test System", "", "1.0 build 42", { 1, 0, 0, 42 } }
  };


  // Parse command-line options...
  for (i = 1; i < argc; i ++)
  {
    if (!strcmp(argv[i], "--help"))
    {
      return (usage(0));
    }
    else if (!strcmp(argv[i], "--version"))
    {
      puts(PAPPL_VERSION);
      return (0);
    }
    else if (!strncmp(argv[i], "--", 2))
    {
      printf("testpappl: Unknown option '%s'.\n", argv[i]);
      return (usage(1));
    }
    else if (argv[i][0] == '-')
    {
      for (opt = argv[i] + 1; *opt; opt ++)
      {
        switch (*opt)
        {
          case '1' : // -1 (single queue)
              soptions &= ~PAPPL_SOPTIONS_MULTI_QUEUE;
              break;
          case 'A' : // -A pam-service
              i ++;
              if (i >= argc)
              {
                puts("testpappl: Expected PAM service name after '-A'.");
                return (usage(1));
	      }
	      auth = argv[i];
              break;
          case 'c' : // -c (clean run)
              clean = true;
              break;
          case 'd' : // -d spool-directory
              i ++;
              if (i >= argc)
              {
                puts("testpappl: Expected spool directory after '-d'.");
                return (usage(1));
	      }
	      spool = argv[i];
              break;
          case 'l' : // -l log-file
              i ++;
              if (i >= argc)
              {
                puts("testpappl: Expected log file after '-l'.");
                return (usage(1));
	      }
	      log = argv[i];
              break;
          case 'L' : // -L level
              i ++;
              if (i >= argc)
              {
                puts("testpappl: Expected log level after '-L'.");
                return (usage(1));
	      }

              if (!strcmp(argv[i], "fatal"))
	      {
                level = PAPPL_LOGLEVEL_FATAL;
	      }
	      else if (!strcmp(argv[i], "error"))
	      {
                level = PAPPL_LOGLEVEL_ERROR;
	      }
	      else if (!strcmp(argv[i], "warn"))
	      {
                level = PAPPL_LOGLEVEL_WARN;
	      }
	      else if (!strcmp(argv[i], "info"))
	      {
                level = PAPPL_LOGLEVEL_INFO;
	      }
	      else if (!strcmp(argv[i], "debug"))
	      {
                level = PAPPL_LOGLEVEL_DEBUG;
	      }
	      else
	      {
	        printf("testpappl: Unknown log level '%s'.\n", argv[i]);
	        return (usage(1));
	      }
              break;
          case 'n' : // -n hostname
              i ++;
              if (i >= argc)
              {
                puts("testpappl: Expected hostname after '-n'.");
                return (usage(1));
	      }
	      host = argv[i];
              break;
          case 'p' :
              i ++;
              if (i >= argc || atoi(argv[i]) <= 0 || atoi(argv[i]) > 32767)
              {
                puts("testpappl: Expected port number after '-p'.");
                return (usage(1));
	      }
	      port = atoi(argv[i]);
              break;
	  default :
	      printf("testpappl: Unknown option '-%c'.\n", *opt);
	      return (usage(1));
        }
      }
    }
    else if (name)
    {
      printf("testpappl: Unexpected argument '%s'.\n", argv[i]);
      return (usage(1));
    }
    else
    {
      // "server name"
      name = argv[i];
    }
  }

  // Initialize the system and any printers...
  system = papplSystemCreate(soptions, name ? name : "Test System", host, port, "_print,_universal", spool, log ? log : "-", level, auth, /* tls_only */false);
  papplSystemAddListeners(system, NULL);
  test_setup_drivers(system);
  papplSystemSetFooterHTML(system,
                           "Copyright &copy; 2020 by Michael R Sweet. "
                           "Provided under the terms of the <a href=\"https://www.apache.org/licenses/LICENSE-2.0\">Apache License 2.0</a>.");
  papplSystemSetSaveCallback(system, (pappl_save_cb_t)papplSystemSaveState, (void *)"testpappl.state");
  papplSystemSetVersions(system, (int)(sizeof(versions) / sizeof(versions[0])), versions);

  if (clean || !papplSystemLoadState(system, "testpappl.state"))
  {
    papplSystemSetContact(system, &contact);
    papplSystemSetDNSSDName(system, name ? name : "Test System");
    papplSystemSetGeoLocation(system, "geo:46.4707,-80.9961");
    papplSystemSetLocation(system, "Test Lab 42");
    papplSystemSetOrganization(system, "Lakeside Robotics");

    printer = papplPrinterCreate(system, PAPPL_SERVICE_TYPE_PRINT, /* printer_id */0, "Office Printer", "pwg_common-300dpi-600dpi-srgb_8", "file:///dev/null");
    papplPrinterSetContact(printer, &contact);
    papplPrinterSetDNSSDName(printer, "Office Printer");
    papplPrinterSetGeoLocation(printer, "geo:46.4707,-80.9961");
    papplPrinterSetLocation(printer, "Test Lab 42");
    papplPrinterSetOrganization(printer, "Lakeside Robotics");

    if (soptions & PAPPL_SOPTIONS_MULTI_QUEUE)
    {
      printer = papplPrinterCreate(system, PAPPL_SERVICE_TYPE_PRINT, /* printer_id */0, "Label Printer", "pwg_4inch-203dpi-black_1", "file:///dev/null");
      papplPrinterSetContact(printer, &contact);
      papplPrinterSetDNSSDName(printer, "Label Printer");
      papplPrinterSetGeoLocation(printer, "geo:46.4707,-80.9961");
      papplPrinterSetLocation(printer, "Test Lab 42");
      papplPrinterSetOrganization(printer, "Lakeside Robotics");
    }
  }

  // Run the system...
  papplSystemRun(system);

  return (0);
}


//
// 'usage()' - Show usage.
//

static int				// O - Exit status
usage(int status)			// I - Exit status
{
  puts("Usage: testpappl [options] [\"server name\"]");
  puts("Options:");
  puts("--help               Show help");
  puts("--version            Show version");
  puts("-1                   Single queue");
  puts("-A pam-service       Enable authentication using PAM service");
  puts("-c                   Do a clean run (no loading of state)");
  puts("-d spool-directory   Set the spool directory");
  puts("-l log-file          Set the log file");
  puts("-L level             Set the log level (fatal, error, warn, info, debug)");
  puts("-n hostname          Set the hostname");
  puts("-p port              Set the listen port");

  return (status);
}
