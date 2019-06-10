using System.Security.Cryptography;

namespace Loader.NET.SDK.Cryptography
{
    internal class Rsa
    {
        public static RSACryptoServiceProvider GenerateRsa(int bits)
        {
            return  new RSACryptoServiceProvider(4096);
        }
    }
}