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

#include "uhdInterface.h"
#include "generic.h"
#include <cstdio>
#include <iostream>
#include <fstream>
using namespace std;

struct argStruct{
	int channel;
	uhdInterface* uhd_int;
};

//Proxies in order to run a thread within a member function
static void *uhdReadProxy(void *in_args){
	argStruct *arguments = static_cast<argStruct*>(in_args);
	void *return_pointer = arguments->uhd_int->rxThread(arguments->channel);
	delete arguments;
	return return_pointer;
}
static void *uhdWriteProxy(void *in_args){
	argStruct *arguments = static_cast<argStruct*>(in_args);
	void *return_pointer = arguments->uhd_int->txThread(arguments->channel);
	delete arguments;
	return return_pointer;
}

ofstream log_file;

/**********************
  * uhdInterface class
  *********************/
uhdInterface::uhdInterface(string args, string tx_subdev, string rx_subdev, string tx_ant, string rx_ant, double tx_rate, double rx_rate, double tx_freq, double rx_freq, double tx_gain, double rx_gain, bool codec_highspeed) : 
	genericSDRInterface(STREAM_INT16_T), 
	d_args(args), 
	d_tx_subdev(tx_subdev), 
	d_rx_subdev(rx_subdev),
	d_tx_ant(tx_ant),
	d_rx_ant(rx_ant),
	d_tx_rate(tx_rate),
	d_rx_rate(rx_rate),
	d_tx_freq(tx_freq),
	d_rx_freq(rx_freq),
	d_tx_gain(tx_gain),
	d_rx_gain(rx_gain),
	d_codec_highspeed(codec_highspeed)
	{

	//Open a log file
	log_file.open("log.out", ios::out | ios::trunc | ios::binary);

	//Connect to the USRP
	connect();

	rx_running = false;
	tx_running = false;

}

void uhdInterface::writeMAX2829Reg(uint32_t value){
	uhd::usrp::dboard_iface::sptr  iface_ptr = shared_uhd->get_rx_dboard_iface();
	printf("UHDINT_XCVR@%p: send value 0x%05x\n",(void*)(iface_ptr.get()),value);
	iface_ptr->write_spi(uhd::usrp::dboard_iface::UNIT_RX, uhd::spi_config_t::EDGE_RISE, value, 24);
}

void uhdInterface::setRXFreq(paramData in_param){
	shared_uhd->set_rx_freq(in_param.getDouble(), in_param.getChannel().rx_chan);
}

void uhdInterface::setTXFreq(paramData in_param){
	shared_uhd->set_tx_freq(in_param.getDouble(), in_param.getChannel().tx_chan);
}

void uhdInterface::setRXGain(paramData in_param){
	shared_uhd->set_rx_gain(in_param.getDouble(), in_param.getChannel().rx_chan);
}

void uhdInterface::setTXGain(paramData in_param){
	shared_uhd->set_tx_gain(in_param.getDouble(), in_param.getChannel().tx_chan);
}

void uhdInterface::setRXRate(paramData in_param){
	shared_uhd->set_rx_rate(in_param.getDouble(), in_param.getChannel().rx_chan);
}

void uhdInterface::setTXRate(paramData in_param){
	shared_uhd->set_tx_rate(in_param.getDouble(), in_param.getChannel().tx_chan);
}

paramData uhdInterface::getRXFreq(rxtxChanInfo in_chan){
	double rx_freq = shared_uhd->get_rx_freq(in_chan.rx_chan);
	return paramData(rx_freq, in_chan);
}

paramData uhdInterface::getTXFreq(rxtxChanInfo in_chan){
	double tx_freq = shared_uhd->get_tx_freq(in_chan.tx_chan);
	return paramData(tx_freq, in_chan);
}

paramData uhdInterface::getRXGain(rxtxChanInfo in_chan){
	double rx_gain = shared_uhd->get_rx_gain(in_chan.rx_chan);
	return paramData(rx_gain, in_chan);
}

paramData uhdInterface::getTXGain(rxtxChanInfo in_chan){
	double tx_gain = shared_uhd->get_tx_gain(in_chan.tx_chan);
	return paramData(tx_gain, in_chan);
}

paramData uhdInterface::getRXRate(rxtxChanInfo in_chan){
	double rx_rate = shared_uhd->get_rx_rate(in_chan.rx_chan);
	return paramData(rx_rate, in_chan);
}

paramData uhdInterface::getTXRate(rxtxChanInfo in_chan){
	double tx_rate = shared_uhd->get_tx_rate(in_chan.tx_chan);
	return paramData(tx_rate, in_chan);
}

bool uhdInterface::checkRXChannel(int in_chan){
	size_t num_rx_chan = shared_uhd->get_rx_num_channels();
	return (size_t)in_chan < num_rx_chan;
}

bool uhdInterface::checkTXChannel(int in_chan){
	size_t num_tx_chan = shared_uhd->get_tx_num_channels();
	return (size_t)in_chan < num_tx_chan;
}

bool uhdInterface::checkRXFreq(paramData in_param){
	uhd::freq_range_t frange = shared_uhd->get_rx_freq_range(in_param.getChannel().rx_chan);
	double clipped = frange.clip(in_param.getDouble());
	return (clipped == in_param.getDouble());
}

bool uhdInterface::checkTXFreq(paramData in_param){
	uhd::freq_range_t frange = shared_uhd->get_tx_freq_range(in_param.getChannel().tx_chan);
	double clipped = frange.clip(in_param.getDouble());
	return (clipped == in_param.getDouble());
}

bool uhdInterface::checkRXGain(paramData in_param){
	uhd::gain_range_t grange = shared_uhd->get_rx_gain_range(in_param.getChannel().rx_chan);
	double clipped = grange.clip(in_param.getDouble());
	return (clipped == in_param.getDouble());
}

bool uhdInterface::checkTXGain(paramData in_param){
	uhd::gain_range_t grange = shared_uhd->get_tx_gain_range(in_param.getChannel().tx_chan);
	double clipped = grange.clip(in_param.getDouble());
	return (clipped == in_param.getDouble());
}

bool uhdInterface::checkRXRate(paramData in_param){
	uhd::meta_range_t rrange = shared_uhd->get_rx_rates(in_param.getChannel().rx_chan);
	double clipped = rrange.clip(in_param.getDouble());
	return (clipped == in_param.getDouble());
}

bool uhdInterface::checkTXRate(paramData in_param){
	uhd::meta_range_t rrange = shared_uhd->get_tx_rates(in_param.getChannel().tx_chan);
	double clipped = rrange.clip(in_param.getDouble());
	return (clipped == in_param.getDouble());
}

void uhdInterface::openRXChannel(int in_chan){
	//Set up the receive streamers (let's keep it to a float stream for now)
	uhd::stream_args_t rx_stream_args("sc16","sc16");//(application format, wire format)
	rx_stream_args.channels.resize(1);
	rx_stream_args.channels[0] = in_chan;
	if((int)rx_streams.size() <= in_chan){
		rx_streams.resize(in_chan+1);
		rx_md.resize(in_chan+1);
	}
	rx_streams[in_chan] = shared_uhd->get_rx_stream(rx_stream_args);

	//We likely want to be always receiving, so let's start that up
	rxStart(in_chan);

	argStruct *arguments = new argStruct();
	arguments->uhd_int = this;
	arguments->channel = in_chan;
	rx_running = true;
	pthread_create(&rx_listener, NULL, uhdReadProxy, (void*)arguments);

}

void uhdInterface::openTXChannel(int in_chan){
	//Set up the transmit streamers (let's keep it to a float stream for now)
	uhd::stream_args_t tx_stream_args("fc32");
	tx_streams[in_chan] = shared_uhd->get_tx_stream(tx_stream_args);

	//Other miscellaneous initialization stuff
	tx_md[in_chan].start_of_burst = false;
	tx_md[in_chan].end_of_burst = false;

	//A tx thread is also instantiated which handles sending stuff from tx_queue out to the radio
	argStruct arguments;
	arguments.uhd_int = this;
	arguments.channel = in_chan;
	tx_running = true;
	pthread_create(&tx_thread, NULL, uhdWriteProxy, (void*)&arguments);
}

void uhdInterface::txIQData(void *data, int num_bytes, int tx_chan){

	//Cast this message to an array of complex floats
	std::complex<float> *in_array_cast = static_cast<std::complex<float> *>(data);
	for(unsigned int jj=0; jj < num_bytes/sizeof(std::complex<float>); jj++)
		tx_queue[tx_chan].push(in_array_cast[jj]);

	//TODO: If there is too much data, block here...
}

//This is a dummy function whose only job is to notify the uhddriver that we're done with the current tx burst
void uhdInterface::txEnd(int in_chan){
	//Send a sample of dummy data out to the device with the end_of_burst bit set
	complex<float> dummy_sample;
	tx_md[in_chan].end_of_burst = true;
	tx_streams[in_chan]->send(&dummy_sample, 1, tx_md[in_chan]);
	tx_md[in_chan].end_of_burst = false;
}

void uhdInterface::rxStart(int in_chan){
	uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
	stream_cmd.num_samps = 0;
	stream_cmd.stream_now = true;
	stream_cmd.time_spec = uhd::time_spec_t();
	shared_uhd->issue_stream_cmd(stream_cmd, in_chan);
}

int uhdInterface::rxData(complex<int16_t> *rx_data_iq, int num_samples, int in_chan){
	uhd::ref_vector<void *> buffers(rx_data_iq);
	//Simple call to stream->recv
	int ret = rx_streams[in_chan]->recv(buffers, num_samples, rx_md[in_chan], 1.0); //1.0 is the timeout time (in seconds)

	//Error checking
	if(rx_md[in_chan].error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT)
		cout << "rx error 1\n";
	else if(rx_md[in_chan].error_code == uhd::rx_metadata_t::ERROR_CODE_OVERFLOW)
		cout << "rx error 2\n";
	else if(rx_md[in_chan].error_code != uhd::rx_metadata_t::ERROR_CODE_NONE){
		cout << "rx error 3\n";
		ret = -1;
	}

	return ret;
}

//Let's break this in to chunks of 256 samples for now.  Can go up or down, but let's try this for now
#define RX_CHUNK_SIZE 512
void *uhdInterface::rxThread(int rx_chan){
	complex<int16_t> rx_data[RX_CHUNK_SIZE];
	while(1){
		rxData(rx_data, RX_CHUNK_SIZE, rx_chan);
		//log_file.write((char*)rx_data, RX_CHUNK_SIZE*sizeof(complex<int16_t>));
		//Now send the data off to where it needs to go...
		distributeRXData(rx_data, RX_CHUNK_SIZE*sizeof(complex<int16_t>), 0);
	}
	return NULL;
}

//This thread waits until a theshold size is met in order to tx a constant stream of samples out to the USRP
#define TX_THRESHOLD 2000
void *uhdInterface::txThread(int tx_chan){
	std::complex<float> temp_send_space[TX_THRESHOLD];
	while(1){
		usleep(5000);
		if(tx_queue[tx_chan].size() > TX_THRESHOLD){
			txStart(tx_chan);
			while(tx_queue[tx_chan].size() > TX_THRESHOLD){
				//TODO: Lock here
				//Copy all the contents into the temp vector
				for(unsigned int ii=0; ii < TX_THRESHOLD; ii++){
					temp_send_space[ii] = tx_queue[tx_chan].front();
					tx_queue[tx_chan].pop();
				}
				//TODO: Unlock here

				//Simple function just passes all iq data out to the device
				tx_md[tx_chan].end_of_burst = false;
				tx_streams[tx_chan]->send(temp_send_space, TX_THRESHOLD, tx_md[tx_chan]);
			}
			txEnd(tx_chan);
		}
	}
	return NULL;
}

void uhdInterface::txStop(){
	if(tx_running){
		pthread_join(tx_thread, NULL);
		tx_running = false;
	}
}

void uhdInterface::rxStop(){
	if(rx_running == true){
		pthread_join(rx_listener, NULL);
		log_file.close();
		rx_running = false;
	}
}

void uhdInterface::setCustomSDRParameter(string name, string val, int in_chan){
	//There are a few custom USRP-specific parameters which must be dealt with
	if(name == "RXANT" || name == "TXANT"){
		vector<string> valid_antennas;
		if(name == "RXANT") valid_antennas = shared_uhd->get_rx_antennas(in_chan);
		else valid_antennas = shared_uhd->get_tx_antennas(in_chan);
		vector<string>::iterator it = find(valid_antennas.begin(), valid_antennas.end(), val);
		if(it != valid_antennas.end())
			if(name == "RXANT") shared_uhd->set_rx_antenna(val, in_chan);
			else shared_uhd->set_tx_antenna(val, in_chan);
		else 
			throw badArgumentException(badArgumentException::OUT_OF_BOUNDS, 1, val);
	} else
		throw invalidCommandException(name);
}

void uhdInterface::disconnect(){
	//TODO: Fill this in...
	txStop();
	rxStop();
	shared_uhd.reset();
}

void uhdInterface::connect(){
	//Open blank UHD object
	shared_uhd = uhd::usrp::multi_usrp::make(d_args);

	//Default to the internal oscillator
	shared_uhd->set_clock_source("internal");

	//If subdevice specification given, use it
	if(d_tx_subdev != "") shared_uhd->set_tx_subdev_spec(d_tx_subdev);
	if(d_rx_subdev != "") shared_uhd->set_rx_subdev_spec(d_rx_subdev);

	//Set the sample rate
	shared_uhd->set_tx_rate(d_tx_rate);
	shared_uhd->set_rx_rate(d_rx_rate);

	//Set the desired carrier frequencies
	shared_uhd->set_tx_freq(d_tx_freq);
	shared_uhd->set_rx_freq(d_rx_freq);

	//Set the desired gains
	shared_uhd->set_tx_gain(d_tx_gain);
	shared_uhd->set_rx_gain(d_rx_gain);

	//Set up the antennas
	printf("setting up antennas....\n");
	if(d_tx_ant != "") shared_uhd->set_tx_antenna(d_tx_ant);
	if(d_rx_ant != "") shared_uhd->set_rx_antenna(d_rx_ant);

	//TODO: Ugh... this is hard-coded...
	//shared_uhd->set_tx_bandwidth(24e6);
	//shared_uhd->set_rx_bandwidth(18e6);

	if(d_codec_highspeed){
		//Write all the regs we need if using this for demo build
		printf("setting max2829 regs...\n");

		if(d_rx_freq > 5e9){
			//5.63 GHz
			writeMAX2829Reg(0x00D33);
			writeMAX2829Reg(0x08004);
			writeMAX2829Reg(0x38E35);
	
			writeMAX2829Reg(0x04006);
			writeMAX2829Reg(0x007A7);
			writeMAX2829Reg(0x068B9);
			writeMAX2829Reg(0x006CB);//RX Gain
			writeMAX2829Reg(0x002AC);//TX Gain
			printf("Setting gpio_ddr...\n");
			uhd::usrp::dboard_iface::sptr  iface_ptr = shared_uhd->get_rx_dboard_iface();
			iface_ptr->set_gpio_ddr(uhd::usrp::dboard_iface::UNIT_RX, 0x7FE0, 0xFFFF);
			printf("Setting up atr registers...\n");
			iface_ptr->set_atr_reg(uhd::usrp::dboard_iface::UNIT_TX, uhd::usrp::gpio_atr::ATR_REG_RX_ONLY, 0xD010);
			iface_ptr->set_atr_reg(uhd::usrp::dboard_iface::UNIT_TX, uhd::usrp::gpio_atr::ATR_REG_TX_ONLY, 0x6810);
			iface_ptr->set_atr_reg(uhd::usrp::dboard_iface::UNIT_TX, uhd::usrp::gpio_atr::ATR_REG_FULL_DUPLEX, 0x6810);//TODO: This shouldn't really happen, should it??
			//printf("First reading the atr registers...\n");
			//printf("ATR_REG_RX_ONLY: %x\n",iface_ptr->get_atr_reg(uhd::usrp::dboard_iface::UNIT_TX, uhd::usrp::dboard_iface::ATR_REG_RX_ONLY));
			//Read back the state of the tx output to see if ANTSEL is switching...
			//while(1)
			//	printf("ANTSEL REG=%x\n",iface_ptr->read_gpio(uhd::usrp::dboard_iface::UNIT_TX));

		} else {
			//2.5 GHz
//			writeMAX2829Reg(0x20A63);
//			writeMAX2829Reg(0x2AAA4);
//			writeMAX2829Reg(0x38E25);
			writeMAX2829Reg(0x10d03);
			writeMAX2829Reg(0x15554);
			writeMAX2829Reg(0x38245);
			writeMAX2829Reg(0x38A45);

			writeMAX2829Reg(0x04006);
			writeMAX2829Reg(0x007A7);
			writeMAX2829Reg(0x068B9);
			writeMAX2829Reg(0x006CB);//RX Gain
			writeMAX2829Reg(0x002AC);//TX Gain
			printf("Setting gpio_ddr...\n");
			uhd::usrp::dboard_iface::sptr  iface_ptr = shared_uhd->get_rx_dboard_iface();
			iface_ptr->set_gpio_ddr(uhd::usrp::dboard_iface::UNIT_RX, 0x7FE0, 0xFFFF);
			printf("Setting up atr registers...\n");
			iface_ptr->set_atr_reg(uhd::usrp::dboard_iface::UNIT_TX, uhd::usrp::gpio_atr::ATR_REG_RX_ONLY, 0xD000);
			iface_ptr->set_atr_reg(uhd::usrp::dboard_iface::UNIT_TX, uhd::usrp::gpio_atr::ATR_REG_TX_ONLY, 0xA800);//Gotta make sure the right amp is turned on!
			iface_ptr->set_atr_reg(uhd::usrp::dboard_iface::UNIT_TX, uhd::usrp::gpio_atr::ATR_REG_FULL_DUPLEX, 0xA800);//TODO: This shouldn't really happen, should it??
	
		}
	}
}


