using System;
using System.IO;
using System.Text;
using Loader.NET.SDK.Api.Structs;
using Loader.NET.SDK.Cryptography;
using Newtonsoft.Json;
using Org.BouncyCastle.Crypto;
using Org.BouncyCastle.Crypto.Encodings;
using Org.BouncyCastle.Crypto.Engines;
using Org.BouncyCastle.Crypto.Generators;
using Org.BouncyCastle.Crypto.Prng;
using Org.BouncyCastle.OpenSsl;
using Org.BouncyCastle.Security;
using xNet;

namespace Loader.NET.SDK.Api
{

    internal class SessionHelper
    {
        private AsymmetricCipherKeyPair _keys;


        public SessionHelper(int bits)
        {
            RsaKeyPairGenerator kpgen = new RsaKeyPairGenerator();
            kpgen.Init(new KeyGenerationParameters(new SecureRandom(new CryptoApiRandomGenerator()), bits));
            _keys = kpgen.GenerateKeyPair();
        }

        public void RequestKeys()
        {
            using (HttpRequest request = new HttpRequest())
            {
                string pubKey;
                StringBuilder pubBuider = new StringBuilder("");
                using (StringWriter sw = new StringWriter(pubBuider))
                {
                    new PemWriter(sw).WriteObject(_keys.Public);
                }

                pubKey = pubBuider.ToString();
                string hash = Crypto.Sha256(Encoding.UTF8.GetBytes($"{pubKey}{ClientData.SessionKey}"));
                RequestParams data = new RequestParams();
                data["uid"] = Convert.ToBase64String(Encoding.UTF8.GetBytes(pubKey));
                data["sign"] = hash;

                string rsp = request.Post($"{ClientData.AppDomain}/api/request_session", data).ToString();
                ServerResponse<SessionData> session = JsonConvert.DeserializeObject<ServerResponse<SessionData>>(rsp);
                if(Crypto.Sha256(Encoding.UTF8.GetBytes($"{session.data.raw}{ClientData.SessionKey}")) != session.data.crc)
                    Environment.Exit(0);
                byte[] encData = Convert.FromBase64String(session.data.raw);
                var decryptEngine = new Pkcs1Encoding(new RsaEngine());
                decryptEngine.Init(false, _keys.Private);
                rsp = Encoding.UTF8.GetString(decryptEngine.ProcessBlock(encData, 0, encData.Length));
                string[] keys = JsonConvert.DeserializeObject<string[]>(rsp);
                Crypto.Config.Key = keys[0];
                Crypto.Config.Iv = keys[1];
            }
        }
    }
}