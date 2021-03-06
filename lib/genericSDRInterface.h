/*    SDRPortal - A generic web-based interface for SDRs
 *    Copyright (C) 2013 Ben Kempke (bpkempke@umich.edu)
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GENERIC_SDR_INT_H
#define GENERIC_SDR_INT_H

#include <map>
#include <string>
#include <string.h>
#include <vector>
#include <stdint.h>
#include "generic.h"
#include "streamConverter.h"
#include "hierarchicalDataflowBlock.h"

class portalDataSocket;

class rxtxChanInfo{
public:
	int rx_chan;
	int tx_chan;
	rxtxChanInfo(int r, int t) : rx_chan(r), tx_chan(t) {};
	rxtxChanInfo() : rx_chan(0), tx_chan(0) {};
};

class paramData {
public:
	paramData(double in_data, rxtxChanInfo in_channel=rxtxChanInfo(0,0)) : channel(in_channel) {data_type = DOUBLE; param_double = in_data;};
	paramData(int in_data, rxtxChanInfo in_channel=rxtxChanInfo(0,0)) : channel(in_channel) {data_type = INT; param_double = (double)in_data;};
	paramData(uint32_t in_data, rxtxChanInfo in_channel=rxtxChanInfo(0,0)) : channel(in_channel) {data_type = UINT32; param_double = (double)in_data;};
	paramData(uint64_t in_data, rxtxChanInfo in_channel=rxtxChanInfo(0,0)) : channel(in_channel) {data_type = UINT64; param_double = (double)in_data;};
	paramData(const char *in_data, rxtxChanInfo in_channel=rxtxChanInfo(0,0)) : channel(in_channel) {data_type = C_STRING; param_cstr = (char*)in_data;};
	paramData(rxtxChanInfo in_channel=rxtxChanInfo(0,0)) : channel(in_channel) {data_type = VOID;};
	~paramData(){};

	//Accessor methods
	int getInt(){return (int)param_double;};
	double getDouble(){return param_double;};
	uint32_t getUInt32(){return (uint32_t)param_double;};
	uint64_t getUInt64(){return (uint64_t)param_double;};
	char *getCString(){return param_cstr;};
	rxtxChanInfo getChannel(){return channel;};
private:
	double param_double;
	char *param_cstr;
	primType data_type;
	rxtxChanInfo channel;
};

struct iqData {
	void *data;
	int num_bytes;
	int channel_uid;
};

class genericSDRInterface;

struct paramAccessor {
	//typdefs to make things prettier later on...
	typedef void (genericSDRInterface::*setMethodType)(paramData);
	typedef paramData (genericSDRInterface::*getMethodType)(rxtxChanInfo);
	typedef bool (genericSDRInterface::*checkMethodType)(paramData);

	//actual struct members (using camel-caps so that the use of these function pointers later-on looks cleaner)
	primType arg_type;
	setMethodType setMethod;
	getMethodType getMethod;
	checkMethodType checkMethod;

	//struct constructor
	paramAccessor(primType in_prim, setMethodType in_set, getMethodType in_get, checkMethodType in_check) : arg_type(in_prim), setMethod(in_set), getMethod(in_get), checkMethod(in_check) {};
	paramAccessor(){};
};

class genericSDRInterface : public hierarchicalDataflowBlock{
public:
	genericSDRInterface(streamType common_type);
	virtual ~genericSDRInterface(){};
	void setSDRParameter(int in_uid, std::string name, std::string val);
	int getNumAllocatedChannels();
	rxtxChanInfo getChanInfo(int uid);

	int addChannel(portalDataSocket *in_channel);
	void bindRXChannel(int rx_chan, int in_uid);
	void bindTXChannel(int tx_chan, int in_uid);

	std::vector<streamType> getResultingPrimTypes(int rx_chan);
	void distributeRXData(void *in_data, int num_bytes, int rx_chan);
	void setStreamDataType(streamType in_type, int in_uid);

	//All possible get/set/check methods.  If they're not implemented, the virtual method will default to throwing an exception
	virtual void setRXFreq(paramData in_param){throw invalidCommandException("");};
	virtual void setTXFreq(paramData in_param){throw invalidCommandException("");};
	virtual void setRXGain(paramData in_param){throw invalidCommandException("");};
	virtual void setTXGain(paramData in_param){throw invalidCommandException("");};
	virtual void setRXRate(paramData in_param){throw invalidCommandException("");};
	virtual void setTXRate(paramData in_param){throw invalidCommandException("");};
	virtual paramData getRXFreq(rxtxChanInfo in_chan){throw invalidCommandException("");};
	virtual paramData getTXFreq(rxtxChanInfo in_chan){throw invalidCommandException("");};
	virtual paramData getRXGain(rxtxChanInfo in_chan){throw invalidCommandException("");};
	virtual paramData getTXGain(rxtxChanInfo in_chan){throw invalidCommandException("");};
	virtual paramData getRXRate(rxtxChanInfo in_chan){throw invalidCommandException("");};
	virtual paramData getTXRate(rxtxChanInfo in_chan){throw invalidCommandException("");};
	virtual bool checkRXChannel(int in_chan){throw invalidCommandException("");};
	virtual bool checkTXChannel(int in_chan){throw invalidCommandException("");};
	virtual void openRXChannel(int in_chan){throw invalidCommandException("");};
	virtual void openTXChannel(int in_chan){throw invalidCommandException("");};
	virtual bool checkRXFreq(paramData in_param){throw invalidCommandException("");};
	virtual bool checkTXFreq(paramData in_param){throw invalidCommandException("");};
	virtual bool checkRXGain(paramData in_param){throw invalidCommandException("");};
	virtual bool checkTXGain(paramData in_param){throw invalidCommandException("");};
	virtual bool checkRXRate(paramData in_param){throw invalidCommandException("");};
	virtual bool checkTXRate(paramData in_param){throw invalidCommandException("");};
	virtual void setCustomSDRParameter(std::string name, std::string val, int in_chan) = 0;
	virtual void disconnect() = 0;
	virtual void connect() = 0;
	virtual void txIQData(void *data, int num_bytes, int tx_chan){};
	std::map<std::string, paramAccessor > param_accessors;

	//Methods inherited from hierarchicalDataflowBlock
	virtual void dataFromUpperLevel(void *data, int num_bytes, int local_up_channel=0){};
	virtual void dataFromLowerLevel(void *data, int num_bytes, int local_down_channel=0);
private:
	int num_channels, cur_channel;
	std::map<int,portalDataSocket*> uid_map;
	std::map<int,std::vector<portalDataSocket*> > rx_chan_to_streams, tx_chan_to_streams;
	std::map<int,rxtxChanInfo> uid_to_chaninfo;

	//Stream converter for converting to/from OTW format
	streamConverter str_converter;
	
};

#endif
