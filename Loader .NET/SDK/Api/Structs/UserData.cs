using System;
using System.Collections.Generic;

namespace Loader.NET.SDK.Api.Structs
{
    [Serializable]
    internal class UserData
    {
        public string nickname;
        public string ip;
        public string reg_date;
        public int user_id;
        public string access_token;
        public bool lifetime_subscription;
        public List<SubscriptionModule> subscription_modules;
    }
}