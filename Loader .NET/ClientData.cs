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
        public static SessionHelper Session = null;

        internal static readonly string SessionKey =
            "5=N4t5SZH_!2Ugw4tctC*U-sMWw_QgT2^eP?nE9GL7JE3=qp?XRjF4ZS+XW$Bk8^^uPTj%AZv7SWmyDwS?2dC#3xJhb7venM&!N?";
    }
}