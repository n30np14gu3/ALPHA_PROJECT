using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Windows;
using Newtonsoft.Json;

namespace Loader.NET.SDK.Api
{
    internal class LocalProto : IDisposable
    {
        [Serializable]
        private struct EndResponse
        {
            public int completed;
        }

        private byte[] _pKey;
        private TcpListener _listener;
        private Socket _cheat;

        public LocalProto(int port)
        {
            _pKey = new byte[8];
            IPAddress ip = IPAddress.Parse("127.0.0.1");
            _listener = new TcpListener(ip, port);
            _listener.Start();
            _cheat = _listener.AcceptSocket();
            _cheat.Receive(_pKey);
        }


        public bool SendJson<T>(T jsonObj)
        {
            List<byte> json = Encoding.ASCII.GetBytes(JsonConvert.SerializeObject(jsonObj)).ToList();
            json.Add(0);

#if DEBUG
            MessageBox.Show($"SIZE SENDING\nN={json.Count}");
#endif
            if (Send(BitConverter.GetBytes(json.Count)) != 4)
                return false;

#if DEBUG
            MessageBox.Show($"DATA SENDING\n{Encoding.UTF8.GetString(json.ToArray())}");
#endif
            return Send(json.ToArray()) == json.Count;
        }

        public bool AwaitExit()
        {
            EndResponse ersp = ReciveJson<EndResponse>();
            return ersp.completed == _pKey.Sum(x => x * 3);
        }

        public void Dispose()
        {
            Array.Clear(_pKey, 0, _pKey.Length);
            _cheat.Close();
            _listener.Stop();

        }

        public int Send(byte[] bytes)
        {
            for (int i = 0; i < bytes.Length; i++)
                bytes[i] ^= _pKey[i % 8];

            return _cheat.Send(bytes);
        }

        public byte[] Recive()
        {
            byte[] packetLengthBytes = new byte[4];
            int packetLength;
            _cheat.Receive(packetLengthBytes);
            for (int i = 0; i < packetLengthBytes.Length; i++)
                packetLengthBytes[i] ^= _pKey[i % 8];

            packetLength = BitConverter.ToInt32(packetLengthBytes, 0);

            byte[] recv = new byte[packetLength];
            _cheat.Receive(recv);
            for (int i = 0; i < packetLength; i++)
                recv[i] ^= _pKey[i % 8];
            return recv;
        }

        public T ReciveJson<T>()
        {
            string json = Encoding.ASCII.GetString(Recive()).Replace("\0", "");
            return JsonConvert.DeserializeObject<T>(json);
        }
    }
}