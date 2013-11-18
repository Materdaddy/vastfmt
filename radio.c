#include "propertydefs.h"
#include "commanddefs.h"
#include "logging.h"
#include "fmtx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <errno.h>
#include <string.h>
#include <getopt.h>

typedef struct settings_s
{
	bool		transmit;			// Whether we're transmitting or not
	double		tuneFrequency;		// Frequency we'll modulate to
	uint16_t	rdsPI;				// PI value (calculated from callsign
	uint8_t		rdsLineId;			// ID of which rdsLineText is in use
	uint8_t		rdsLineCount;		// Count of rdsLineText messages allocated
	char *		rdsLineText[12];	// RDS PS Message (Station information)
	char *		rtMessage;			// RDS RT Message (Artist/Song information)
	uint8_t		power;				// Transmit power (in dBµV) (88-120)
	double		antennaCap;			// Antenna Capacitor
	uint8_t		preemphasisId;		// PreemphasisID (0 = 75µs USA; 1 = 50µs Europe)
	bool		pilot;				// 
	uint32_t	pilotDeviation;		// 
	uint16_t	pilotFrequency;		// 
	bool		lmr;				// 
	bool		rds;				// 
	uint32_t	rdsDeviation;		// 
} settings_t;

settings_t settings = { 0 };

uint16_t parseInt(char *PI)
{
	uint16_t pi_val = 0;

    sscanf(PI, "%x%x", &pi_val, (&pi_val+1));

	return pi_val;
}

// Based on code found at:
// http://www.w9wi.com/articles/rdsreverse.htm
uint16_t call_to_pi(char *call)
{
	uint16_t picode = 0;
	int i = 0;

	// Convert string to upper case
	for ( i = 0; i < strlen(call); ++i )
		call[i] = toupper(call[i]);

	if ( strlen(call) == 4 )
	{
		int letter1 = call[0]-65;
		int letter2 = call[1]-65;
		int letter3 = call[2]-65;
		int letter4 = call[3]-65;

		picode = (26 * 26 * letter2) + (26 * letter3) + letter4;
		if ( letter1 == 10 ) {
			picode = picode + 4096;
		} else {
			picode = picode + 21672;
		}
	}
	else if ( strlen(call) == 3 )
	{
		if ( strcmp("KBW", call) == 0 )
			picode = parseInt("99A5");
		if ( strcmp("KCY", call) == 0 )
			picode = parseInt("99A6");
		if ( strcmp("KDB", call) == 0 )
			picode = parseInt("9990");
		if ( strcmp("KDF", call) == 0 )
			picode = parseInt("99A7");
		if ( strcmp("KEX", call) == 0 )
			picode = parseInt("9950");
		if ( strcmp("KFH", call) == 0 )
			picode = parseInt("9951");
		if ( strcmp("KFI", call) == 0 )
			picode = parseInt("9952");
		if ( strcmp("KGA", call) == 0 )
			picode = parseInt("9953");
		if ( strcmp("KGB", call) == 0 )
			picode = parseInt("9991");
		if ( strcmp("KGO", call) == 0 )
			picode = parseInt("9954");
		if ( strcmp("KGU", call) == 0 )
			picode = parseInt("9955");
		if ( strcmp("KGW", call) == 0 )
			picode = parseInt("9956");
		if ( strcmp("KGY", call) == 0 )
			picode = parseInt("9957");
		if ( strcmp("KHQ", call) == 0 )
			picode = parseInt("99AA");
		if ( strcmp("KID", call) == 0 )
			picode = parseInt("9958");
		if ( strcmp("KIT", call) == 0 )
			picode = parseInt("9959");
		if ( strcmp("KJR", call) == 0 )
			picode = parseInt("995A");
		if ( strcmp("KLO", call) == 0 )
			picode = parseInt("995B");
		if ( strcmp("KLZ", call) == 0 )
			picode = parseInt("995C");
		if ( strcmp("KMA", call) == 0 )
			picode = parseInt("995D");
		if ( strcmp("KMJ", call) == 0 )
			picode = parseInt("995E");
		if ( strcmp("KNX", call) == 0 )
			picode = parseInt("995F");
		if ( strcmp("KOA", call) == 0 )
			picode = parseInt("9960");
		if ( strcmp("KOB", call) == 0 )
			picode = parseInt("99AB");
		if ( strcmp("KOY", call) == 0 )
			picode = parseInt("9992");
		if ( strcmp("KPQ", call) == 0 )
			picode = parseInt("9993");
		if ( strcmp("KQV", call) == 0 )
			picode = parseInt("9964");
		if ( strcmp("KSD", call) == 0 )
			picode = parseInt("9994");
		if ( strcmp("KSL", call) == 0 )
			picode = parseInt("9965");
		if ( strcmp("KUJ", call) == 0 )
			picode = parseInt("9966");
		if ( strcmp("KUT", call) == 0 )
			picode = parseInt("9995");
		if ( strcmp("KVI", call) == 0 )
			picode = parseInt("9967");
		if ( strcmp("KWG", call) == 0 )
			picode = parseInt("9968");
		if ( strcmp("KXL", call) == 0 )
			picode = parseInt("9996");
		if ( strcmp("KXO", call) == 0 )
			picode = parseInt("9997");
		if ( strcmp("KYW", call) == 0 )
			picode = parseInt("996B");
		if ( strcmp("WBT", call) == 0 )
			picode = parseInt("9999");
		if ( strcmp("WBZ", call) == 0 )
			picode = parseInt("996D");
		if ( strcmp("WDZ", call) == 0 )
			picode = parseInt("996E");
		if ( strcmp("WEW", call) == 0 )
			picode = parseInt("996F");
		if ( strcmp("WGH", call) == 0 )
			picode = parseInt("999A");
		if ( strcmp("WGL", call) == 0 )
			picode = parseInt("9971");
		if ( strcmp("WGN", call) == 0 )
			picode = parseInt("9972");
		if ( strcmp("WGR", call) == 0 )
			picode = parseInt("9973");
		if ( strcmp("WGY", call) == 0 )
			picode = parseInt("999B");
		if ( strcmp("WHA", call) == 0 )
			picode = parseInt("9975");
		if ( strcmp("WHB", call) == 0 )
			picode = parseInt("9976");
		if ( strcmp("WHK", call) == 0 )
			picode = parseInt("9977");
		if ( strcmp("WHO", call) == 0 )
			picode = parseInt("9978");
		if ( strcmp("WHP", call) == 0 )
			picode = parseInt("999C");
		if ( strcmp("WIL", call) == 0 )
			picode = parseInt("999D");
		if ( strcmp("WIP", call) == 0 )
			picode = parseInt("997A");
		if ( strcmp("WIS", call) == 0 )
			picode = parseInt("99B3");
		if ( strcmp("WJR", call) == 0 )
			picode = parseInt("997B");
		if ( strcmp("WJW", call) == 0 )
			picode = parseInt("99B4");
		if ( strcmp("WJZ", call) == 0 )
			picode = parseInt("99B5");
		if ( strcmp("WKY", call) == 0 )
			picode = parseInt("997C");
		if ( strcmp("WLS", call) == 0 )
			picode = parseInt("997D");
		if ( strcmp("WLW", call) == 0 )
			picode = parseInt("997E");
		if ( strcmp("WMC", call) == 0 )
			picode = parseInt("999E");
		if ( strcmp("WMT", call) == 0 )
			picode = parseInt("999F");
		if ( strcmp("WOC", call) == 0 )
			picode = parseInt("9981");
		if ( strcmp("WOI", call) == 0 )
			picode = parseInt("99A0");
		if ( strcmp("WOL", call) == 0 )
			picode = parseInt("9983");
		if ( strcmp("WOR", call) == 0 )
			picode = parseInt("9984");
		if ( strcmp("WOW", call) == 0 )
			picode = parseInt("99A1");
		if ( strcmp("WRC", call) == 0 )
			picode = parseInt("99B9");
		if ( strcmp("WRR", call) == 0 )
			picode = parseInt("99A2");
		if ( strcmp("WSB", call) == 0 )
			picode = parseInt("99A3");
		if ( strcmp("WSM", call) == 0 )
			picode = parseInt("99A4");
		if ( strcmp("WWJ", call) == 0 )
			picode = parseInt("9988");
		if ( strcmp("WWL", call) == 0 )
			picode = parseInt("9989");
	}

	return picode;
}

void usage(char *appname)
{
printf("Usage: %s [OPTION...]\n"
"\n"
"%s is a command line application to control the VastElectonics V-FMT212R USB FM\n"
"transmitter.\n"
"\n"
"Note that currently nothing that writes to EEPROM is supported.  You will likely\n"
"want to configure your transmitter using the Windows app and simply use t his to\n"
"tweak things on the fly, but support for that should be coming soon.\n"
"\n"
"Options:\n"
"\t-t, --transmit\t\tEnable transmitter\n"
"\t-n, --no-transmit\tDisable transmitter\n"
"\t-f, --tune-frequency\tSet the transmitter's frequency in MHz\n"
"\t-s, --rds-station\t\tSet the call sign of your station (Text)\n"
"\t-T, --rds-text\t\tSet the RDS text (Text)\n"
"\t-p, --power\t\tSet the transmit power in dBµV\n"
"\t-a, --antenna-cap\t\tSet the antenna capacitance in pF\n"
"\t-e, --preemphasisid\tSet the Pre-Emphasis to 50µs (Europe) or 75µs (USA) by\n"
"\t\t\t\tspecifying the string \"USA\" or \"Europe\"\n"
"\t-P, --pilot\t\tEnable pilot tone encoding\n"
"\t    --no-pilot\t\tDisable pilot tone encoding\n"
"\t-D, --pilot-deviation\tSet the pilot deviation in Hz\n"
"\t-F, --pilot-frequency\tSet the pilot frequency in Hz\n"
"\t-S, --stereo\t\tEnable stereo transmission\n"
"\t-M, --mono\t\tDisable stereo transmission\n"
"\t-R, --rds\t\t\tEnable RDS transmission\n"
"\t    --no-rds\t\tDisable RDS transmission\n"
"\t-d, --rds-deviation\tSet the RDS deviation\n"
"\t-v, --verbose\t\tTurn up logging level.  Can be specified more than once for more logging\n",
	appname,
	appname);
}


int main(int argc, char *argv[])
{

	FMTX_MODE_ENUM ret = FMTX_MODE_OK;
    fmtxCmdSpecial = FMTX_SPECIAL_FRONTEND; // We want to work with frontend to set frequency
	loglevel = LOG_ERROR; // Turn down our logging
    char cpuid[32];
    char rev[32];
	int c;

    cpuid[0] = '\0';
    rev[0] = '\0';

	//Set defaults

	settings.transmit = true;						// Transmit by default
	settings.tuneFrequency = 87.9;					// in MHz
    sscanf("40A7", "%x%x", &settings.rdsPI, (&settings.rdsPI+1)); // 40A7 in hex
	settings.power = 88;							// in dBµV
	settings.antennaCap = 7.5;						// in pF
	settings.pilot = true;							// Enabled by default
	settings.pilotDeviation = 6750;					// in Hz
	settings.pilotFrequency = 19000;				// in Hz
	settings.lmr = true;							// Left Minus Right (Stereo)
	settings.rds = true;							// Enabled by default
	settings.rdsDeviation = 2000;					// in Hz

	while (1)
	{
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		static struct option long_options[] =
		{
			{"transmit",			no_argument,		0, 't'},
			{"no-transmit",			no_argument,		0, 'n'},
			{"tune-frequency",		required_argument,	0, 'f'},
			{"rds-station",			required_argument,	0, 's'},
			{"rds-text",			required_argument,	0, 'T'},
			{"power",				required_argument,	0, 'p'},
			{"antenna-cap",			required_argument,	0, 'a'},
			{"preemphasisid",		required_argument,	0, 'e'},
			{"pilot",				no_argument,		0, 'P'},
			{"no-pilot",			no_argument,		0,  1 },
			{"pilot-deviation",		required_argument,	0, 'D'},
			{"pilot-frequency",		required_argument,	0, 'F'},
			{"stereo",				no_argument,		0, 'S'},
			{"mono",				no_argument,		0, 'M'},
			{"rds",					no_argument,		0, 'R'},
			{"no-rds",				no_argument,		0,  2 },
			{"rds-deviation",		required_argument,	0, 'd'},
			{"verbose",				no_argument,		0, 'v'},
			{0,						0,					0,	0 }
		};

		c = getopt_long(argc, argv, "tnf:s:T:p:a:e:PD:F:SMRd:v",
		long_options, &option_index);
		if (c == -1)
			break;

		switch (c)
		{
			case 't':
				settings.transmit = true;
				logwrite(LOG_INFO, "Enabled transmission");
				break;
			case 'n':
				settings.transmit = false;
				logwrite(LOG_INFO, "Diabled transmission");
				break;
			case 'f': // tune-frequency
				settings.tuneFrequency = strtod(optarg, NULL);
				logwrite(LOG_INFO, "Set tune frequency to %f", settings.tuneFrequency);
				break;
			case 's': // rds-station
				if ( settings.rdsLineCount >= 12 )
				{
					logwrite(LOG_ERROR, "You already have 12 RDS line text strings");
					continue;
				}

				if ( settings.rdsLineText[settings.rdsLineCount] );
					free(settings.rdsLineText[settings.rdsLineCount]);

				settings.rdsLineText[settings.rdsLineCount] = strdup(optarg);
				logwrite(LOG_INFO, "Set Station to %s",
					settings.rdsLineText[settings.rdsLineCount]);

				settings.rdsLineCount++;

				//TODO: Calculate PI
    			//sscanf("0xHEX", "%x%x", &settings.rdsPI, (&settings.rdsPI+1));
				//logwrite(LOG_INFO, "Set RDS PI to %ld", settings.rdsPI);
				break;
			case 'T': // rds-text
				free(settings.rtMessage);
				settings.rtMessage = strdup(optarg);
				logwrite(LOG_INFO, "Set RDS Text to %s", settings.rtMessage);
				break;
			case 'p': // power
				settings.power = atoi(optarg);
				if ( settings.power < 88 )
				{
					logwrite(LOG_ERROR, "Power set too low, upping to 88");
					settings.power = 88;
				}
				if ( settings.power > 120 )
				{
					logwrite(LOG_ERROR, "Power set too high, lowering to 120");
					settings.power = 120;
				}
				logwrite(LOG_INFO, "Set transmit power to %lld", settings.power);
				break;
			case 'a': // antenna-cap
				settings.antennaCap = strtod(optarg, NULL);
				logwrite(LOG_INFO, "Set antenna capacitor to %f", settings.antennaCap);
				break;
			case 'e': // preemphasisid
				if ( strcmp(optarg, "USA") == 0 )
				{
					logwrite(LOG_INFO, "Setting Preemphasis ID to 0 for USA");
					settings.preemphasisId = 0;
				}
				else if ( strcmp(optarg, "Europe") == 0 )
				{
					logwrite(LOG_INFO, "Setting Preemphasis ID to 1 for Europe");
					settings.preemphasisId = 1;
				}
				else
				{
					logwrite(LOG_INFO, "Couldn't determine Preemphasis ID");
				}
				break;
			case 'P': // pilot
				logwrite(LOG_INFO, "Enabling pilot tone");
				settings.pilot = true;
				break;
			case  1:  // no-pilot
				logwrite(LOG_INFO, "Disabling pilot tone");
				settings.pilot = false;
				break;
			case 'D': // pilot-deviation
				settings.pilotDeviation = atoi(optarg);
				logwrite(LOG_INFO, "Setting pilot deviation to %d Hz", settings.pilotDeviation);
				break;
			case 'F': // pilot-frequency
				settings.pilotFrequency = atoi(optarg);
				logwrite(LOG_INFO, "Setting pilot frequency to %hd Hz", settings.pilotFrequency);
				break;
			case 'S': // stereo
				settings.lmr = true;
				logwrite(LOG_INFO, "Enabling stereo (Disabling mono)");
				break;
			case 'M': // mono
				settings.lmr = false;
				logwrite(LOG_INFO, "Enabling mono (Disabling stereo)");
				break;
			case 'R': // rds
				settings.rds = true;
				logwrite(LOG_INFO, "Enabling RDS");
				break;
			case  2:  // no-rds
				settings.rds = false;
				logwrite(LOG_INFO, "Disabling RDS");
				break;
			case 'd': // rds-deviation
				settings.rdsDeviation = atoi(optarg);
				logwrite(LOG_INFO, "Set RDS deviation to %d", settings.rdsDeviation);
				break;
			case 'v': // verbose
				logwrite(loglevel, "Upping verbosity");
				if ( loglevel < 5 )
					loglevel++;
				break;
			case 'h': //help
				usage(argv[0]);
				exit(EXIT_SUCCESS);
				break;
			default:
				usage(argv[0]);
				exit(EXIT_FAILURE);
		}
	}


	ret = fmtxIoAppIdString(cpuid, rev);
    if (ret != FMTX_MODE_OK)
	{
        logwrite(LOG_ERROR,"Can't get id string!");
        exit(EXIT_FAILURE);
    }
    logwrite(LOG_INFO,"Got CPU ID: %s rev.: %s", cpuid, rev);


	ret = fmtxTransmitterSetTuneFreq(settings.tuneFrequency);
    if (ret != FMTX_MODE_OK)
	{
		logwrite(LOG_ERROR, "Couldn't set freq to %d", settings.tuneFrequency);
		exit(EXIT_FAILURE);
	}


	if (settings.transmit)
	{
		// Turn on the front end
		ret = fmtxIoAppFeUp();
	    switch(ret)
		{
		    case FMTX_MODE_POWER_UP:
		        logwrite(LOG_INFO, "Frontend powered up!");
		        fmtxCmdStatus=FMTX_MODE_OK;
		        break;
		    case FMTX_MODE_TRANSMITTING:
		        logwrite(LOG_INFO, "Frontend already powered up!");
		        fmtxCmdStatus=FMTX_MODE_OK;
		        break;
		    default:
		        logwrite(LOG_ERROR, "Error while powering up");
		        fmtxCmdStatus=FMTX_MODE_NONE;
				exit(EXIT_FAILURE);
		        break;
	    }
	}
	else
	{
		// Turn off the front end
		ret = fmtxIoAppFeDown();
	    if (ret != FMTX_MODE_POWER_DOWN)
		{
	        logwrite(LOG_ERROR,"error while powering down");
	    }

		//Turning off, nothing else to do here!
		return 0;
	}


	ret = fmtxRDSSetPI(settings.rdsPI);
    if (ret != FMTX_MODE_OK)
	{
        logwrite(LOG_ERROR, "Can't set RDS AF");
    }


	// iVal TODO
	//ret = fmtxRDSSetPsMessageCount(iVal);
	ret = fmtxRDSSetPsMessageCount(settings.rdsLineId);
    if (ret != FMTX_MODE_OK)
	{
        logwrite(LOG_ERROR, "Can't set RDS PS Number");
    }


	ret = fmtxRDSSetPsMessageById(settings.rdsLineId,
			settings.rdsLineText[settings.rdsLineId]);
    if (ret != FMTX_MODE_OK)
	{
        logwrite(LOG_ERROR, "Can't set RDS PS Message");
    }


	ret = fmtxRDSSetRtMessage(settings.rtMessage);
    if (ret != FMTX_MODE_OK)
	{
        logwrite(LOG_ERROR, "Can't set RDS RT Message");
    }


	ret = fmtxTransmitterSetTunePower(settings.power, settings.antennaCap);
    if (ret != FMTX_MODE_OK)
	{
        logwrite(LOG_ERROR, "Can't set transmitter power and capacitor");
    }


	ret = fmtxTransmitterSetPreemphasisId(settings.preemphasisId);
    if(ret != FMTX_MODE_OK){
        logwrite(LOG_ERROR, "Can't set transmitter preemphasis flag");
    }


    uint8_t ComponentFlags = 0;
    if(settings.lmr)
        ComponentFlags |= TX_COMPONENT_ENABLE_LMR_MASK;
    if(settings.pilot)
        ComponentFlags |= TX_COMPONENT_ENABLE_PILOT_MASK;
    if(settings.rds)
        ComponentFlags |= TX_COMPONENT_ENABLE_RDS_MASK;
	ret = fmtxTransmitterSetComponentFlags(ComponentFlags);
    if (ret != FMTX_MODE_OK)
	{
        logwrite(LOG_ERROR, "Can't set transmitter components flag");
    }


	ret = fmtxTransmitterSetPilotDeviation(settings.pilotDeviation);
    if (ret != FMTX_MODE_OK)
	{
        logwrite(LOG_ERROR, "Can't set transmitter pilot deviation flag");
    }

	ret = fmtxTransmitterSetPilotFrequency(settings.pilotFrequency);
    if (ret != FMTX_MODE_OK)
	{
        logwrite(LOG_ERROR, "Can't set transmitter pilot frequency flag");
    }

	ret = fmtxTransmitterSetRDSDeviation(settings.rdsDeviation);
    if (ret != FMTX_MODE_OK)
	{
        logwrite(LOG_ERROR, "Can't set transmitter audio deviation flag");
    }

	return 0;
}
