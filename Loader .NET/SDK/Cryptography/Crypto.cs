using System;
using System.Security.Cryptography;

namespace Loader.NET.SDK.Cryptography
{
    internal class Crypto
    {

        internal static string Sha256(byte[] data) => BitConverter
            .ToString(new SHA256CryptoServiceProvider().ComputeHash(data)).Replace("-", "").ToLower();

        internal class Config
        {
            internal static string Key = "z*8cX4EDR7#xr|7iMQw*o5vdkeR4GIqC";
            internal static string Iv = "xL}oAK8sA1JYEO3I";
        }
    }
}