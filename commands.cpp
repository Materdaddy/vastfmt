#include "commands.h"
#include "fmtx.h"
#include "logging.h"

#include "propertydefs.h"

using namespace std;


VastCommand::VastCommand()
{
}

VastCommand::~VastCommand()
{
}

void VastCommand::printParameters()
{
	cerr << "Unimplimented print for your class!" << endl;
}

bool VastCommand::getParametersFromVast()
{
	cerr << "Unimplemented getParametersFromVast for your class!" << endl;
	return false;
}

bool VastCommand::saveParametersToVast()
{
	cerr << "Unimplemented saveParametersToVast for your class!" << endl;
	return false;
}


/*
 *
 */
VastPowerAntennaCap::VastPowerAntennaCap()
{
}

VastPowerAntennaCap::~VastPowerAntennaCap()
{
}

VastPowerAntennaCap& VastPowerAntennaCap::get()
{
	static VastPowerAntennaCap instance;
	return instance;
}

void VastPowerAntennaCap::setPower(uint8_t power)
{
	mInitialized = true;
	mPowerSet = true;
	mPower = power;
}

void VastPowerAntennaCap::setAntennaCap(double antennaCap)
{
	mInitialized = true;
	mAntennaCapSet = true;
	mAntennaCap = antennaCap;
}

void VastPowerAntennaCap::printParameters()
{
	if ( mInitialized )
	{
		if ( mPowerSet )
			cout << "Power: " << static_cast<int>(mPower) << endl;
		if ( mAntennaCapSet )
			cout << "Antenna Cap: " << mAntennaCap << endl;
	}
	else
		cerr << "Uninitialized!\n";
}

bool VastPowerAntennaCap::saveParametersToVast()
{
	if ( !mInitialized )
	{
		cerr << "Uninitialized!\n";
		return false;
	}

	FMTX_MODE_ENUM ret = FMTX_MODE_OK;

	if ( ! mPowerSet && ! mAntennaCapSet )
	{
		double tmp_freq;
		uint8_t power;
		double antennaCap;

		ret = (FMTX_MODE_ENUM) fmtxTransmitterGetTuneStatus(&tmp_freq, &power, &antennaCap);
		if (ret != FMTX_MODE_OK)
		{
			logwrite(LOG_ERROR, "Can't get transmitter power and capacitor");
			exit(EXIT_FAILURE);
		}

		ret = (FMTX_MODE_ENUM) fmtxTransmitterSetTunePower(power, antennaCap);
		if (ret != FMTX_MODE_OK)
		{
			logwrite(LOG_ERROR, "Can't set transmitter power and capacitor");
			exit(EXIT_FAILURE);
		}
	}

	if ( ! mPowerSet && mAntennaCapSet )
	{
		double tmp_freq;
		uint8_t power;
		double tmp_antennaCap;

		ret = (FMTX_MODE_ENUM) fmtxTransmitterGetTuneStatus(&tmp_freq, &power, &tmp_antennaCap);
		if (ret != FMTX_MODE_OK)
		{
			logwrite(LOG_ERROR, "Can't get transmitter power and capacitor");
			exit(EXIT_FAILURE);
		}

		ret = (FMTX_MODE_ENUM) fmtxTransmitterSetTunePower(power, mAntennaCap);
		if (ret != FMTX_MODE_OK)
		{
			logwrite(LOG_ERROR, "Can't set transmitter power and capacitor");
			exit(EXIT_FAILURE);
		}
	}

	if ( mPowerSet && ! mAntennaCapSet )
	{
		double tmp_freq;
		uint8_t tmp_power;
		double antennaCap;

		ret = (FMTX_MODE_ENUM) fmtxTransmitterGetTuneStatus(&tmp_freq, &tmp_power, &antennaCap);
		if (ret != FMTX_MODE_OK)
		{
			logwrite(LOG_ERROR, "Can't get transmitter power and capacitor");
			exit(EXIT_FAILURE);
		}

		ret = (FMTX_MODE_ENUM) fmtxTransmitterSetTunePower(mPower, antennaCap);
		if (ret != FMTX_MODE_OK)
		{
			logwrite(LOG_ERROR, "Can't set transmitter power and capacitor");
			exit(EXIT_FAILURE);
		}
	}

	if ( mPowerSet && mAntennaCapSet )
	{
		ret = (FMTX_MODE_ENUM) fmtxTransmitterSetTunePower(mPower, mAntennaCap);
		if (ret != FMTX_MODE_OK)
		{
			logwrite(LOG_ERROR, "Can't set transmitter power and capacitor");
			exit(EXIT_FAILURE);
		}
	}

	return true;
}

/*
 *
 */
VastTransmission::VastTransmission()
{
}

VastTransmission::~VastTransmission()
{
}

VastTransmission& VastTransmission::get()
{
	static VastTransmission instance;
	return instance;
}

VastTransmission& VastTransmission::operator=(bool transmit)
{
	mInitialized = true;
	mTransmit = transmit;
}

void VastTransmission::printParameters()
{
	if ( mInitialized )
		cout << "Transmission: " << (mTransmit ? "on" : "off") << endl;
	else
		cerr << "Uninitialized!\n";
}

bool VastTransmission::saveParametersToVast()
{
	if ( !mInitialized )
	{
		cerr << "Uninitialized!\n";
		return false;
	}

	FMTX_MODE_ENUM ret = FMTX_MODE_OK;

	if (mTransmit)
	{
		// Turn on the front end
		ret = (FMTX_MODE_ENUM) fmtxIoAppFeUp();
		switch(ret)
		{
			case FMTX_MODE_POWER_UP:
			{
				logwrite(LOG_INFO, "Frontend powered up!");
				fmtxCmdStatus=FMTX_MODE_OK;
				break;
			}
			case FMTX_MODE_TRANSMITTING:
			{
				logwrite(LOG_INFO, "Frontend already powered up!");
				fmtxCmdStatus=FMTX_MODE_OK;
				break;
			}
			default:
			{
				logwrite(LOG_ERROR, "Error while powering up");
				fmtxCmdStatus=FMTX_MODE_NONE;
				exit(EXIT_FAILURE);
				break;
			}
		}
	}
	else
	{
		// Turn off the front end
		ret = (FMTX_MODE_ENUM) fmtxIoAppFeDown();
		if (ret != FMTX_MODE_POWER_DOWN)
		{
			logwrite(LOG_ERROR,"error while powering down");
			exit(EXIT_FAILURE);
		}
		exit(EXIT_SUCCESS);
	}

	return true;
}


/*
 *
 */
VastFrequency::VastFrequency()
{
}

VastFrequency::~VastFrequency()
{
}

VastFrequency& VastFrequency::get()
{
	static VastFrequency instance;
	return instance;
}

VastFrequency& VastFrequency::operator=(double frequency)
{
	mInitialized = true;
	mFrequency = frequency;
}

void VastFrequency::printParameters()
{
	if ( mInitialized )
		cout << "Frequency: " << mFrequency << endl;
	else
		cerr << "Uninitialized!\n";
}

bool VastFrequency::saveParametersToVast()
{
	if ( !mInitialized )
	{
		cerr << "Uninitialized!\n";
		return false;
	}

	FMTX_MODE_ENUM ret = FMTX_MODE_OK;

	ret = (FMTX_MODE_ENUM) fmtxTransmitterSetTuneFreq(mFrequency);
	if (ret != FMTX_MODE_OK)
	{
		logwrite(LOG_ERROR, "Couldn't set freq to %d", mFrequency);
		return false;
	}

	return true;
}


/*
 *
 */
VastPreemphasisId::VastPreemphasisId()
{
}

VastPreemphasisId::~VastPreemphasisId()
{
}

VastPreemphasisId& VastPreemphasisId::get()
{
	static VastPreemphasisId instance;
	return instance;
}

VastPreemphasisId& VastPreemphasisId::operator=(uint8_t preemphasisId)
{
	mInitialized = true;
	mPreemphasisId = preemphasisId;
}

void VastPreemphasisId::printParameters()
{
	if ( mInitialized )
		cout << "PreemphasisId: " << static_cast<int>(mPreemphasisId)
			 << ( mPreemphasisId == 1 ? " (US)" : "" )
			 << ( mPreemphasisId == 2 ? " (Europe)" : "" )
			 << endl;
	else
		cerr << "Unitialized!\n";
}

bool VastPreemphasisId::saveParametersToVast()
{
	if ( !mInitialized )
	{
		cerr << "Uninitialized!\n";
		return false;
	}

	FMTX_MODE_ENUM ret = FMTX_MODE_OK;

	ret = (FMTX_MODE_ENUM) fmtxTransmitterSetPreemphasisId(mPreemphasisId);
	if(ret != FMTX_MODE_OK)
	{
		logwrite(LOG_ERROR, "Can't set transmitter preemphasis flag");
		return false;
	}

	return true;
}


/*
 *
 */
VastPilotLmrRds::VastPilotLmrRds()
{
}

VastPilotLmrRds::~VastPilotLmrRds()
{
}

VastPilotLmrRds& VastPilotLmrRds::get()
{
	static VastPilotLmrRds instance;
	return instance;
}

void VastPilotLmrRds::setPilot(bool pilot)
{
	mInitialized = true;

	mPilotSet = true;
	mPilot = pilot;
}

void VastPilotLmrRds::setStereo(bool stereo)
{
	mInitialized = true;

	mStereoSet = true;
	mStereo = stereo;
}

void VastPilotLmrRds::setRds(bool rds)
{
	mInitialized = true;

	mRdsSet = true;
	mRds = rds;
}

void VastPilotLmrRds::printParameters()
{
	if ( mInitialized )
	{
		if ( mPilotSet )
			cout << "Pilot " << ( mPilot ? "enabled" : "disabled" ) << endl;
		if ( mStereoSet )
			cout << ( mStereo ? "Stereo" : "Mono" ) << endl;
		if ( mRdsSet )
			cout << "RDS " << ( mRds ? "enabled" : "disabled" ) << endl;
	}
	else
		cerr << "Unitialized!\n";
}

bool VastPilotLmrRds::saveParametersToVast()
{
	if ( !mInitialized )
	{
		cerr << "Uninitialized!\n";
		return false;
	}

	FMTX_MODE_ENUM ret = FMTX_MODE_OK;
	uint8_t ComponentFlags = 0;

	if ( ! mPilotSet || ! mStereoSet || ! mRdsSet )
		ComponentFlags = (FMTX_MODE_ENUM) fmtxTransmitterGetComponentFlags();

	if ( mStereoSet && mStereo )
		ComponentFlags |= TX_COMPONENT_ENABLE_LMR_MASK;
	else if ( mStereoSet )
		ComponentFlags &= ~TX_COMPONENT_ENABLE_LMR_MASK;

	if ( mPilotSet && mPilot )
		ComponentFlags |= TX_COMPONENT_ENABLE_PILOT_MASK;
	else if ( mPilotSet )
		ComponentFlags &= ~TX_COMPONENT_ENABLE_PILOT_MASK;

	if ( mRdsSet && mRds )
		ComponentFlags |= TX_COMPONENT_ENABLE_RDS_MASK;
	else if ( mRdsSet)
		ComponentFlags &= ~TX_COMPONENT_ENABLE_RDS_MASK;

	ret = (FMTX_MODE_ENUM) fmtxTransmitterSetComponentFlags(ComponentFlags);
	if (ret != FMTX_MODE_OK)
	{
		logwrite(LOG_ERROR, "Can't set transmitter components flag");
		exit(EXIT_FAILURE);
	}

	return true;
}


/*
 *
 */
VastPilotDeviation::VastPilotDeviation()
{
}

VastPilotDeviation::~VastPilotDeviation()
{
}

VastPilotDeviation& VastPilotDeviation::get()
{
	static VastPilotDeviation instance;
	return instance;
}

VastPilotDeviation& VastPilotDeviation::operator=(uint32_t pilotDeviation)
{
	mInitialized = true;
	mPilotDeviation = pilotDeviation;
}

void VastPilotDeviation::printParameters()
{
	if ( mInitialized )
		cout << "PilotDeviation: " << static_cast<int>(mPilotDeviation) << endl;
	else
		cerr << "Unitialized!\n";
}

bool VastPilotDeviation::saveParametersToVast()
{
	if ( !mInitialized )
	{
		cerr << "Uninitialized!\n";
		return false;
	}

	FMTX_MODE_ENUM ret = FMTX_MODE_OK;

	ret = (FMTX_MODE_ENUM) fmtxTransmitterSetPilotDeviation(mPilotDeviation);
	if (ret != FMTX_MODE_OK)
	{
		logwrite(LOG_ERROR, "Can't set transmitter pilot deviation flag");
		return false;
	}

	return true;
}


/*
 *
 */
VastPilotFrequency::VastPilotFrequency()
{
}

VastPilotFrequency::~VastPilotFrequency()
{
}

VastPilotFrequency& VastPilotFrequency::get()
{
	static VastPilotFrequency instance;
	return instance;
}

VastPilotFrequency& VastPilotFrequency::operator=(uint16_t pilotFrequency)
{
	mInitialized = true;
	mPilotFrequency = pilotFrequency;
}

void VastPilotFrequency::printParameters()
{
	if ( mInitialized )
		cout << "PilotFrequency: " << static_cast<int>(mPilotFrequency) << endl;
	else
		cerr << "Unitialized!\n";
}

bool VastPilotFrequency::saveParametersToVast()
{
	if ( !mInitialized )
	{
		cerr << "Uninitialized!\n";
		return false;
	}

	FMTX_MODE_ENUM ret = FMTX_MODE_OK;

	ret = (FMTX_MODE_ENUM) fmtxTransmitterSetPilotFrequency(mPilotFrequency);
	if (ret != FMTX_MODE_OK)
	{
		logwrite(LOG_ERROR, "Can't set transmitter pilot frequency flag");
		return false;
	}

	return true;
}


/*
 *
 */
VastRDSRtText::VastRDSRtText()
{
}

VastRDSRtText::~VastRDSRtText()
{
}

VastRDSRtText& VastRDSRtText::get()
{
	static VastRDSRtText instance;
	return instance;
}

VastRDSRtText& VastRDSRtText::operator=(string rtMessage)
{
	mInitialized = true;

	if ( mArtist.empty() && mTitle.empty() )
		mRtMessage = rtMessage;
	else
		cerr << "Artist or Title already set, ignoring generic RT message.\n";
}

void VastRDSRtText::setArtist(string artist)
{
	mInitialized = true;

	if ( mRtMessage.length() )
	{
		cerr << "RT Message was previously set, ignoring in favor of RT+ artist\n";
		mRtMessage.clear();
	}

	mArtist = artist;
}

void VastRDSRtText::setTitle(string title)
{
	mInitialized = true;

	if ( mRtMessage.length() )
	{
		cerr << "RT Message was previously set, ignoring in favor of RT+ title\n";
		mRtMessage.clear();
	}

	mTitle = title;
}

void VastRDSRtText::printParameters()
{
	if ( mInitialized )
	{
		if ( mRtMessage.length() )
			cout << "RDS RT Message: " << mRtMessage << endl;
		else
		{
			if ( mArtist.length() )
				cout << "RDS RT+ (code 4 - ITEM.ARTIST): " << mArtist << endl;
			if ( mTitle.length() )
				cout << "RDS RT+ (code 1 - ITEM.TITLE): " << mTitle << endl;
		}
	}
	else
		cerr << "Uninitialized!\n";
}

bool VastRDSRtText::saveParametersToVast()
{
	if ( !mInitialized )
	{
		cerr << "Uninitialized!\n";
		return false;
	}

	FMTX_MODE_ENUM ret = FMTX_MODE_OK;

	if ( mRtMessage.length() )
	{
		ret = (FMTX_MODE_ENUM) fmtxRDSSetRtMessage(mRtMessage.c_str());
		if (ret != FMTX_MODE_OK)
		{
			logwrite(LOG_ERROR, "Can't set RDS RT Message");
			exit(EXIT_FAILURE);
		}
	}

	if ( mTitle.length() || mArtist.length() )
	{
		int title_len = 0;
		int title_pos = 0;
		int title_code = 1;		// ITEM.TITLE

		int artist_len = 0;
		int artist_pos = 0;
		int artist_code = 4;	// ITEM.ARTIST

		string rtMessage;

		if ( mTitle.length() && mArtist.length() )
		{
			title_len = mTitle.length();
			artist_len = mArtist.length();

			artist_pos = 0;
			title_pos = title_len + 3;

			rtMessage = mArtist + " - " + mTitle;
		}
		else if ( mTitle.length() )
		{
			title_len = mTitle.length();

			rtMessage = mTitle;
		}
		else if ( mArtist.length() )
		{
			artist_len = mArtist.length();

			rtMessage = mArtist;
		}

		ret = (FMTX_MODE_ENUM) fmtxRDSSetRtMessage(rtMessage.c_str());
		if (ret != FMTX_MODE_OK)
		{
			logwrite(LOG_ERROR, "Can't set RDS RT Message");
			exit(EXIT_FAILURE);
		}

		if ( artist_len == 0 )	// Use DUMMY_CLASS code if we don't have
			artist_code = 0;	// an artist set
		if ( title_len == 0 )	// Use DUMMY_CLASS code if we don't have
			title_code = 0;		// a title set

		ret = (FMTX_MODE_ENUM) fmtxRDSSetRtPlusInfo(artist_code, artist_pos, artist_len, title_code, title_pos, title_len);
		if (ret != FMTX_MODE_OK)
		{
			logwrite(LOG_ERROR, "Can't set RDS RT+ Params");
			exit(EXIT_FAILURE);
		}
	}

	return true;
}

/*
 *
 */
VastPsMessage::VastPsMessage()
{
}

VastPsMessage::~VastPsMessage()
{
}

VastPsMessage& VastPsMessage::get()
{
	static VastPsMessage instance;
	return instance;
}

/*
VastPsMessage& VastPsMessage::operator=(int rdsLineId)
{
	mInitialized = true;
	mRdsLineId = rdsLineId;
}
*/

VastPsMessage& VastPsMessage::operator=(string psMessage)
{
	mInitialized = true;
	mPsMessage = psMessage;
}

void VastPsMessage::printParameters()
{
	/*
	int count = 0;
	for ( string psm : mPsMessages )
	{
		count++;
		cout << "PS Message[" << count << "]: " << psm << endl;
	}
	*/

	if ( mInitialized )
		cout << "PS Message[0]: " << mPsMessage << endl;
	else
		cerr << "Uninitialized!\n";
}

/*
char *convert(const string & s)
{
	char *pc = new char[s.size()+1];
	strcpy(pc, s.c_str());
	return pc; 
}
*/

bool VastPsMessage::saveParametersToVast()
{
	if ( !mInitialized )
	{
		cerr << "Uninitialized!\n";
		return false;
	}

	FMTX_MODE_ENUM ret = FMTX_MODE_OK;

	ret = (FMTX_MODE_ENUM) fmtxRDSSetPsMessageCount(mRdsLineId);
	if (ret != FMTX_MODE_OK)
	{
		logwrite(LOG_ERROR, "Can't set RDS PS Number");
		exit(EXIT_FAILURE);
	}

	if ( mPsMessage.size() )
	{
		/*
		transform(vs.begin(), vs.end(), back_inserter(vc), convert);

		ret = (FMTX_MODE_ENUM) fmtxRDSSetPsMessageById(mRdsLineId,
				mRdsLineText[settings.rdsLineId]);
		*/

		ret = (FMTX_MODE_ENUM) fmtxRDSSetPsMessageById(mRdsLineId,
				const_cast<char*>(mPsMessage.c_str()));

		/*
		for ( size_t i = 0 ; i < vc.size() ; i++ )
			delete [] vc[i];
		*/

		if (ret != FMTX_MODE_OK)
		{
			logwrite(LOG_ERROR, "Can't set RDS PS Message");
			exit(EXIT_FAILURE);
		}
	}
}

/*
 *
 */
VastRdsDeviation::VastRdsDeviation()
{
}

VastRdsDeviation::~VastRdsDeviation()
{
}

VastRdsDeviation& VastRdsDeviation::get()
{
	static VastRdsDeviation instance;
	return instance;
}

VastRdsDeviation& VastRdsDeviation::operator=(uint32_t rdsDeviation)
{
	mInitialized = true;
	mRdsDeviation = rdsDeviation;
}

void VastRdsDeviation::printParameters()
{
	if ( mInitialized )
		cout << "RDS Deviation: " << mRdsDeviation << endl;
	else
		cerr << "Uninitialized!\n";
}

bool VastRdsDeviation::saveParametersToVast()
{
	if ( !mInitialized )
	{
		cerr << "Uninitialized!\n";
		return false;
	}

	FMTX_MODE_ENUM ret = FMTX_MODE_OK;

	ret = (FMTX_MODE_ENUM) fmtxTransmitterSetRDSDeviation(mRdsDeviation);
	if (ret != FMTX_MODE_OK)
	{
		logwrite(LOG_ERROR, "Can't set transmitter audio deviation flag");
		exit(EXIT_FAILURE);
	}

	return true;
}

VastRdsPI::VastRdsPI()
{
}

VastRdsPI::~VastRdsPI()
{
}

VastRdsPI& VastRdsPI::get()
{
	static VastRdsPI instance;
	return instance;
}

VastRdsPI& VastRdsPI::operator=(uint16_t rdsPI)
{
	mInitialized = true;
	mRdsPI = rdsPI;
}

void VastRdsPI::printParameters()
{
	if ( mInitialized )
		cout << "RDS PI: " << static_cast<int>(mRdsPI)
			 << hex << uppercase << " (" << mRdsPI << ")"
			 << dec << nouppercase << endl;
	else
		cerr << "Uninitialized!\n";
}

bool VastRdsPI::saveParametersToVast()
{
	if ( !mInitialized )
	{
		cerr << "Uninitialized!\n";
		return false;
	}

	FMTX_MODE_ENUM ret = FMTX_MODE_OK;

	ret = (FMTX_MODE_ENUM) fmtxRDSSetPI(mRdsPI);
	if (ret != FMTX_MODE_OK)
	{
		logwrite(LOG_ERROR, "Can't set RDS PI");
		exit(EXIT_FAILURE);
	}

	return true;
}

/*
 *
 */
VastReset::VastReset()
{
}

VastReset::~VastReset()
{
}

VastReset& VastReset::get()
{
	static VastReset instance;
	return instance;
}

void VastReset::printParameters()
{
	if ( mInitialized )
		cout << "Reset" << endl;
	else
		cerr << "Uninitialized!\n";
}

bool VastReset::saveParametersToVast()
{
	if ( !mInitialized )
	{
		cerr << "Uninitialized!\n";
		return false;
	}

	FMTX_MODE_ENUM ret = FMTX_MODE_OK;

	// Turn on the front end
	ret = (FMTX_MODE_ENUM) fmtxIoAppFeReset();
	switch(ret)
	{
		case FMTX_MODE_POWER_DOWN:
		{
			logwrite(LOG_INFO, "Frontend powered down!");
			fmtxCmdStatus=FMTX_MODE_OK;
			break;
		}
		default:
		{
			logwrite(LOG_ERROR, "Error while resetting");
			fmtxCmdStatus=FMTX_MODE_NONE;
			exit(EXIT_FAILURE);
			break;
		}
	}

	return true;
}
