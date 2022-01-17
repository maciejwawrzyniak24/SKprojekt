using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
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

namespace SKProjekt
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged
    {
        public ObservableCollection<string> RoomList { get; set; } =  new ObservableCollection<string>();

        private string roomText;
        public string RoomText
        {
            get { return roomText; }
            set
            {
                if (value != roomText)
                {
                    roomText = value;
                    OnPropertyChanged();
                }
            }
        }
        private string currentRoom = "";
        public string CurrentRoom
        {
            get { return currentRoom; }
            set
            {
                if (value != currentRoom)
                {
                    ServerConnection.LeaveRoom(currentRoom);
                    currentRoom = value;
                    OnPropertyChanged();
                    ServerConnection.EnterRoom(value);
                    CurrentRoomFull = "Pokój : " + currentRoom;
                }
            }
        }

        public string CurrentRoomFull
        {
            get {  return "Pokój : " + currentRoom; }
            set
            {
                OnPropertyChanged();
            }
        }

        public MainWindow()
        {
            InitializeComponent();
            this.DataContext = this;
            UsernameTextBox.Text = ServerConnection.Username;
            RoomList = new ObservableCollection<string>(ServerConnection.GetRoomList());
            Ref();
        }

        public async Task Refresh()
        {
            if(CurrentRoom != "")
            {
                RoomText = await Task.Run(() => ServerConnection.GetRoomMessages(CurrentRoom));
                RoomList = await Task.Run(() => new ObservableCollection<string>(ServerConnection.GetRoomList()));
                ListaPokoi.ItemsSource = null;
                ListaPokoi.ItemsSource = RoomList;
            }
        }

        private void ChangeUsername(object sender, RoutedEventArgs e)
        {
            ServerConnection.Username = UsernameTextBox.Text;
            MessageBox.Show("Zmieniono nazwe uzytkownika");
        }

        private async Task Ref()
        {
            while (true)
            {
                await Refresh();
                await Task.Delay(1000);
            }
        }

        private void TextBlock_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ClickCount >= 2)
            {
                CurrentRoom = ((TextBlock)sender).Text;
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));

        }

        private void SendMessage(object sender, RoutedEventArgs e)
        {
            if (CurrentRoom != "")
            {
                ServerConnection.SendMessage(Wiadomosc.Text, CurrentRoom);
                string temp = ServerConnection.GetRoomMessages(CurrentRoom);
                RoomText = temp;
                Wiadomosc.Text = "";
            }
            else MessageBox.Show("Najpierw wejdz do pokoju");

        }

        private void NowyPokoj(object sender, RoutedEventArgs e)
        {
            ServerConnection.NewRoom(NowyPokojTekst.Text);
            CurrentRoom = NowyPokojTekst.Text;
        }

        private void Window_Closing(object sender, CancelEventArgs e)
        {
            ServerConnection.LeaveRoom(CurrentRoom);
        }
    }

}
