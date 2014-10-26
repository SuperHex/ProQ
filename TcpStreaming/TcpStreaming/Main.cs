using System;
using Gtk;
using System.Net.Sockets;
using System.Net;
using System.Diagnostics;
using System.Threading;
using System.Runtime.InteropServices;

namespace TcpStreaming
{

	class MainClass
	{
		[DllImport("user32.dll", SetLastError = true)]
		private static extern bool MoveWindow(IntPtr hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint);
		
		[DllImport("user32.dll")]
		private static extern IntPtr SetParent(IntPtr hWndChild, IntPtr hWndNewParent);

		public static void Main (string[] args)
		{
			Application.Init ();
			MainWindow win = new MainWindow ();
			win.Show ();


//			IPAddress server = IPAddress.Parse("192.168.0.106");
//			TcpListener itcp = new TcpListener(server, 8080);
//			itcp.Start();
//			System.Net.Sockets.Socket isocket = itcp.AcceptSocket();
//
//			if(isocket.Connected)
//			{
				var ffplay = new Process
				{
					StartInfo =
					{
						FileName = "/opt/local/bin/ffplay",
						Arguments = "tcp://192.168.0.115:8080",
						// hides the command window
						CreateNoWindow = true, 
						// redirect input, output, and error streams..
						RedirectStandardError = true,
						RedirectStandardOutput = true,
						UseShellExecute = false    
					}
				};

				ffplay.EnableRaisingEvents = true;
				ffplay.OutputDataReceived += (o, e) => Debug.WriteLine(e.Data ?? "NULL", "ffplay");
				ffplay.ErrorDataReceived += (o, e) => Debug.WriteLine(e.Data ?? "NULL", "ffplay");
				ffplay.Exited += (o, e) => Debug.WriteLine("Exited", "ffplay");
				ffplay.Start();
				
				Thread.Sleep(200); // you need to wait/check the process started, then...
				
				// child, new parent
				// make 'this' the parent of ffmpeg (presuming you are in scope of a Form or Control)
				SetParent(ffplay.MainWindowHandle, win.Handle);
				
				// window, x, y, width, height, repaint
				// move the ffplayer window to the top-left corner and set the size to 320x280
				MoveWindow(ffplay.MainWindowHandle, 0, 0, 320, 280, true);

				Application.Run();
			}
//		}
	}
}
