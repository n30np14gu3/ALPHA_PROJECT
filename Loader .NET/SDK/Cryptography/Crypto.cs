using System;
using System.Security.Cryptography;

namespace Loader.NET.SDK.Cryptography
{
    internal class Crypto
    {
        internal static byte[] FromHex(string hex)
        {
            byte[] raw = new byte[hex.Length / 2];
            for (int i = 0; i < raw.Length; i++)
            {
                raw[i] = Convert.ToByte(hex.Substring(i * 2, 2), 16);
            }
            return raw;
        }

        internal static string Sha256(byte[] data) => BitConverter
            .ToString(new SHA256CryptoServiceProvider().ComputeHash(data)).Replace("-", "").ToLower();


    }
}