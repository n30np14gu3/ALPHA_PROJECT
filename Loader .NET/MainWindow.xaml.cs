using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Loader.NET.SDK.Api;
using Loader.NET.SDK.Api.Structs;
using Loader.NET.SDK.Cryptography;
using Loader.NET.SDK.Win32;
using MaterialDesignThemes.Wpf;
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
        private int _csgoPid = 0;

        public MainWindow()
        {
            InitializeComponent();
            SessionHelper helper = new SessionHelper(1024);
            helper.RequestKeys();
            new AuthWindow().ShowDialog();
            InitializeComponent();


            UserInfo.Items.Add(new TextBlock {Text = $"Имя пользователя: {ClientData.Data?.nickname}"});
            UserInfo.Items.Add(new TextBlock {Text = $"IP: {ClientData.Data?.ip}"});
            UserInfo.Items.Add(new TextBlock {Text = $"Дата регистрации: {ClientData.Data?.reg_date}"});

            if (ClientData.Data != null)
            {
                foreach (SubscriptionModule module in ClientData.Data.subscription_modules)
                {
                    SubscriptionComponents.Items.Add(new TextBlock
                        { Text = $"{module.name} [истекает: {module.end_date}]" });
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
            await Task.Run(() => runCsGo());
            await Task.Run(() => downloadLibs());
            await Task.Run(() => injectLibs());
            LaunchBtn.IsEnabled = true;

        }

        void downloadLibs()
        {
            try
            {
                using (HttpRequest request = new HttpRequest {IgnoreProtocolErrors = true})
                {
                    RequestParams data = new RequestParams();
                    data["access_token"] = ClientData.Data.access_token;
                    data["game_id"] = ClientData.GameId;

                    string rsp = request.Post($"{ClientData.AppDomain}/api/request_dll", data).ToString();
                    rsp = Aes.DecryptResponse(Convert.FromBase64String(rsp));
                    List<string> libs = JsonConvert.DeserializeObject<List<string>>(rsp);
                    _dlls.Clear();
                    foreach (string lib in libs)
                    {
                        _dlls.Add(Convert.FromBase64String(Encoding.UTF8.GetString(Crypto.FromHex(lib))));
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"ОШИБКА!\n{ex.Message}", "ERROR", MessageBoxButton.OK, MessageBoxImage.Error);
                _dlls.Clear();
            }
        }

        void injectLibs()
        {
            try
            {
                foreach (byte[] raw in _dlls)
                {
                    if (!ManualMapInjector.InjectDll(raw, _csgoPid))
                        throw new Exception("Не удалось запустить чит!");

                    Thread.Sleep(1000);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "ERROR", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        void runCsGo()
        {
            if (Process.GetProcessesByName("csgo").Length == 0)
            {
                Process.Start("steam://rungameid/730");
                while (Process.GetProcessesByName("csgo").Length == 0) { }
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


            _csgoPid = Process.GetProcessesByName("csgo")[0].Id;
        }
    }
}
