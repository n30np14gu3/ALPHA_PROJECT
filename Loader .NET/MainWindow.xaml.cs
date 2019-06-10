using System;
using System.Collections.Generic;
using System.IO;
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
using System.Windows.Navigation;
using System.Windows.Shapes;
using Loader.NET.SDK.Api;
using Loader.NET.SDK.Api.Structs;
using Loader.NET.SDK.Cryptography;
using MaterialDesignThemes.Wpf;


namespace Loader.NET
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private bool _isDrag;

        public MainWindow()
        {

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


        private void checkKeys()
        {

        }

        private void generateKeyPair()
        {

        }

        private void loadKeyPair()
        {

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
    }
}
