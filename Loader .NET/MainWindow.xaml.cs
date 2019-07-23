using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using Loader.NET.SDK.Api;
using Loader.NET.SDK.Api.Structs;
using Loader.NET.SDK.Cryptography;
using Loader.NET.SDK.Win32;
using Newtonsoft.Json;
using xNet;


namespace Loader.NET
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private bool _isDrag;
        private List<byte[]> _dlls = new List<byte[]>();
        private int _csgoPid;

        public MainWindow()
        {
            InitializeComponent();
            new AuthWindow().ShowDialog();
            InitializeComponent();


            UserInfo.Items.Add(new TextBlock {Text = $"Имя пользователя: {ClientData.Data?.nickname}"});
            UserInfo.Items.Add(new TextBlock {Text = $"Дата регистрации: {ClientData.Data?.reg_date}"});

            if (ClientData.Data != null)
            {
                foreach (SubscriptionModule module in ClientData.Data.subscription_modules)
                {
                    SubscriptionComponents.Items.Add(new TextBlock
                        { Text = $"{module.name} [истекает: {(ClientData.Data.lifetime_subscription ? "Навсегда" : module.end_date)}]" });
                }
            }
        }

        private void DragHeader_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            _isDrag = true;
            DragMove();
        }

        private void DragHeader_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            _isDrag = false;
        }

        private void DragHeader_MouseMove(object sender, MouseEventArgs e)
        {
            if (_isDrag && WindowState == WindowState.Maximized)
            {
                _isDrag = false;
                WindowState = WindowState.Normal;
            }
        }

        private void CloseBtn_Click(object sender, RoutedEventArgs e)
        {
            Environment.Exit(0);
        }

        private void MinBtn_Click(object sender, RoutedEventArgs e)
        {
            WindowState = WindowState.Minimized;
        }

        private void BrandLink_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            Process.Start(ClientData.AppDomain);
        }

        private async void LaunchBtn_Click(object sender, RoutedEventArgs e)
        {
            LaunchBtn.IsEnabled = false;
            try
            {
                showMessage("Подождите, происходит запуск...");
                await Task.Run(runCsGo);
                await Task.Run(downloadLibs);
                await Task.Run(injectLibs);
                await Task.Run(cheatSign);
                showMessage("Запуск произошел успешно!");
            }
            catch (Exception ex)
            {
                showMessage($"Произошла ошибка: {ex.Message}");
            }
        }

        void downloadLibs()
        {
            using (HttpRequest request = new HttpRequest { IgnoreProtocolErrors = true })
            {
                RequestParams data = new RequestParams();
                data["access_token"] = ClientData.Data.access_token;
                data["game_id"] = ClientData.GameId;

                string rsp = request.Post($"{ClientData.AppDomain}/api/request_dll", data).ToString();
                List<string> libs = JsonConvert.DeserializeObject<List<string>>(rsp);
                _dlls.Clear();
                foreach (string lib in libs)
                {
                    _dlls.Add(Convert.FromBase64String(Encoding.UTF8.GetString(Crypto.FromHex(lib))));
                }

                _dlls = _dlls.OrderBy(x => x.Length).ToList();
            }
        }

        private void cheatSign()
        {
            using (LocalProto proto = new LocalProto(4034))
            {
                CheatSendInfo csi = new CheatSendInfo {access_token = ClientData.Data.access_token, user_id = ClientData.Data.user_id};
                if (!proto.SendJson(csi))
                {
                    throw new Exception("Не удалось произвести подпись чита.");
                }

                if(!proto.AwaitExit())
                    throw new Exception("Не удалось подтвердить запуск чита.");
            }
        }

        void injectLibs()
        {
            string tmpFile = "";
            try
            {
#if DEBUG
                if (MessageBox.Show("Use CE?", "", MessageBoxButton.YesNo) == MessageBoxResult.Yes)
                    goto ce_lbl;
#endif
                foreach (byte[] raw in _dlls)
                {
                    tmpFile = Path.Combine(Path.GetTempPath(), $"{Path.GetRandomFileName()}.dll");
                    File.WriteAllBytes(tmpFile, raw);
                    if (!Injector.ManualMapInject(_csgoPid, tmpFile, true))
                    {
                        if (File.Exists(tmpFile))
                            File.Delete(tmpFile);
                        throw new Exception("Не удалось запустить чит");
                    }
                }

#if DEBUG
            ce_lbl:
                MessageBox.Show("PRESS AFTER INJECT");
#endif
            }
            catch(Exception ex)
            {
                if (File.Exists(tmpFile))
                    File.Delete(tmpFile);
                showMessage(ex.Message);
            }
            finally
            {
                _dlls.Clear();
                GC.Collect();
                GC.WaitForPendingFinalizers();
            }
        }

        void runCsGo()
        {
            repeat:
            try
            {
                if (Process.GetProcessesByName("csgo").Length == 0)
                {
                    Process.Start("steam://rungameid/730");
                    while (Process.GetProcessesByName("csgo").Length == 0)
                    {
                    }
                }

                kek:
                int loadedModules = 0;
                foreach (ProcessModule module in Process.GetProcessesByName("csgo")[0].Modules)
                {
                    if (module.ModuleName == "client_panorama.dll")
                        loadedModules++;

                    if (module.ModuleName == "engine.dll")
                        loadedModules++;

                    if (module.ModuleName == "server.dll")
                        loadedModules++;
                }

                if (loadedModules != 3)
                    goto kek;
            }
            catch
            {
                goto repeat;
            }
            _csgoPid = Process.GetProcessesByName("csgo")[0].Id;
        }

        private void showMessage(string messageText)
        {
            Dispatcher.Invoke(() =>
            {
                InfoMessage_Text.Content = messageText;
                InfoMessage.IsActive = true;
            });
        }

        private void LLogout_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            Properties.Settings.Default.Reset();
            Environment.Exit(0);
        }

        private void InfoMessage_Text_ActionClick(object sender, RoutedEventArgs e)
        {
            InfoMessage.IsActive = false;
        }
    }
}
