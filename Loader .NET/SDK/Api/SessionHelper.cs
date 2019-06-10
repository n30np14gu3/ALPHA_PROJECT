using System;
using System.Security.Cryptography;
using Loader.NET.SDK.Cryptography;

namespace Loader.NET.SDK.Api
{
    internal class SessionHelper:IDisposable
    {
        private RSACryptoServiceProvider _rsa;

        public SessionHelper()
        {
            _rsa = Rsa.GenerateRsa(4096);
        }


        public void Dispose()
        {
            _rsa?.Dispose();
        }
    }
}