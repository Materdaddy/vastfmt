#ifndef __VAST_COMMANDS_H__
#define __VAST_COMMANDS_H__

#include <stdint.h>
#include <iostream>
#include <string>

class VastCommand
{
public:
	VastCommand();
	VastCommand(VastCommand &V)
	{
		std::cerr << "Shouldn't be copying me!!!\n";
		exit(EXIT_FAILURE);
	}
	~VastCommand();

	virtual void printParameters();

	virtual bool getParametersFromVast();
	virtual bool saveParametersToVast();

protected:
	bool mInitialized = false;
};

class VastPowerAntennaCap : public VastCommand
{
public:
	VastPowerAntennaCap();
	~VastPowerAntennaCap();

	static VastPowerAntennaCap& get();

	void setPower(uint8_t power);
	void setAntennaCap(double antennaCap);

	void printParameters();
	bool saveParametersToVast();

private:
	bool mPowerSet = false;
	uint8_t mPower;// = 88;
	bool mAntennaCapSet = false;
	double mAntennaCap;// = 7.5;
};

class VastTransmission : public VastCommand
{
public:
	VastTransmission();
	~VastTransmission();

	static VastTransmission& get();
	VastTransmission& operator=(bool transmit);

	void printParameters();
	bool saveParametersToVast();

private:
	bool mTransmit;
};

class VastFrequency : public VastCommand
{
public:
	VastFrequency();
	~VastFrequency();

	static VastFrequency& get();
	VastFrequency &operator=(double frequency);

	void printParameters();
	bool saveParametersToVast();

private:
	double mFrequency;
};

class VastPreemphasisId : public VastCommand
{
public:
	VastPreemphasisId();
	~VastPreemphasisId();

	static VastPreemphasisId& get();
	VastPreemphasisId& operator=(uint8_t preemphasisId);

	void printParameters();
	bool saveParametersToVast();

private:
	uint8_t mPreemphasisId;// = 0;
};

class VastPilotLmrRds : public VastCommand
{
public:
	VastPilotLmrRds();
	~VastPilotLmrRds();

	static VastPilotLmrRds& get();

	void setPilot(bool pilot);
	void setStereo(bool stereo);
	void setRds(bool rds);

	void printParameters();
	bool saveParametersToVast();
	bool getParameter();

private:
	bool mPilotSet = false;
	bool mPilot;
	bool mStereoSet = false;
	bool mStereo;
	bool mRdsSet = false;
	bool mRds;
};

class VastPilotDeviation : public VastCommand
{
public:
	VastPilotDeviation();
	~VastPilotDeviation();

	static VastPilotDeviation& get();
	VastPilotDeviation& operator=(uint32_t pilotDeviation);

	void printParameters();
	bool saveParametersToVast();

private:
	uint16_t mPilotDeviation;
};


class VastPilotFrequency : public VastCommand
{
public:
	VastPilotFrequency();
	~VastPilotFrequency();

	static VastPilotFrequency& get();
	VastPilotFrequency& operator=(uint16_t pilotFrequency);

	void printParameters();
	bool saveParametersToVast();

private:
	uint32_t mPilotFrequency;
};


class VastRDS : public VastCommand
{
public:
	VastRDS();
	~VastRDS();

	static VastRDS& get();
	VastRDS& operator=(bool rds);

	void printParameters();
	bool saveParametersToVast();

private:
	bool mRds;
};


class VastRDSRtText : public VastCommand
{
public:
	VastRDSRtText();
	~VastRDSRtText();

	static VastRDSRtText& get();
	VastRDSRtText& operator=(std::string rtMessage);

	// Setters for RT+ stuff instead of
	// a the '=' operator for a generic
	// RT RDS message.
	void setArtist(std::string artist);
	void setTitle(std::string title);

	void printParameters();
	bool saveParametersToVast();

private:
	std::string mRtMessage;
	std::string mArtist;
	std::string mTitle;
};


class VastPsMessage : public VastCommand
{
public:
	VastPsMessage();
	~VastPsMessage();

	static VastPsMessage& get();
	VastPsMessage& operator=(std::string psMessage);

	void printParameters();
	bool saveParametersToVast();

private:
	int mRdsLineId = 0;
	std::string mPsMessage;
};

class VastRdsDeviation : public VastCommand
{
public:
	VastRdsDeviation();
	~VastRdsDeviation();

	static VastRdsDeviation& get();
	VastRdsDeviation& operator=(uint32_t rdsDeviation);

	void printParameters();
	bool saveParametersToVast();

private:
	uint32_t mRdsDeviation;
};

class VastRdsPI : public VastCommand
{
public:
	VastRdsPI();
	~VastRdsPI();

	static VastRdsPI& get();
	VastRdsPI& operator=(uint16_t rdsPI);

	void printParameters();
	bool saveParametersToVast();

private:
	uint16_t mRdsPI;
};

#endif // __VAST_COMMANDS_H__
