using System;
using System.IO;
using System.Security.Cryptography;
using System.Text;

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

        internal static string DecryptResponse(byte[] rsp)
        {

            string result;
            using (Rijndael cryptor = Rijndael.Create())
            {
                cryptor.Mode = CipherMode.CBC;

                cryptor.IV = Encoding.UTF8.GetBytes(Config.Iv);
                cryptor.Key = Encoding.UTF8.GetBytes(Config.Key);
                cryptor.Padding = PaddingMode.PKCS7;

                ICryptoTransform decryptor = cryptor.CreateDecryptor();
                using (MemoryStream msDecrypt = new MemoryStream(rsp))
                {
                    using (CryptoStream csDecrypt = new CryptoStream(msDecrypt, decryptor, CryptoStreamMode.Read))
                    {
                        using (StreamReader srDecrypt = new StreamReader(csDecrypt))
                        {
                            result = srDecrypt.ReadToEnd();
                        }
                    }
                }
            }
            return result;
        }

        internal static string EncryptResponse(string data)
        {

            string result;
            using (Rijndael cryptor = Rijndael.Create())
            {
                cryptor.Mode = CipherMode.CBC;

                cryptor.IV = Encoding.UTF8.GetBytes(Config.Iv);
                cryptor.Key = Encoding.UTF8.GetBytes(Config.Key);
                cryptor.Padding = PaddingMode.PKCS7;

                ICryptoTransform decryptor = cryptor.CreateEncryptor();
                using (MemoryStream msDecrypt = new MemoryStream())
                {
                    using (CryptoStream csDecrypt = new CryptoStream(msDecrypt, decryptor, CryptoStreamMode.Write))
                    {
                        using (StreamWriter srDecrypt = new StreamWriter(csDecrypt))
                        {
                            srDecrypt.Write(data);
                        }

                        result = BitConverter.ToString(msDecrypt.ToArray()).Replace("-", "");
                    }


                }
            }
            return result;
        }

    }
}