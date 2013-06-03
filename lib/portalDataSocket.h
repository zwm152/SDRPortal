
class portalDataSocket : public hierarchicalDataflowBlock{
public:
	portalDataSocket(socketType in_socket_type, int socket_num, genericSDRInterface *in_sdr_int);
	~portalDataSocket();

	//Methods inherited from hierarchicalDataflowBlock
	virtual void dataFromUpperLevel(void *data, int num_bytes, int local_up_channel=0);
	virtual void dataFromLowerLevel(void *data, int num_bytes, int local_down_channel=0);
private:
	genericSocketInterface *socket_int;
	genericSDRInterface *sdr_int;
};
