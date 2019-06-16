using System;
using System.Runtime.InteropServices;

namespace Loader.NET.SDK.Win32
{
    [StructLayout(LayoutKind.Sequential, Size = 116), Serializable]
    struct SERVER_RESPONSE
    {
        public int user_id;

        [MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst = 64)]
        public byte[] access_token;

        [MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst = 32)]
        public byte[] key;

        [MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst = 16)]
        public byte[] iv;
    }
}