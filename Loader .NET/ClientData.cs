using System;
using Loader.NET.SDK.Api;
using Loader.NET.SDK.Api.Structs;

namespace Loader.NET
{
    internal class ClientData
    {
        public static readonly DateTime LastUpdate = new DateTime(2029, 05, 03, 12, 12, 12);
        public const int GameId = 1;
        public static UserData Data = null;
        public static bool Logged = false;
        public static string ZipPassword = "";
        public static string AppDomain = "http://alpha-cheat.io";
    }
}