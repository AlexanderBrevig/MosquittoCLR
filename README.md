MosquittoCLR
============

*NB: The structure of this project will change when I'm closer to something that can be reused.*
For now, the names of the projects just happened to be like this. Expect changes.

A CLR wrapper around the http://mosquitto.org/ project

* Include a reference to the MosquittoPlus.CLR project
* Create an instance of the MosquittoPlus
  * `var mosquitto = new MosquittoPlusCLR.MosquittoPlus();`
* Define an OnMessage callback handler 
  * `MosquittoPlusCLR.MosquittoPlus.OnMessage = (topic,msg)=>{Console.WriteLine("msg:"+topic+"->"+msg);};`
* Connect to a broker
  * `mosquitto.Connect("testclient", "localhost", 1883, true, 60);`
* Subscribe to topics
  * `mosquitto.Publish("/topic/a")`
* Publish messages
  * `mosquitto.Publish("/topic", "message", 2, false);`
   
    
    
