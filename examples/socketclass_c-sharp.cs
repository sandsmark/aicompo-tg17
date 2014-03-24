#region License
 
// --------------------------------------------------
// Demonstration code for AI-compo gathering.org 2014
// By: Patrick Tørresvold, 2014
// Twitter: @2rsvold
// --------------------------------------------------
 
#endregion
 
using System;
using System.IO;
using System.Net.Sockets;
using System.Text;
using System.Threading;
 
namespace Example
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            // Our TCP client that we use to communicate with the server
            TcpClient Client = new TcpClient();
 
            // Some default variables
            const string host = "localhost";
            const int port = 54321;
 
            // Let's try to connect to the server
            try
            {
                Console.Write("Connecting: ");
 
                //Connect to the server using our TCP client
                Client.Connect("localhost", 54321);
 
                // Give the server some time incase delay
                Thread.Sleep(100);
 
                // If we are connected
                if (Client.Connected)
                {
                    // Create a stream writer based on the connection we allready have done
                    Stream Writer = Client.GetStream();
 
                    // Create a reader with UTF8 encoding to read from the connection we allready have
                    StreamReader Reader = new StreamReader(Client.GetStream(), Encoding.UTF8);
                    Console.WriteLine("Connected!");
 
                    // Convert our string to bytes so we can send it back to the connection
                    ASCIIEncoding asen = new ASCIIEncoding();
                    byte[] data = asen.GetBytes("NAME ExampleBot"); // See documentation for more commands
 
                    // Write (aka send the string) to the server
                    Writer.Write(data, 0, data.Length);
 
                    // While we are connected, just dump the result to the console
                    while (Client.Connected)
                        Console.WriteLine("Response: " + Reader.ReadLine());
 
                    // NOTE: It's in this "while"-loop you would do your processing of the response and calculate something smart to do next
                    // There is JSON-support in this application, and I would highly recommend that you take a look at "Newtonsoft.JSON"
                    // and see how you can easly convert a json-response into a model of your liking for easier processing :)
                }
            }
            catch (Exception e) // If we fail to connect: Say so!
            {
                Console.WriteLine("Unable to connect to " + host + ":" + port);
            }
 
            Console.WriteLine("Where's the any key?");
            Console.ReadKey();
        }
    }
}
