using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows;

namespace SKProjekt
{
    class ServerConnection
    {
        public static int Port { get; set; } = 1234;

        public static string IP { get; set; }
        public static TcpClient client { get; set; }
        public static NetworkStream stream { get; set; }
        public static string Username { get; set; } = "Default";

        public static void GetConnection(string ip = "192.168.56.103")
        {
            IP = ip;
            client = new TcpClient(IP, Port);
            stream = client.GetStream();
        }

        public static void SendToServer(string message, int size = 200)
        {
            Byte[] data2 = System.Text.Encoding.ASCII.GetBytes(message);
            Byte[] data = new byte[size];
            Array.Copy(data2, data, data2.Length);
            stream.Write(data, 0, data.Length);
        }

        public static String ReceiveFromServer(int size = 200)
        {
            Byte[] data = new byte[size];
            Int32 bytes = stream.Read(data, 0, size);
            String responseData = System.Text.Encoding.ASCII.GetString(data, 0, bytes);
            for(int i = 1; i < bytes; i++)
            {
                if(responseData[i] == '\x0')
                {
                    bytes = i;
                    break;
                }
            }
            responseData = responseData.Substring(0, bytes);
            return responseData;
        }


        public static void SendMessage(string message, string room)
        {
            SendToServer("newMsg");
            ReceiveFromServer();
            SendToServer(room);
            SendToServer(Username);
            SendToServer(message);

        }

        public static List<string> GetRoomList()
        {
            SendToServer("listPok");
            List<string> roomList = new List<string>();
            ReceiveFromServer();

            while (true)
            {
                string temp = ReceiveFromServer();
                if(temp[0] == '\x0')
                { break;  }
                else
                {
                    roomList.Add(temp);
                }
            } 

            return roomList;
        }

        public static string GetRoomMessages(string room)
        {
            string ret = "";

            SendToServer("refresh");
            ReceiveFromServer();
            SendToServer(room);
            SendToServer("0");

            while (true)
            {
                string temp = ReceiveFromServer();
                if (temp[0] == '\x0')
                { break; }
                else
                {
                    ret += temp;
                }
            }
            ReceiveFromServer();

            return ret;
        }

        public static void EnterRoom(string room)
        {
            SendToServer("onRoom");
            ReceiveFromServer();
            SendToServer(room);
            ReceiveFromServer();
        }

        public static void LeaveRoom(string room)
        {
            SendToServer("clRoom");
            ReceiveFromServer();
            SendToServer(room);
        }

        public static void NewRoom(string room)
        {
            SendToServer("newRoom");
            ReceiveFromServer();
            SendToServer(room);
        }
    }
}
