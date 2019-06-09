namespace Loader.NET.SDK.Api
{
    internal class ServerResponse<T>
    {
        public ServerCodes code;
        public T data;
        public string salt;
    }
}