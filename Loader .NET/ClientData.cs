using System;
using Loader.NET.SDK.Api.Structs;

namespace Loader.NET
{
    internal class ClientData
    {
        public static readonly DateTime LastUpdate = new DateTime(2019, 7, 23, 3, 0, 0);
        public const int GameId = 1;
        public static UserData Data = null;
        public static bool Logged = false;
        public static string AppDomain = " https://alphacheat.com";
    }
}