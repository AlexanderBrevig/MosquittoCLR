using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MosquittoPlusCLR;
namespace MosquittoPlus.CLI
{
    class Program
    {
        public class MQTTRoutes
        {
            public MQTTRoutes(MosquittoPlusCLR.MosquittoPlus mosq)
            {
                this.Mosquitto = mosq;
            }
            public Func<string, string, dynamic> this[string topic]
            {
                get
                {
                    if (!handlers.ContainsKey(topic)) {
                        return null;
                    }
                    return handlers[topic];
                }
                set
                {
                    if (value == null) {
                        handlers.Remove(topic);
                        Mosquitto.Unubscribe(topic);
                    } else if (!handlers.ContainsKey(topic)) {
                        Mosquitto.Subscribe(topic);
                        handlers[topic] = value;
                    }
                }
            }

            public MosquittoPlusCLR.MosquittoPlus Mosquitto { get; private set; }

            private Dictionary<string, Func<string, string, dynamic>> handlers = new Dictionary<string, Func<string, string, dynamic>>();
        }
        static void Main(string[] args)
        {
            var mosquitto = new MosquittoPlusCLR.MosquittoPlus();

            MQTTRoutes routes = new MQTTRoutes(mosquitto);

            MosquittoPlusCLR.MosquittoPlus.OnMessage = (topic, msg) => {
                Console.WriteLine("msg: " + topic + " " + msg);
                var callback = routes[topic];
                if (callback != null) {
                    callback(topic, msg);
                }
            };

            MosquittoPlusCLR.MosquittoPlus.OnConnect = (status, msg) => {
                Console.WriteLine("connect: " + status + " " + msg);
                string version = mosquitto.Version();
                Console.WriteLine("Version: " + version);
                mosquitto.Publish("/sensors/123/temp", "some message");
            };
            MosquittoPlusCLR.MosquittoPlus.OnDisconnect = (status) => {
                Console.WriteLine("disconnect: " + status);
            };


            mosquitto.Connect("testclient", "localhost", 1883, true, 60);


            routes["/test/this"] = (topic, msg) => {
                Console.WriteLine("got handler woho ! " + topic + ": " + msg);
                return null;
            };

            while (true) {
                mosquitto.Update();
            }
        }
    }
}
