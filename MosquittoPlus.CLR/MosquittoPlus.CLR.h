// MosquittoPlus.CLR.h

#pragma once

#include<string>
using namespace System;

extern "C" {
#include "mosquitto.h"
}

namespace MosquittoPlusCLR {

	public ref class MosquittoPlus
	{
	public:
		MosquittoPlus(){
			//nothing for now
		}
		~MosquittoPlus(){
			Cleanup();
		}

		/*
		* Get a verison string MAJOR.MINOR.REVISION
		*/
		String^ Version();

		/*
		* Connect to an MQTT broker.
		*
		* Parameters:
		* 	id -        String to use as the client id. If NULL, a random client id
		* 	            will be generated. If id is NULL, clean_session must be true.
		* 	server -    the hostname or ip address of the broker to connect to.
		* 	port -      the network port to connect to. Usually 1883.
		* 	cleanSession - set to true to instruct the broker to clean all messages
		*                  and subscriptions on disconnect, false to instruct it to
		*                  keep them. See the man page mqtt(7) for more details.
		*                  Note that a client will never discard its own outgoing
		*                  messages on disconnect. Calling <mosquitto_connect> or
		*                  <mosquitto_reconnect> will cause the messages to be resent.
		*                  Use <mosquitto_reinitialise> to reset a client to its
		*                  original state.
		*                  Must be set to true if the id parameter is NULL.
		* 	keepalive - the number of seconds after which the broker should send a PING
		*              message to the client if no other messages have been exchanged
		*              in that time.
		*
		* Returns:
		* 	MOSQ_ERR_SUCCESS - on success.
		* 	MOSQ_ERR_INVAL -   if the input parameters were invalid.
		* 	MOSQ_ERR_ERRNO -   if a system call returned an error. The variable errno
		*                     contains the error code, even on Windows.
		*                     Use strerror_r() where available or FormatMessage() on
		*                     Windows.
		*/
		int Connect(String^ clientName, System::String^ server, Int16^ port, Boolean^ cleanSession, Int16^ keepalive);
		
		/*
		* Reconnect to a broker. (default QOS = 2, no retain)
		*
		* This function provides an easy way of reconnecting to a broker after a
		* connection has been lost. It uses the values that were provided in the
		* <Connect> call. 
		*
		* Returns:
		* 	MOSQ_ERR_SUCCESS - on success.
		* 	MOSQ_ERR_INVAL -   if the input parameters were invalid.
		* 	MOSQ_ERR_NOMEM -   if an out of memory condition occurred.
		*
		* Returns:
		* 	MOSQ_ERR_SUCCESS - on success.
		* 	MOSQ_ERR_INVAL -   if the input parameters were invalid.
		* 	MOSQ_ERR_ERRNO -   if a system call returned an error. The variable errno
		*                     contains the error code, even on Windows.
		*                     Use strerror_r() where available or FormatMessage() on
		*                     Windows.
		*/
		int Reconnect();

		/*
		* Disconnect from the broker.
		*
		* Returns:
		*	MOSQ_ERR_SUCCESS - on success.
		* 	MOSQ_ERR_INVAL -   if the input parameters were invalid.
		* 	MOSQ_ERR_NO_CONN -  if the client isn't connected to a broker.
		*/
		int Disconnect();

		/*
		* Publish a message on a given topic.
		*
		* Parameters:
		* 	topic - the topic to publish to
		*   message - the message to send
		*
		* Returns:
		* 	MOSQ_ERR_SUCCESS -      on success.
		* 	MOSQ_ERR_INVAL -        if the input parameters were invalid.
		* 	MOSQ_ERR_NOMEM -        if an out of memory condition occurred.
		* 	MOSQ_ERR_NO_CONN -      if the client isn't connected to a broker.
		*	MOSQ_ERR_PROTOCOL -     if there is a protocol error communicating with the
		*                          broker.
		* 	MOSQ_ERR_PAYLOAD_SIZE - if payloadlen is too large.
		*/
		int Publish(String^ topic, String^ message);

		/*
		* Publish a message on a given topic.
		*
		* Parameters:
		* 	topic - the topic to publish to
		*   message - the message to send
		* 	qos -        integer value 0, 1 or 2 indicating the Quality of Service to be
		*               used for the message.
		* 	retain -     set to true to make the message retained.
		*
		* Returns:
		* 	MOSQ_ERR_SUCCESS -      on success.
		* 	MOSQ_ERR_INVAL -        if the input parameters were invalid.
		* 	MOSQ_ERR_NOMEM -        if an out of memory condition occurred.
		* 	MOSQ_ERR_NO_CONN -      if the client isn't connected to a broker.
		*	MOSQ_ERR_PROTOCOL -     if there is a protocol error communicating with the
		*                          broker.
		* 	MOSQ_ERR_PAYLOAD_SIZE - if payloadlen is too large.
		*/
		int Publish(String^ topic, String^ message, Int16^ qos, Boolean^ retain);
		
		/*
		* Subscribe to a topic.
		*
		* Parameters:
		*	topic - the topic to subscribe to
		*
		* Returns:
		*	MOSQ_ERR_SUCCESS - on success.
		* 	MOSQ_ERR_INVAL -   if the input parameters were invalid.
		* 	MOSQ_ERR_NOMEM -   if an out of memory condition occurred.
		* 	MOSQ_ERR_NO_CONN - if the client isn't connected to a broker.
		*/
		int Subscribe(System::String^ topic);

		/*
		* Unsubscribe from a topic.
		*
		* Parameters:
		*	topic - the topic to unsubscribe from
		*
		* Returns:
		*	MOSQ_ERR_SUCCESS - on success.
		* 	MOSQ_ERR_INVAL -   if the input parameters were invalid.
		* 	MOSQ_ERR_NOMEM -   if an out of memory condition occurred.
		* 	MOSQ_ERR_NO_CONN - if the client isn't connected to a broker.
		*/
		int Unubscribe(System::String^ topic);

		/*
		* The main network loop for the client. You must call this frequently in order
		* to keep communications between the client and broker working. If incoming
		* data is present it will then be processed. Outgoing commands, from e.g.
		* <Publish>, are normally sent immediately that their function is
		* called, but this is not always possible.
		*
		* Returns:
		*	MOSQ_ERR_SUCCESS -   on success.
		* 	MOSQ_ERR_INVAL -     if the input parameters were invalid.
		* 	MOSQ_ERR_NOMEM -     if an out of memory condition occurred.
		* 	MOSQ_ERR_NO_CONN -   if the client isn't connected to a broker.
		*   MOSQ_ERR_CONN_LOST - if the connection to the broker was lost.
		*	MOSQ_ERR_PROTOCOL -  if there is a protocol error communicating with the
		*                       broker.
		* 	MOSQ_ERR_ERRNO -     if a system call returned an error. The variable errno
		*                       contains the error code, even on Windows.
		*                       Use strerror_r() where available or FormatMessage() on
		*                       Windows.
		*/
		int Update();

		/*
		* The main network loop for the client. You must call this frequently in order
		* to keep communications between the client and broker working. If incoming
		* data is present it will then be processed. Outgoing commands, from e.g.
		* <Publish>, are normally sent immediately that their function is
		* called, but this is not always possible. 
		*
		* Parameters:
		*	timeout -     Maximum number of milliseconds to wait for network activity
		*	              in the select() call before timing out. Set to 0 for instant
		*	              return.  Set negative to use the default of 1000ms.
		*
		* Returns:
		*	MOSQ_ERR_SUCCESS -   on success.
		* 	MOSQ_ERR_INVAL -     if the input parameters were invalid.
		* 	MOSQ_ERR_NOMEM -     if an out of memory condition occurred.
		* 	MOSQ_ERR_NO_CONN -   if the client isn't connected to a broker.
		*   MOSQ_ERR_CONN_LOST - if the connection to the broker was lost.
		*	MOSQ_ERR_PROTOCOL -  if there is a protocol error communicating with the
		*                       broker.
		* 	MOSQ_ERR_ERRNO -     if a system call returned an error. The variable errno
		*                       contains the error code, even on Windows.
		*                       Use strerror_r() where available or FormatMessage() on
		*                       Windows.
		*/
		int Update(Int16^ timeout);

		/*
		* Call to free resources associated with the library.
		*
		* Returns:
		* 	MOSQ_ERR_SUCCESS - always
		*/
		int Cleanup();

		static System::Action<System::String^, System::String^>^ OnMessage;
		static System::Action<System::Int16, System::String^>^ OnConnect;
		static System::Action<System::Int16>^ OnDisconnect;
	private:
		int PublishHelper(String^ topic, String^ message, int qos, bool retain);
		mosquitto* mosq;
	};
}
