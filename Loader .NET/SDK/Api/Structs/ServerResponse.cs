namespace Loader.NET.SDK.Api.Structs
{
    internal class ServerResponse<T>
    {
        public ServerCodes code;
        public T data;
        public string salt;
    }
}