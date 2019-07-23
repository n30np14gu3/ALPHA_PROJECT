using System;

namespace Loader.NET.SDK.Api.Structs
{
    [Serializable]
    internal class ServerResponse<T>
    {
        public ServerCodes code;
        public T data;
    }
}