#region License
 
// --------------------------------------------------
// Copyright © OKB. All Rights Reserved.
//
// This software is proprietary information of OKB.
// USE IS SUBJECT TO LICENSE TERMS.
// --------------------------------------------------
 
#endregion
 
using System;
using System.Threading;
 
using Tasks;
using Tasks.Models;
 
namespace Main
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            Communicator communicator = new Communicator();
            Processor processor = new Processor();
            bool first = true;
            communicator.Connect("BOT");
 
            while (communicator.Client.Connected)
            {
                string response = communicator.ReadResponse();
                if (response != null)
                {
                    processor.UpdateGame(response);
                    if (processor.Game.Players != null)
                    {
                        foreach (PlayerModel playerModel in processor.Game.Players)
                        {
                            Console.WriteLine(playerModel.Id + ": " + playerModel.X + "," + playerModel.Y + " - " + processor.DistanceToPlayer(playerModel.Id));
                            //if (processor.DistanceToPlayer(playerModel.Id) < 5)
                            //{
                            //    communicator.SendCommand("BOMB");
                            //    Thread.Sleep(600);
                            //    communicator.SendCommand("UP");
                            //    communicator.SendCommand("RIGHT");
                            //    communicator.SendCommand("BOMB");
                            //    Thread.Sleep(600);
                            //    communicator.SendCommand("DOWN");
                            //    communicator.SendCommand("LEFT");
                            //}
                            //else
                            //{
                            //    if (first)
                            //    {
                            //        communicator.SendCommand("UP");
                            //        communicator.SendCommand("BOMB");
                            //        communicator.SendCommand("DOWN");
                            //        communicator.SendCommand("LEFT");
                            //        first = false;
                            //    }
                            //}
                        }
                    }
                }
            }
 
            //Exit code
            Console.WriteLine();
            Console.WriteLine("Where's the any key?");
 
            Console.ReadKey();
            communicator.Close();
        }
    }
}
