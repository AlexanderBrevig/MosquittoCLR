#include "stdafx.h"

#include "MosquittoPlus.CLR.h"
#using <mscorlib.dll>
#include<iostream>

void __cdecl on_message(struct mosquitto * mosq, void * usrData, const mosquitto_message * message);
void __cdecl on_connect(struct mosquitto * mosq, void * usrData, int status);
void __cdecl on_disconnect(struct mosquitto * mosq, void * usrData, int status);


String^ MosquittoPlusCLR::MosquittoPlus::Version(){
	int major = 0;
	int minor = 0;
	int revision = 0;
	int ret = mosquitto_lib_version(&major, &minor, &revision);
	if (ret != MOSQ_ERR_SUCCESS){
		return nullptr;
	}
	std::cout << major << minor << revision << std::endl;
	String^ majorString = Convert::ToString(major);
	String^ minorString = Convert::ToString(minor);
	String^ revisionString = Convert::ToString(revision);

	return majorString + "." + minorString + "." + revisionString;
}

int MosquittoPlusCLR::MosquittoPlus::Connect(String^ gcClientName, String^ gcServerName, Int16^ port, Boolean^ gcCleanSession, Int16^ gcKeepalive) {
	IntPtr clientNamePtr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(gcClientName);
	char * clientName = (char*)(void*)clientNamePtr;

	mosquitto_lib_init();

	bool cleanSession = Convert::ToBoolean(gcCleanSession);
	mosq = mosquitto_new(clientName, cleanSession, 0);
	if (mosq == nullptr){
		System::Runtime::InteropServices::Marshal::FreeHGlobal(clientNamePtr);
		return MOSQ_ERR_NOT_FOUND;
	}

	IntPtr serverPtr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(gcServerName);
	char * serverName = (char*)(void*)serverPtr;
	int serverPort = Convert::ToInt16(port);
	int keepalive = Convert::ToInt16(gcKeepalive);
	int ret = mosquitto_connect(mosq, serverName, serverPort, keepalive);
	System::Runtime::InteropServices::Marshal::FreeHGlobal(serverPtr);

	if (ret != MOSQ_ERR_SUCCESS){
		return ret;
	}
	mosquitto_message_callback_set(mosq, on_message);
	mosquitto_connect_callback_set(mosq, on_connect);
}

int MosquittoPlusCLR::MosquittoPlus::Reconnect(){
	return mosquitto_reconnect(mosq);
}

int MosquittoPlusCLR::MosquittoPlus::Disconnect(){
	return mosquitto_disconnect(mosq);
}


int MosquittoPlusCLR::MosquittoPlus::Publish(String^ gcTopic, String^ gcMessage){
	return PublishHelper(gcTopic, gcMessage, 2, false);
}

int MosquittoPlusCLR::MosquittoPlus::Publish(String^ gcTopic, String^ gcMessage, Int16^ gcQOS, Boolean^ gcRetaion){
	int qos = Convert::ToInt16(gcQOS);
	int retain = Convert::ToBoolean(gcRetaion);
	return PublishHelper(gcTopic, gcMessage, qos, retain);
}

int MosquittoPlusCLR::MosquittoPlus::Subscribe(System::String^ gcTopic){
	IntPtr topicPtr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(gcTopic);
	char* topic = (char*)(void*)topicPtr;
	int ret = mosquitto_subscribe(mosq, 0, topic, 1);
	System::Runtime::InteropServices::Marshal::FreeHGlobal(topicPtr);
	return ret;
}

int MosquittoPlusCLR::MosquittoPlus::Unubscribe(System::String^ gcTopic){
	IntPtr topicPtr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(gcTopic);
	char* topic = (char*)(void*)topicPtr;
	int ret = mosquitto_unsubscribe(mosq, 0, topic);
	System::Runtime::InteropServices::Marshal::FreeHGlobal(topicPtr);
	return ret;
}

int MosquittoPlusCLR::MosquittoPlus::Update() {
	return mosquitto_loop(mosq, -1, 1);
}

int MosquittoPlusCLR::MosquittoPlus::Update(Int16^ gcTimeout) {
	int timeout = Convert::ToInt16(gcTimeout);
	return mosquitto_loop(mosq, timeout, 1);
}

int MosquittoPlusCLR::MosquittoPlus::Cleanup() {
	mosquitto_destroy(mosq);
	return mosquitto_lib_cleanup();
}

/// ==================== PRIVATE ==================================

int MosquittoPlusCLR::MosquittoPlus::PublishHelper(String^ gcTopic, String^ gcMessage, int qos, bool retain){
	IntPtr topicPtr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(gcTopic);
	IntPtr messagePtr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(gcMessage);

	char * tpc = (char*)(void*)topicPtr;
	char *msg = (char*)(void*)messagePtr;
	void* payload = (void*)msg;
	int len = strlen(msg);
	int ret = mosquitto_publish(mosq, 0, tpc, len, payload, qos, retain);

	System::Runtime::InteropServices::Marshal::FreeHGlobal(topicPtr);
	System::Runtime::InteropServices::Marshal::FreeHGlobal(messagePtr);

	return ret;
}

/// Callback helpers

void __cdecl on_message(struct mosquitto * mosq, void * usrData, const mosquitto_message * message){
	char *topic = message->topic;
	char msg[128] = {};
	memcpy(msg, message->payload, message->payloadlen);
	String^ topicGC = gcnew String(topic);
	String^ msgGC = gcnew String(msg);
	if (MosquittoPlusCLR::MosquittoPlus::OnMessage != nullptr){
		MosquittoPlusCLR::MosquittoPlus::OnMessage(topicGC, msgGC);
	}
}

void __cdecl on_connect(struct mosquitto * mosq, void * usrData, int status){
	if (MosquittoPlusCLR::MosquittoPlus::OnConnect != nullptr){
		switch (status){
		case 0:
			MosquittoPlusCLR::MosquittoPlus::OnConnect(Convert::ToInt16(status), Convert::ToString("ok"));
			break;
		case 1:
			MosquittoPlusCLR::MosquittoPlus::OnConnect(Convert::ToInt16(status), Convert::ToString("unacceptable protocol version"));
			break;
		case 2:
			MosquittoPlusCLR::MosquittoPlus::OnConnect(Convert::ToInt16(status), Convert::ToString("identifier rejected"));
			break;
		case 3:
			MosquittoPlusCLR::MosquittoPlus::OnConnect(Convert::ToInt16(status), Convert::ToString("broker unavailable"));
			break;

		}
	}
}

void __cdecl on_disconnect(struct mosquitto * mosq, void * usrData, int status){
	if (MosquittoPlusCLR::MosquittoPlus::OnDisconnect != nullptr){
		MosquittoPlusCLR::MosquittoPlus::OnDisconnect(Convert::ToInt16(status));
	}
}