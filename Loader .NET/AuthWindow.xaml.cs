using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using Loader.NET.SDK.Api;
using Loader.NET.SDK.Cryptography;
using Loader.NET.SDK.Device;
using MaterialDesignThemes.Wpf;
using Newtonsoft.Json;
using xNet;

namespace Loader.NET
{
    /// <summary>
    /// Interaction logic for AuthWindow.xaml
    /// </summary>
    public partial class AuthWindow : Window
    {
        private bool _isDrag;

        public AuthWindow()
        {
            InitializeComponent();
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
            Close();
        }

        private void MinBtn_Click(object sender, RoutedEventArgs e)
        {
            WindowState = WindowState.Minimized;
        }

        private void AuthBtn_Click(object sender, RoutedEventArgs e)
        {
            if (string.IsNullOrWhiteSpace(TEmail.Text) || string.IsNullOrWhiteSpace(TPassword.Password))
                return;

            AuthBtn.IsEnabled = false;
            TEmail.IsEnabled = false;
            TPassword.IsEnabled = false;

            using (HttpRequest request = new HttpRequest())
            {
                try
                {
                    RequestParams data = new RequestParams();
                    data["email"] = TEmail.Text;
                    data["password"] = Crypto.Sha256(Encoding.UTF8.GetBytes(TPassword.Password));
                    data["hwid"] = HWID.GetSign(TEmail.Text);
                    data["game_id"] = ClientData.GameId;

                    string result = request.Post($"{ClientData.AppDomain}/api/login", data).ToString();
                    result = Crypto.DecryptResponse(Convert.FromBase64String(result));
                    ServerResponse<UserData> response = JsonConvert.DeserializeObject<ServerResponse<UserData>>(result);
                    switch (response.code)
                    {
                        case ServerCodes.API_CODE_GAME_NOT_FOUND:
                            throw new Exception("Игра не найдена!");

                        case ServerCodes.API_CODE_USER_NOT_FOUND:
                            throw new Exception("Пользователь не найден");

                        case ServerCodes.API_CODE_SUBSCRIPTION_EXPIRY:
                            throw new Exception("Не найдено активной подписки!");

                        case ServerCodes.API_CODE_HWID_ERROR:
                            throw new Exception("Аккаунт привязан к другому устройству");

                        case ServerCodes.API_CODE_USER_BLOCKED:
                            throw new Exception("Возможности аккаунта ограничены!");

                        case ServerCodes.API_CODE_SUBSCRIPTION_DUPLICATE:
                            throw new Exception("На ваше устройство уже оформлена подписка на эту же игру");

                        case ServerCodes.API_CODE_OK:
                            ClientData.Logged = true;
                            ClientData.Data = response.data;
                            ClientData.ZipPassword = Crypto.Sha256(Encoding.UTF8.GetBytes(
                                $"{Crypto.Sha256(Encoding.UTF8.GetBytes(TPassword.Password))}.{response.data.access_token}"));
                            Close();
                            break;
                        default:
                            throw new Exception("Неизвестная ошибка");
                    }
                }
                catch (Exception ex)
                {
                    AuthMessage.Content = ex.Message;
                    SnackbarMessage.IsActive = true;
                }
                finally
                {
                    AuthBtn.IsEnabled = true;
                    TEmail.IsEnabled = true;
                    TPassword.IsEnabled = true;
                }
            }
        }

        private void SnackbarMessage_OnActionClick(object sender, RoutedEventArgs e)
        {
            SnackbarMessage.IsActive = false;
        }

        private void RepassLink_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            Process.Start("http://alpha-cheat.io/form/reset_password");
        }

        private void RegLink_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            Process.Start("http://alpha-cheat.io/form/register");
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (!ClientData.Logged)
                Environment.Exit(0);
        }
    }
}
