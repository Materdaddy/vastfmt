#include "propertydefs.h"
#include "commanddefs.h"
#include "logging.h"
#include "fmtx.h"
#include "commands.h"

#include <iostream>
#include <vector>
#include <functional> // reference_wrapper

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <math.h>

// Helper that won't allow me to add a second reference to the list.  This is
// useful because I use singletons (for other design reasons) and am avoiding
// multiple instances of configured types.
#define add_to(v, e)					\
	{									\
										\
	bool found = false;					\
	for ( VastCommand& c : commands )	\
		if ( &c == &e )					\
			found = true;				\
	if ( ! found )						\
		v.push_back(e);					\
										\
	}

int impedance = -1;

uint16_t parseInt(const char *PI)
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

uint8_t watts_to_dbuv(double watts)
{
	if ( impedance < 0 )
	{
		logwrite(LOG_ERROR, "Must have '--impedance' argument before argument '--power-watts'");
		exit(EXIT_FAILURE);
	}

    double dBW = 10 * log10(watts);
    double dBm = dBW + 30;
    double p = pow(10, dBm / 10) / 1000;
    double u = sqrt(p * impedance);
    double dBuV = 20 * log10(u * 1000000);
    return dBuV;
}

void usage(char *appname)
{
printf("Usage: %s [OPTION...]\n"
"\n"
"%s is a command line application to control the VastElectonics V-FMT212R\n"
"USB FM transmitter.\n"
"\n"
"Note that currently nothing that writes to EEPROM is supported.  You will likely\n"
"want to configure your transmitter using the Windows app and simply use t his to\n"
"tweak things on the fly, but support for that should be coming soon.\n"
"\n"
"Options:\n"
"\t-t, --transmit\t\tEnable transmitter\n\n"
"\t-n, --no-transmit\tDisable transmitter\n\n"
"\t-f, --tune-frequency\tSet the transmitter's frequency in MHz\n\n"
"\t-s, --rds-station\tSet the call sign of your station (PS)\n\n"
"\t-R, --rds-text\t\tSet the RDS text (RT)\n\n"
// http://www.pira.cz/rds/show.asp?art=rds_encoder_support
//  -> http://www.pira.cz/rds/rtpclass.pdf
"\t-T, --title\t\tSet the title of the track (RT+ code 1)\n\n"
"\t-A, --artist\t\tSet the artist of the track (RT+ code 4)\n\n"
"\t-p, --power\t\tSet the transmit power in dBµV\n\n"
"\t-a, --antenna-cap\tSet the antenna capacitance in pF\n\n"
"\t    --audio-deviation\tSet the audio deviation in Hz\n\n"
"\t-i, --impedance\t\tImpedance of your antenna cable in ohms.  Used in calculation\n"
"\t\t\t\tfor '--power-watts' and must be specified before '--power-watts'\n\n"
"\t    --power-watts\tSet the power in watts unit instead of dBµV.\n"
"\t\t\t\tNOTE: This requires that you specify the impedance of your\n"
"\t\t\t\tcable first with the '--impedance' argument\n\n"
"\t-e, --preemphasisid\tSet the Pre-Emphasis to 50µs (Europe) or 75µs (USA) by\n"
"\t\t\t\tspecifying the string \"USA\" or \"Europe\"\n\n"
"\t-P, --pilot\t\tEnable pilot tone encoding\n\n"
"\t    --no-pilot\t\tDisable pilot tone encoding\n\n"
"\t-D, --pilot-deviation\tSet the pilot deviation in Hz\n\n"
"\t-F, --pilot-frequency\tSet the pilot frequency in Hz\n\n"
"\t-S, --stereo\t\tEnable stereo transmission\n\n"
"\t-M, --mono\t\tDisable stereo transmission\n\n"
"\t    --rds\t\tEnable RDS transmission\n\n"
"\t    --no-rds\t\tDisable RDS transmission\n\n"
"\t-d, --rds-deviation\tSet the RDS deviation\n\n"
"\t    --reset\t\tAttempt to reset the frontend\n\n"
"\t-v, --verbose\t\tTurn up logging level.  Can be specified more than once for more logging\n\n"
"\t-h, --help\t\tDisplay this help\n"
,
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

	std::vector<std::reference_wrapper<VastCommand>> commands;

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
			{"rds-text",			required_argument,	0, 'R'},
			{"title",				required_argument,	0, 'T'},
			{"artist",				required_argument,	0, 'A'},
			{"power",				required_argument,	0, 'p'},
			{"impedance",			required_argument,	0, 'i'},
			{"power-watts",			required_argument,	0,  4 },
			{"antenna-cap",			required_argument,	0, 'a'},
			{"audio-deviation",		required_argument,	0,  6 },
			{"preemphasisid",		required_argument,	0, 'e'},
			{"pilot",				no_argument,		0, 'P'},
			{"no-pilot",			no_argument,		0,  1 },
			{"pilot-deviation",		required_argument,	0, 'D'},
			{"pilot-frequency",		required_argument,	0, 'F'},
			{"stereo",				no_argument,		0, 'S'},
			{"mono",				no_argument,		0, 'M'},
			{"rds",					no_argument,		0,  3 },
			{"no-rds",				no_argument,		0,  2 },
			{"rds-deviation",		required_argument,	0, 'd'},
			{"reset",				no_argument,		0,  5 },
			{"verbose",				no_argument,		0, 'v'},
			{"help",				no_argument,		0, 'h'},
			{0,						0,					0,  0 }
		};

		c = getopt_long(argc, argv, "tnf:s:R:T:A:p:i:a:e:PD:F:SMd:vh",
		long_options, &option_index);
		if (c == -1)
			break;

		switch (c)
		{
			case 't':
			{
				VastTransmission& xmit = VastTransmission::get();
				xmit = true;
				add_to(commands, xmit);

				logwrite(LOG_INFO, "Enabled transmission");
				break;
			}
			case 'n':
			{
				VastTransmission& xmit = VastTransmission::get();
				xmit = false;
				add_to(commands, xmit);

				logwrite(LOG_INFO, "Diabled transmission");
				break;
			}
			case 'f': // tune-frequency
			{
				VastFrequency& freq = VastFrequency::get();
				freq = strtod(optarg, NULL);
				add_to(commands, freq);

				logwrite(LOG_INFO, "Set tune frequency to %f", strtod(optarg, NULL));
				break;
			}
			case 's': // rds-station
			{
				VastPsMessage& psm = VastPsMessage::get();
				std::string psmessage = optarg;
				psm = psmessage;
				add_to(commands, psm);

				logwrite(LOG_INFO, "Set Station to %s", optarg);

				VastRdsPI& rdspi = VastRdsPI::get();
				rdspi = call_to_pi(optarg);
				add_to(commands, rdspi);

				logwrite(LOG_INFO, "Set RDS PI to %ld", call_to_pi(optarg));
				break;
			}
			case 'R': // rds-text
			{
				VastRDSRtText& rds = VastRDSRtText::get();
				rds = optarg;
				add_to(commands, rds);

				logwrite(LOG_INFO, "Set RDS RT to %s", optarg);
				break;
			}
			case 'T': // title
			{
				VastRDSRtText& rds = VastRDSRtText::get();
				std::string title = optarg;
				rds.setTitle(title);
				add_to(commands, rds);

				logwrite(LOG_INFO, "Set RDS RT+ code 1 (ITEM.TITLE) to %s", optarg);
				break;
			}
			case 'A': // artist
			{
				VastRDSRtText& rds = VastRDSRtText::get();
				std::string artist = optarg;
				rds.setArtist(artist);
				add_to(commands, rds);

				logwrite(LOG_INFO, "Set RDS RT+ code 4 (ITEM.ARTIST) to %s", optarg);
				break;
			}
			case 'p': // power
			{
				if ( atoi(optarg) < 88 )
				{
					VastPowerAntennaCap& pwr = VastPowerAntennaCap::get();
					pwr.setPower(88);
					add_to(commands, pwr);

					logwrite(LOG_WARN, "Power set too low, upping to 88dBµV");
				}
				else if ( atoi(optarg) > 120 )
				{
					VastPowerAntennaCap& pwr = VastPowerAntennaCap::get();
					pwr.setPower(120);
					add_to(commands, pwr);

					logwrite(LOG_WARN, "Power set too high, lowering to 120dBµV");
				}
				else
				{
					VastPowerAntennaCap& pwr = VastPowerAntennaCap::get();
					pwr.setPower(atoi(optarg));
					add_to(commands, pwr);

					logwrite(LOG_INFO, "Set transmit power to %hhddBµV", atoi(optarg));
				}

				break;
			}
			case 'a': // antenna-cap
			{
				VastPowerAntennaCap& cap = VastPowerAntennaCap::get();
				cap.setAntennaCap(strtod(optarg, NULL));
				add_to(commands, cap);

				logwrite(LOG_INFO, "Set antenna capacitor to %f", strtod(optarg, NULL));
				break;
			}
			case 6:
			{
				VastAudioDeviation& aDev = VastAudioDeviation::get();
				aDev = atoi(optarg);
				add_to(commands, aDev);

				logwrite(LOG_INFO, "Set audio deviation to %d", atoi(optarg));
				break;
			}
			case 'e': // preemphasisid
			{
				if ( strcmp(optarg, "USA") == 0 )
				{
					VastPreemphasisId& pre = VastPreemphasisId::get();
					pre = 0;
					add_to(commands, pre);

					logwrite(LOG_INFO, "Setting Preemphasis ID to 0 for USA");
				}
				else if ( strcmp(optarg, "Europe") == 0 )
				{
					VastPreemphasisId& pre = VastPreemphasisId::get();
					pre = 1;
					add_to(commands, pre);

					logwrite(LOG_INFO, "Setting Preemphasis ID to 1 for Europe");
				}
				else
				{
					logwrite(LOG_ERROR, "Couldn't determine Preemphasis ID");
					usage(argv[0]);
					exit(EXIT_FAILURE);
				}
				break;
			}
			case 'P': // pilot
			{
				VastPilotLmrRds& pilot = VastPilotLmrRds::get();
				pilot.setPilot(true);
				add_to(commands, pilot);

				logwrite(LOG_INFO, "Enabling pilot tone");
				break;
			}
			case  1:  // no-pilot
			{
				VastPilotLmrRds& pilot = VastPilotLmrRds::get();
				pilot.setPilot(false);
				add_to(commands, pilot);

				logwrite(LOG_INFO, "Disabling pilot tone");
				break;
			}
			case 'D': // pilot-deviation
			{
				VastPilotDeviation& pdev = VastPilotDeviation::get();
				pdev = atoi(optarg);
				add_to(commands, pdev);

				logwrite(LOG_INFO, "Setting pilot deviation to %d Hz", atoi(optarg));
				break;
			}
			case 'F': // pilot-frequency
			{
				VastPilotFrequency& pfreq = VastPilotFrequency::get();
				pfreq = atoi(optarg);
				add_to(commands, pfreq);

				logwrite(LOG_INFO, "Setting pilot frequency to %hd Hz", atoi(optarg));
				break;
			}
			case 'S': // stereo
			{
				VastPilotLmrRds& lmr = VastPilotLmrRds::get();
				lmr.setStereo(true);
				add_to(commands, lmr);

				logwrite(LOG_INFO, "Enabling stereo (Disabling mono)");
				break;
			}
			case 'M': // mono
			{
				VastPilotLmrRds& lmr = VastPilotLmrRds::get();
				lmr.setStereo(false);
				add_to(commands, lmr);

				logwrite(LOG_INFO, "Enabling mono (Disabling stereo)");
				break;
			}
			case 'i': // impedance
			{
				impedance = atoi(optarg);

				logwrite(LOG_INFO, "Setting impedance of antenna/cable to %uohms", impedance);
				break;
			}
			case  4: // power-watts
			{
				uint8_t power = watts_to_dbuv(strtod(optarg, NULL));
				if ( power < 88 )
				{
					logwrite(LOG_ERROR, "Power set too low, upping to 88dBµV");
					power = 88;
				}
				if ( power > 120 )
				{
					logwrite(LOG_ERROR, "Power set too high, lowering to 120dBµV");
					power = 120;
				}

				VastPowerAntennaCap& pwr = VastPowerAntennaCap::get();
				pwr.setPower(power);
				add_to(commands, pwr);

				logwrite(LOG_INFO, "Set transmit power to %hhddBµV", power);
				break;
			}

			case  5: // reset
			{
				VastReset& reset = VastReset::get();
				add_to(commands, reset);

				logwrite(LOG_INFO, "Resetting FE");
				break;
			}
			case  3: // rds
			{
				VastPilotLmrRds& rds = VastPilotLmrRds::get();
				rds.setRds(true);
				add_to(commands, rds);

				logwrite(LOG_INFO, "Enabling RDS");
				break;
			}
			case  2:  // no-rds
			{
				VastPilotLmrRds& rds = VastPilotLmrRds::get();
				rds.setRds(false);
				add_to(commands, rds);

				logwrite(LOG_INFO, "Disabling RDS");
				break;
			}
			case 'd': // rds-deviation
			{
				VastRdsDeviation& rdev = VastRdsDeviation::get();
				rdev = atoi(optarg);
				add_to(commands, rdev);
				logwrite(LOG_INFO, "Set RDS deviation to %d", atoi(optarg));
				break;
			}
			case 'v': // verbose
			{
				logwrite(loglevel, "Upping verbosity");
				if ( loglevel < 5 )
					loglevel++;
				break;
			}
			case 'h': //help
			{
				usage(argv[0]);
				exit(EXIT_SUCCESS);
				break;
			}
			default:
			{
				usage(argv[0]);
				exit(EXIT_FAILURE);
			}
		}
	}

	if ( commands.size() == 0 )
	{
		logwrite(LOG_INFO, "Nothing to do!");
		exit(EXIT_SUCCESS);
	}

	ret = (FMTX_MODE_ENUM) fmtxIoAppIdString(cpuid, rev);
	if (ret != FMTX_MODE_OK)
	{
		logwrite(LOG_ERROR,"Can't get id string!");
		exit(EXIT_FAILURE);
	}
	logwrite(LOG_INFO,"Got CPU ID: %s rev.: %s", cpuid, rev);

	for ( VastCommand& cmd : commands )
	{
		cmd.printParameters();
		cmd.saveParametersToVast();
	}

	return 0;
}
