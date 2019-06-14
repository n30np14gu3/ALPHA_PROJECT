using System;

namespace Loader.NET.SDK.Api.Structs
{
    [Serializable]
    internal enum ServerCodes : uint
    {
        API_CODE_UNKNOWN_ERROR = 300,

        API_CODE_USER_NOT_FOUND,
        API_CODE_SUBSCRIPTION_DUPLICATE,
        API_CODE_SUBSCRIPTION_EXPIRY,
        API_CODE_USER_BLOCKED,
        API_CODE_HWID_ERROR,

        API_CODE_GAME_NOT_FOUND,
        API_CODE_GAME_DISABLED,
        API_CODE_OK = 100,
    }
}