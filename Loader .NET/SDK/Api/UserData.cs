using System.Collections.Generic;

namespace Loader.NET.SDK.Api
{
    internal class UserData
    {
        public string nickname;
        public int user_id;
        public string access_token;
        public List<SubscriptionModule> subscription_modules;
    }
}