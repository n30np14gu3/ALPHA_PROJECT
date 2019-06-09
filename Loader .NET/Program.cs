using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Reflection;
using System.Windows;
using Loader.NET.SDK.Api;
using Loader.NET.SDK.Cryptography;
using xNet;
using Newtonsoft.Json;

namespace Loader.NET
{
    public class Program
    {
        static Dictionary<string, Assembly> assembliesDictionary = new Dictionary<string, Assembly>();

        [STAThread]
        public static void Main()
        {
#if !DEBUG
            AppDomain.CurrentDomain.AssemblyResolve += OnResolveAssembly;
#endif
            try
            {
                using (HttpRequest request = new HttpRequest {IgnoreProtocolErrors = true})
                {
                    RequestParams data = new RequestParams();
                    data["game_id"] = ClientData.GameId;
                    string rsp = request.Post($"{ClientData.AppDomain}/api/request_updates", data).ToString();
                    rsp = Crypto.DecryptResponse(Convert.FromBase64String(rsp));
                    ServerResponse<UpdateInfo> updateInfo = JsonConvert.DeserializeObject<ServerResponse<UpdateInfo>>(rsp);
                    switch (updateInfo.code)
                    {
                        case ServerCodes.API_CODE_GAME_NOT_FOUND:
                            throw new Exception("Игра не найдена!");

                        case ServerCodes.API_CODE_GAME_DISABLED:
                            throw new Exception("Лоадер для данной игры приостановлен!");

                        case ServerCodes.API_CODE_OK:
                            if (DateTime.Parse(updateInfo.data.last_update) > ClientData.LastUpdate)
                            {
                                MessageBox.Show(
                                    "Доступно новое обновление! Подождите, пока мастер обновления закончит работу!", "NEW UPDATE FOUNDED!", MessageBoxButton.OK, MessageBoxImage.Asterisk);
                                Environment.Exit(0);
                            }
                            break;
                    }
                }
            }
            catch (Exception e)
            {
                MessageBox.Show($"НЕ УДАЛОСЬ ПРОВЕРИТЬ ОБНОВЛЕНИЯ!\r\n{e.Message}", "ERROR", MessageBoxButton.OK, MessageBoxImage.Error);
                Environment.Exit(0);
            }

            App.Main();

        }

        private static Assembly OnResolveAssembly(object sender, ResolveEventArgs args)
        {
            AssemblyName assemblyName = new AssemblyName(args.Name);
            Assembly executingAssembly = Assembly.GetExecutingAssembly();
            string path = string.Format("{0}.dll", assemblyName.Name);

            if (assemblyName.CultureInfo.Equals(CultureInfo.InvariantCulture) == false)
            {
                path = String.Format(@"{0}\{1}", assemblyName.CultureInfo, path);
            }

            if (!assembliesDictionary.ContainsKey(path))
            {
                using (Stream assemblyStream = executingAssembly.GetManifestResourceStream(path))
                {
                    if (assemblyStream != null)
                    {
                        var assemblyRawBytes = new byte[assemblyStream.Length];
                        assemblyStream.Read(assemblyRawBytes, 0, assemblyRawBytes.Length);
                        using (var pdbStream = executingAssembly.GetManifestResourceStream(Path.ChangeExtension(path, "pdb")))
                        {
                            if (pdbStream != null)
                            {
                                var pdbData = new Byte[pdbStream.Length];
                                pdbStream.Read(pdbData, 0, pdbData.Length);
                                var assembly = Assembly.Load(assemblyRawBytes, pdbData);
                                assembliesDictionary.Add(path, assembly);
                                return assembly;
                            }
                        }
                        assembliesDictionary.Add(path, Assembly.Load(assemblyRawBytes));
                    }
                    else
                    {
                        assembliesDictionary.Add(path, null);
                    }
                }
            }
            return assembliesDictionary[path];
        }
    }
}